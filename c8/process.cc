// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Pawel Czarnecki (pawel@8thwall.com)

// NOTE(pawel) generate a blank file for when host target is emscripten
// see process.h for additional information

#ifndef JAVASCRIPT

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "process.h",
  };
  visibility = {
    "//visibility:public",
  };
  deps = {
    "//c8:exceptions",
    "//c8/string:strcat",
    "//c8/string:format",
    "//c8:vector",
    "//c8:map",
    "//c8:set",
  };
};
cc_end(0xc90342b9);

#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#include <chrono>
#include <thread>

#include "c8/process.h"
#include "c8/string/strcat.h"

using namespace std::chrono_literals;

namespace c8 {
namespace process {

Pipe::Pipe() {
  // NOTE(pawel) macOS does not support pipe2 so we need to use fcntl
  if (pipe(_pipe) == -1) {
    C8_THROW(c8::format("could not pipe (errno: %d)", errno));
  }
}

Pipe::~Pipe() { close(); }

void Pipe::close() {
  if (_pipe[C8_PIPE_LEFT] != -1) {
    ::close(_pipe[C8_PIPE_LEFT]);
  }
  if (_pipe[C8_PIPE_RIGHT] != -1) {
    ::close(_pipe[C8_PIPE_RIGHT]);
  }
  _mode = PipeMode::CLOSED;
}

// this allows our reads and writes to be non-blocking. this way the read call returns false
// and does not return a buffer
void Pipe::setNonBlocking() {
  if (_pipe[C8_PIPE_READER] != -1) {
    if (
      fcntl(_pipe[C8_PIPE_READER], F_SETFL, fcntl(_pipe[C8_PIPE_READER], F_GETFL) | O_NONBLOCK)
      < 0) {
      C8_THROW("could not make pipe non blocking");
    }
  }
  if (_pipe[C8_PIPE_WRITER] != -1) {
    if (
      fcntl(_pipe[C8_PIPE_WRITER], F_SETFL, fcntl(_pipe[C8_PIPE_WRITER], F_GETFL) | O_NONBLOCK)
      < 0) {
      C8_THROW("could not make pipe non blocking");
    }
  }
}

Pipe &Pipe::operator=(Pipe &&other) {
  std::swap(_pipe[0], other._pipe[0]);
  std::swap(_pipe[1], other._pipe[1]);
  std::swap(_mode, other._mode);
  std::swap(_readEof, other._readEof);
  return *this;
}

Pipe::Pipe(Pipe &&other) { *this = std::move(other); }

// closes the write end of the pipe
void Pipe::makeReader() {
  if (_mode != PipeMode::DEFAULT && !isReader()) {
    C8_THROW("pipe mode already set to something other than reader");
  }
  if (_pipe[C8_PIPE_WRITER] != -1) {
    ::close(_pipe[C8_PIPE_WRITER]);
    _pipe[C8_PIPE_WRITER] = -1;
  }
  _mode = PipeMode::READER;
}

// closes the read end of the pipe
void Pipe::makeWriter() {
  if (_mode != PipeMode::DEFAULT) {
    C8_THROW("pipe mode already set to something other than writer");
  }
  if (_pipe[C8_PIPE_READER] != -1) {
    ::close(_pipe[C8_PIPE_READER]);
    _pipe[C8_PIPE_READER] = -1;
  }
  _mode = PipeMode::WRITER;
}

void Pipe::dup2Reader(int targetFd) {
  if (dup2(_pipe[C8_PIPE_READER], targetFd) == -1) {
    C8_THROW(strCat("failed to dup2 ", errno));
  }
}

void Pipe::dup2Writer(int targetFd) {
  if (dup2(_pipe[C8_PIPE_WRITER], targetFd) == -1) {
    C8_THROW(strCat("failed to dup2 ", errno));
  }
}

// returns [reader, writer]
std::tuple<Pipe, Pipe> Pipe::makeConnectedPair() {
  int pipe[2] = {-1, -1};
  if (::pipe(pipe) == -1) {
    C8_THROW(c8::format("could not pipe (errno: %d)", errno));
  }
  Pipe reader(pipe);
  reader._pipe[C8_PIPE_WRITER] = -1;
  reader._mode = PipeMode::READER;

  Pipe writer(pipe);
  writer._pipe[C8_PIPE_READER] = -1;
  writer._mode = PipeMode::WRITER;

  return {std::move(reader), std::move(writer)};
}

// somePipe << someBuffer
// returns unwritten data that would otherwise block
std::optional<Vector<uint8_t>> Pipe::operator<<(const Vector<uint8_t> &source_bytes) {
  if (!isWriter()) {
    C8_THROW("can not write buffer to pipe not marked as writer");
  }

  int written = write(_pipe[C8_PIPE_WRITER], source_bytes.data(), source_bytes.size());
  if (written == -1 && errno != EAGAIN && errno != EINTR && errno != EWOULDBLOCK) {
    C8_THROW(strCat("could not write to pipe, errno=", errno));
  }

  if (written == source_bytes.size()) {
    return std::nullopt;
  }

  Vector<uint8_t> rest(begin(source_bytes) + written, begin(source_bytes) + written + 1);
  return rest;
}

// write to pipe and return pointer to byte just after last one written,
// note that partial writes are possible in cases of EINTR.
const char *Pipe::writeBytes(const char *data, size_t size) {
  if (!isWriter()) {
    C8_THROW("cannot write buffer to pipe not marked as writer");
  }

  auto written = write(_pipe[C8_PIPE_WRITER], data, size);
  if (written == -1 && errno != EAGAIN && errno != EINTR && errno != EWOULDBLOCK) {
    C8_THROW(strCat("could not write to pipe, errno=", errno));
  }

  return data + written;
}

const uint8_t *Pipe::writeBytes(const uint8_t *data, size_t len) {
  return reinterpret_cast<const uint8_t *>(writeBytes(reinterpret_cast<const char *>(data), len));
}

// returns true if EOF is reached
bool Pipe::readBytes(Vector<uint8_t> &dest, const size_t numToRead) {
  if (!isReader()) {
    C8_THROW("cannot read from pipe not marked as reader");
  }

  dest.resize(numToRead);  // ensure capacity.
  size_t totalRead{0};
  int actuallyRead;

  while (totalRead < numToRead) {
    actuallyRead = read(_pipe[C8_PIPE_READER], dest.data() + totalRead, numToRead - totalRead);

    if (actuallyRead == -1) {
      if (errno != EAGAIN && errno != EINTR && errno != EWOULDBLOCK) {
        C8_THROW(strCat("could not read from pip, errno=", errno));
      }
    } else if (actuallyRead == 0) {
      dest.resize(totalRead);
      return true;  // eof
    } else {
      totalRead += actuallyRead;
    }
  }
  return false;
}

// TODO(pawel) use raw data like writeBuffer()
Pipe &Pipe::operator<<(const String &source) {
  std::optional<Vector<uint8_t>> remaining = Vector<uint8_t>(begin(source), end(source));
  do {
    remaining = this->operator<<(*remaining);
  } while (remaining);
  return *this;
}

// if (somePipr >> destinationVector) { /* EOF has not been reached */ }
bool Pipe::operator>>(Vector<uint8_t> &dest_bytes) {
  if (!isReader()) {
    C8_THROW("can not read from pipe not marked as reader");
  }
  dest_bytes.clear();

  if (_readEof) {
    return false;
  }

  constexpr size_t bufferSegmentSize = 512;
  uint8_t buffer[bufferSegmentSize];

  while (true) {
    auto res = read(_pipe[C8_PIPE_READER], buffer, bufferSegmentSize);
    if (res > 0) {
      dest_bytes.insert(end(dest_bytes), buffer, buffer + res);  // [first, last)
      return true;
    } else if (res == 0) {
      _readEof = true;
      return true;
    } else {
      if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK) {
        // this would be a blocking operation
        return true;  // the pipe is not yet closed
      } else {
        C8_THROW(strCat("failed to read from pipe (", errno, ") fd=", _pipe[C8_PIPE_READER]));
      }
    }
  }
}

Process::~Process() {
  if (!detached_ && _state == ProcessState::STARTED) {
    // waitpid() is a blocking syscall which can be interrupted so we call it again in this case.
    while (waitpid(_pid, &_exitStatus, 0) == -1 && errno == EINTR)
      ;
  }
}

void Process::setWorkDirectory(String directory) {
  if (_state != ProcessState::DEFAULT) {
    C8_THROW("cannot set work directory on process that was started");
  }
  _workingDirectory = directory;
}

int Process::kill(int signal) { return ::kill(_pid, signal); }

int Process::getExitCode() {
  if (_state != ProcessState::STARTED && _state != ProcessState::EXITED) {
    C8_THROW("cannot getExitStatus() for process that hasn't launched successfully");
  }
  if (_state == ProcessState::STARTED) {
    while (waitpid(_pid, &_exitStatus, 0) == -1) {
      if (errno == EINTR) {
        continue;
      }
      C8_THROW("waitpid() failed with errno %d pid=%d file=%s", errno, _pid, _progName.c_str());
    }
    _exitStatus = WEXITSTATUS(_exitStatus);
    _state = ProcessState::EXITED;
  }
  return _exitStatus;
};

// launch the child process; closing input pipes (useful for chaining)
PipeMap Process::operator()(PipeMap &&inputPipes) { return this->operator()(inputPipes); }

// launch child process; using predefined pipes
PipeMap Process::operator()(PipeMap &inputPipes) {
  if (_state != ProcessState::DEFAULT) {
    C8_THROW("cannot start an already started process");
  }

  PipeMap outputPipes;
  for (auto fd : _descriptorsToRedirect) {
    outputPipes[fd] = Pipe();
  }

  // here we do a neat little trick to see if execvp() secceeded
  // we create a temporary pipe
  // then we fork()
  // the child closes the read end; the parent closes the write end
  // the child set FD_CLOEXEC on the write end (close the descriptor on exec)
  // if there is an error the child will write the error info to the pipe then _exit()
  // if execvp() suceeds the pipe will be empty
  // the parent performas a blocking read; a read of 0 bytes means EOF and success
  // the presence of bytes means error, and we can throw an exception.

  int statusPipe[2];
  if (pipe(statusPipe) == -1) {
    C8_THROW("could not pipe");
  }

  while (true) {
    _pid = fork();

    if (_pid == 0) {
      // we are the child; configure our side of the pipes
      for (auto &[targetFd, pipe] : inputPipes) {
        pipe.makeReader();
        pipe.dup2Reader(targetFd);
      }
      for (auto &[targetFd, pipe] : outputPipes) {
        pipe.makeWriter();
        pipe.dup2Writer(targetFd);
      }

      // set up our side of the statusPipe
      ::close(statusPipe[C8_PIPE_READER]);
      fcntl(statusPipe[C8_PIPE_WRITER], F_SETFD, FD_CLOEXEC);

      // prepare command line arguments
      char const **args =
        (char const **)std::malloc((_arguments.size() + 1) * sizeof(char const *));
      for (int i = 0; i < _arguments.size(); i++) {
        args[i] = _arguments[i].c_str();
      }
      args[_arguments.size()] = 0;

      // try to change to the specified work directory
      if (_workingDirectory) {
        if (chdir(_workingDirectory->c_str()) != 0) {
          goto report_errno_exit;
        }
      }

      for (auto &env : _env) {
        // From the man pages:
        // The string pointed to by string becomes part of the environment.
        // A program should not alter or free the string, and should not use stack
        // or other transient string variables as arguments to putenv().

        // The function putenv() failed because string is a
        // NULL pointer or string is without an “=” character.
        if (putenv(env.data())) {
          C8Log("[process] Failed on putenv(): %s", env.c_str());
          goto report_errno_exit;
        }
      }

      // execute the target program
      execvp(_progName.c_str(), (char *const *)args);

    // if we make it to here, execution failed; we should tell the parent and bail
    report_errno_exit:
      write(statusPipe[C8_PIPE_WRITER], &errno, sizeof(int));
      _exit(1);

    } else if (_pid == -1) {
      if (errno == EAGAIN) {
        C8Log("fork() resulted in EAGAIN, trying again in 1 second");
        std::this_thread::sleep_for(1s);
        continue;
      }
      C8_THROW(strCat("could not fork ", errno));
    } else {
      // we are the parent; configure our side of the pipes
      for (auto &[targetFd, pipe] : inputPipes) {
        if (pipe.isReader()) {
          // if an input is a reader, that means it's write end has already been connected elsewhere
          // and we just connected it's read end to the subprocess, so we can close it
          pipe.close();
        } else {
          // since we connected the read end of this pipe, we'll mark it as a writer
          pipe.makeWriter();
        }
      }
      for (auto &[targetFd, pipe] : outputPipes) {
        // marking as reader closes the write end of the pipes
        pipe.makeReader();
      }

      // set up out side of statusPipe and perform the wait
      ::close(statusPipe[C8_PIPE_WRITER]);
      int res;
      do {
        res = read(statusPipe[C8_PIPE_READER], &_childExecError, sizeof(int));
      } while (res == -1 && errno == EINTR);
      ::close(statusPipe[C8_PIPE_READER]);

      if (res > 0) {
        _state = ProcessState::FAILED;
        throw ExecError(_progName.c_str(), _childExecError);
      } else if (res == 0) {
        _state = ProcessState::STARTED;
        return outputPipes;
      } else {
        C8_THROW("failed to read from statusPipe (%d)", errno);
      }
    }
  }
}

PipeMap mapPipes(PipeMap input, Vector<std::tuple<int, int>> transform) {
  PipeMap transformed;
  for (auto [outputFd, inputFd] : transform) {
    transformed[inputFd] = std::move(input[outputFd]);
  }
  return transformed;
}

PipeMap stdOutToIn(PipeMap input) { return mapPipes(std::move(input), {{C8_STDOUT, C8_STDIN}}); }

ProcessChain ProcessChain::operator|(Process &right) {
  _output = right(stdOutToIn(std::move(_output)));
  _processes.push_back(std::move(right));
  return std::move(*this);
}

bool ProcessChain::operator>>(Vector<uint8_t> &dest_bytes) {
  return _output[C8_STDOUT] >> dest_bytes;
}

ProcessChain operator|(Process &left, Process &right) {
  ProcessChain chain;
  chain._output = right(stdOutToIn(left()));
  chain._processes.push_back(std::move(left));
  chain._processes.push_back(std::move(right));
  return chain;
}

String getOutput(Pipe &pipe) {
  Vector<uint8_t> temp;
  String res;
  while (pipe >> temp) {
    res.insert(end(res), begin(temp), end(temp));
  }
  return res;
}

Vector<uint8_t> getRawOutput(Pipe &pipe) {
  Vector<uint8_t> temp, output;
  while (pipe >> temp) {
    output.insert(end(output), begin(temp), end(temp));
  }
  return output;
}

ExecuteResult execute(const ExecuteOptions &opts, Vector<String> args) {
  auto found = opts.file.find_last_of("/");
  auto name = found == (String::npos) ? opts.file : opts.file.substr(found + 1);
  PipeMap input;
  if (opts.redirectStdin) {
    input[C8_STDIN] = Pipe();
  }
  TreeSet<int> output;
  if (opts.redirectStout) {
    output.insert(C8_STDOUT);
  }
  if (opts.redirectStderr) {
    output.insert(C8_STDERR);
  }

  args.insert(args.begin(), name);
  ExecuteResult res(Process(opts.file, args, output, opts.env));
  if (opts.workDirectory) {
    res.process.setWorkDirectory(*opts.workDirectory);
  }
  // this is calling the process's operator() which launches the program
  auto running = res.process(input);
  if (opts.redirectStdin) {
    res.stdin = std::move(input[C8_STDIN]);
  }
  if (opts.redirectStout) {
    res.stdout = std::move(running[C8_STDOUT]);
  }
  if (opts.redirectStderr) {
    res.stderr = std::move(running[C8_STDERR]);
  }
  return res;
}

}  // namespace process
}  // namespace c8

#endif  // JAVASCRIPT
