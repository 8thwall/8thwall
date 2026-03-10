// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Pawel Czarnecki (pawel@8thwall.com)

#pragma once

// This process library was created for the simple purpose of executing other programs and
// obtaining their output in a strait-forward way. Included is a way to chain multiple programs
// together. You can also write to a process's stdin. Some low level functionality is exposed
// for flexibility (and more exotic needs). The high-level functionality allows you to run
// a process and obtain a pipe from which you can read the program's ouput. Non blocking I/O
// is supported by calling setNonBlocking() on a pipe.

// This library uses unix system class so it will work everywhere that they are available
// (and not blocked like on iOS). This library provides enough abstractions to be able to be
// adapted for other platforms such as Windows with some bazel platform flags and #ifdef's
// As of January 2020 Windows support is not something we need so it is not included here

// NOTE(pawel) generate a blank file for when host target is emscripten
#ifndef JAVASCRIPT

#include <fcntl.h>
#include <signal.h>
#include <unistd.h>  // NOTE(pawel) not cross platform

#include <optional>
#include <tuple>

#include "c8/exceptions.h"
#include "c8/map.h"
#include "c8/set.h"
#include "c8/string.h"
#include "c8/string/format.h"
#include "c8/vector.h"

namespace c8 {

struct ExecError : public RuntimeError {
  ExecError(const char *progName, int code)
      : RuntimeError(
        format("execvp(%s) failed in child: %s (%d)", progName, strerror(code), code).c_str()),
        _code(code) {}

  int getChildExecCode() { return _code; }
  const char *getDescription() { return strerror(_code); }

private:
  int _code;
};

namespace process {

// Intended usage (chunking)
//
// auto pipe = execute("git-lfs", "smudge", "/path/to/file");
// auto res = getOutput(pipe);    // NOTE this runs the chunked reader in a loop so non-blocking
//                                        mode is not recommended
//
//
// additionally: for chunked reader, pipe >> temp returns false when EOF is reached
//
// auto pipe = execute("git-lfs", "/path/to/file");
// pipe.setNonBlocking(); // OPTIONAL
// Vector temp;
// String res;
// while (pipe >> temp) {
//  res.insert(end(res), begin(temp), end(temp));
// }
//

// let's say pipes always go from left to right -> | ->
// so we write to the left and read from the right
// just a reminder that you write to pipe[1] and read from pipe[0]
constexpr int C8_PIPE_READER = 0;
constexpr int C8_PIPE_RIGHT = 0;
constexpr int C8_PIPE_WRITER = 1;
constexpr int C8_PIPE_LEFT = 1;

constexpr int C8_STDIN = 0;
constexpr int C8_STDOUT = 1;
constexpr int C8_STDERR = 2;

enum class PipeMode {
  DEFAULT,
  READER,
  WRITER,
  CLOSED,
};

enum class RedirectDirection {
  REDIRECT_CHILD_READER,
  REDIRECT_CHILD_WRITER,
};

// typical usage would be:
// (1) make pipe, (2) fork, (3) closeReader in one of the processes, (4) closeWriter in the other
// it is required to markerReader() before reading or markerWriter() before writing
// the mode can only be set once
// this is enforced to catch unintended bugs
class Pipe {
public:
  // some (mostly older) programs are no smart when reading from a non-blocking stdin.
  //
  explicit Pipe();
  ~Pipe();

  Pipe(const Pipe &) = delete;  // no copies allowed
  Pipe &operator=(const Pipe &) = delete;
  Pipe(Pipe &&other);  // but we can move things
  Pipe &operator=(Pipe &&other);

  // closes the write end of this pipe; throws an error if this or makeWriter() was already called
  void makeReader();

  // closes the read end of this pipe; throws an error if this or makeReader() was already called
  void makeWriter();

  // calls dup2() on the reader side of the pipe; makeReader() must have been called
  void dup2Reader(int targetFd);

  // calls dup2() on the writer side of the pipe; makeWriter() must have been called
  void dup2Writer(int targetFd);

  // blocking write to this pipe; unwritten data is returned
  std::optional<Vector<uint8_t>> operator<<(const Vector<uint8_t> &source_bytes);
  Pipe &operator<<(const String &source);

  // blocking write; returns pointer to byte just after last one written
  // i.e. pointer to next segment to be written.
  const char *writeBytes(const char *data, const size_t size);
  const uint8_t *writeBytes(const uint8_t *data, const size_t size);

  // returns true if EOF is reached. blocks until the requested number of bytes
  // is read or until eof is reached.
  bool readBytes(Vector<uint8_t> &dest, const size_t numToRead);

  // blocking read from this pipe; returns true if EOF has been encountered
  // contents of vector are overwritten (as I'm sure is obvious)
  bool operator>>(Vector<uint8_t> &dest_bytes);

  bool isReader() { return _mode == PipeMode::READER; }
  bool isWriter() { return _mode == PipeMode::WRITER; }
  bool isClosed() { return _mode == PipeMode::CLOSED; }

  void close();

  void setNonBlocking();

  // the returned pipes depend on each other; keep them in same scope.
  // the returned order is [reader, writer].
  static std::tuple<Pipe, Pipe> makeConnectedPair();

private:
  Pipe(int pipe[2]) {
    _pipe[0] = pipe[0];
    _pipe[1] = pipe[1];
  }
  int _pipe[2] = {-1, -1};
  PipeMode _mode{PipeMode::DEFAULT};
  bool _readEof{false};  // if we encountered and EOF this will be set
};

using PipeMap = TreeMap<int, Pipe>;

enum class ProcessState { DEFAULT, STARTED, WAITING, EXITED, FAILED };

// describes the setup for a process, instantiation does not launch it
class Process {
public:
  // position 0 of the arguments is the name of the program
  // some programs behave detect the name by which they were invoked and behave differently
  // busybox is a great example of this behavior
  explicit Process() = default;
  Process(
    const String &progName,
    const Vector<String> &arguments,
    const TreeSet<int> &redirectDescriptors = {},
    const Vector<String> &env = {})
      : _progName(std::move(progName)),
        _arguments(std::move(arguments)),
        _env(std::move(env)),
        _descriptorsToRedirect(std::move(redirectDescriptors)) {}
  ~Process();

  Process(const Process &) = delete;
  Process &operator=(const Process &) = delete;

  // The default move constructor does not swap primitive values which means that the other's
  // destructor will wait on it unless we set other's state to DEFAULT.
  Process(Process &&other) {
    _progName = std::move(other._progName);
    _arguments = std::move(other._arguments);
    _env = std::move(other._env);
    _workingDirectory = std::move(other._workingDirectory);
    _descriptorsToRedirect = std::move(other._descriptorsToRedirect);
    std::swap(_pid, other._pid);
    std::swap(_childExecError, other._childExecError);
    std::swap(detached_, other.detached_);
    // Puts other into the DEFAULT state so the destructor doesn't wait for it.
    std::swap(_state, other._state);
  }
  Process &operator=(Process &&other) = delete;

  // maps input pipes to process, redirects output descriptors as requested by
  // _descriptorsToRedirect performs the fork and runs the child process
  PipeMap operator()(PipeMap &&inputPipes = {});
  PipeMap operator()(PipeMap &inputPipes);

  // value is valid only when process state == FAILED
  int getChildExecError() { return _childExecError; }

  // only valid when state == STARTED || state == EXITED
  // blocks when status is STARTED
  int getExitCode();

  // can be set before the process is launched
  void setWorkDirectory(String directory);

  // valid once sucessfully launched
  pid_t getPid() { return _pid; }

  // valid once sucessfully launched
  int kill(int signal = SIGTERM);

  // Don't wait for the process when it goes out of scope.
  void detach() { detached_ = true; }

private:
  // can be just executable name since we use execvp, which does a PATH search
  String _progName;

  // arguments that are passed to the program, 0th position is the program name
  Vector<String> _arguments;

  // environment to set for the program
  Vector<String> _env;

  // optionally set work directory to cd into before spawning
  std::optional<String> _workingDirectory;

  // parent has child's pid after launch
  pid_t _pid = 0;

  // after call to getExistStatus()
  int _exitStatus;

  // these filedescriptors will be piped as writers on the child side
  // and returned from launch() as readers
  TreeSet<int> _descriptorsToRedirect;

  ProcessState _state = ProcessState::DEFAULT;

  // this is the return value of execvp() in the child process if the function failed
  int _childExecError;

  bool detached_ = false;
};

class ProcessChain {
public:
  ProcessChain() = default;
  ~ProcessChain() = default;

  ProcessChain(ProcessChain &&other) = default;

  ProcessChain operator|(Process &right);

  bool operator>>(Vector<uint8_t> &dest_bytes);

private:
  Vector<Process> _processes;
  PipeMap _output;
  friend ProcessChain operator|(Process &left, Process &right);
};

PipeMap mapPipes(PipeMap input, Vector<std::tuple<int, int>> transform);

PipeMap stdOutToIn(PipeMap input);

// when two process are launch, happens in the case of auto chain = echo | grep
ProcessChain operator|(Process &left, Process &right);

struct ExecuteOptions {
  String file;
  std::optional<String> workDirectory;
  Vector<String> env;
  bool redirectStdin{false};
  bool redirectStout{false};
  bool redirectStderr{false};
};

struct Execute2Options {
  String file;
  String workDirectory;
};

struct ExecuteResult {
  ExecuteResult(Process &&p) : process(std::move(p)) {}

  std::optional<Pipe> stdin;
  std::optional<Pipe> stdout;
  std::optional<Pipe> stderr;
  Process process;
};

ExecuteResult execute(const ExecuteOptions &opts, Vector<String> args = {});

// given a program name and arguments, return an stdout pipe for the process
template <typename... Args>
Pipe execute(const String &file, const Args... args) {
  auto found = file.find_last_of("/");
  auto name = found == (String::npos) ? file : file.substr(found + 1);
  return std::move(Process(file, {name, args...}, {C8_STDOUT})()[C8_STDOUT]);
}

// given program name and args, executs the process and returns the process object
template <typename... Args>
Process executeNoRedirect(const String &file, const Args... args) {
  auto found = file.find_last_of("/");
  auto name = (found == String::npos) ? file : file.substr(found + 1);
  auto p = Process(file, {name, args...});
  p();
  return p;
}

// given a program name and args, executes process and returns input and output pipes
template <typename... Args>
std::tuple<Pipe, Pipe> execute2(const String &file, const Args &...args) {
  auto found = file.find_last_of("/");
  auto name = (found == String::npos) ? file : file.substr(found + 1);

  PipeMap input;
  input[0] = Pipe();
  auto p = Process(file, {name, args...}, {C8_STDOUT});
  p.detach();
  auto output = std::move(p(input)[C8_STDOUT]);
  return {std::move(input[0]), std::move(output)};
}

template <typename... Args>
std::tuple<Pipe, Pipe> execute2(const Execute2Options &opts, const Args &...args) {
  auto found = opts.file.find_last_of("/");
  auto name = (found == String::npos) ? opts.file : opts.file.substr(found + 1);

  PipeMap input;
  input[0] = Pipe();
  ExecuteResult res(Process(opts.file, {name, args...}, {C8_STDOUT}));
  res.process.detach();
  res.process.setWorkDirectory(opts.workDirectory);
  auto running = res.process(input);
  return {std::move(input[0]), std::move(running[C8_STDOUT])};
}

// given program name and args, execute process and return input pipe and process object
template <typename... Args>
std::tuple<Pipe, Process> execute2NoRedirect(const String &file, const Args &...args) {
  auto found = file.find_last_of("/");
  auto name = (found == String::npos) ? file : file.substr(found + 1);

  PipeMap input;
  input[0] = Pipe();
  auto p = Process(file, {name, args...});
  auto output = p(input);
  return {std::move(input[0]), std::move(p)};
}

// give program name and args, returns input, output pipes and process object
template <typename... Args>
std::tuple<Pipe, Pipe, Process> execute3(const String &file, const Args &...args) {
  auto found = file.find_last_of("/");
  auto name = (found == String::npos) ? file : file.substr(found + 1);

  PipeMap input;
  input[0] = Pipe();
  auto p = Process(file, {name, args...}, {C8_STDOUT});
  auto output = std::move(p(input)[C8_STDOUT]);
  return {std::move(input[0]), std::move(output), std::move(p)};
}

String getOutput(Pipe &pipe);

Vector<uint8_t> getRawOutput(Pipe &pipe);

}  // namespace process
}  // namespace c8

#endif  // JAVASCRIPT
