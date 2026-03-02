// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Pawel Czarnecki (pawel@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":process",
    "//c8:c8-log",
    "@com_google_googletest//:gtest_main",
    ":vector",
    ":string",
  };
};
cc_end(0x39d3326a);

#include <chrono>
#include <iostream>
#include <thread>

#include "c8/c8-log.h"
#include "c8/process.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace std::chrono_literals;
using ::testing::ContainerEq;

namespace c8 {
namespace {

using namespace process;

class ProcessTest : public ::testing::Test {};

// tesiting various high-level against low-level cogs in this machine

TEST_F(ProcessTest, Echo) {
  Process echo("echo", {"echo", "really", "big", "foo"}, {1});
  auto resultPipes = echo();

  Vector<uint8_t> result;
  auto &pipe = resultPipes[1];
  String echoRes;
  while (pipe >> result) {
    echoRes += std::string(begin(result), end(result));
    std::this_thread::sleep_for(1ms);
  }
  EXPECT_EQ(echoRes, "really big foo\n");
}

TEST_F(ProcessTest, Echo2) {
  auto pipe = std::move(Process("echo", {"echo", "hello world"}, {C8_STDOUT})()[C8_STDOUT]);
  auto pipe2 = execute("echo", "hello world");

  Vector<uint8_t> temp;
  String res, res2;
  while (pipe >> temp) {
    res.insert(end(res), begin(temp), end(temp));
  }
  while (pipe2 >> temp) {
    res2.insert(end(res2), begin(temp), end(temp));
  }

  EXPECT_EQ(res, res2);
}

TEST_F(ProcessTest, Echo3) {
  auto pipe = execute("echo", "foo bar");
  auto pipe2 = execute("echo", "foo", "bar");

  // NOTE(pawel) in "real" programs it's better to manually read chunks when using non-blocking
  pipe.setNonBlocking();
  pipe2.setNonBlocking();

  auto res = getOutput(pipe);
  auto res2 = getOutput(pipe2);

  EXPECT_EQ(res, "foo bar\n");
  EXPECT_EQ(res, res2);
}

TEST_F(ProcessTest, PipeEchoGrep) {
  // redirect stdout
  Process echo(
    "echo",
    {"echo",
     "woah\n"
     "check it out\n"
     "im a pretty command line argument",
     "\nand im another"},
    {1});

  // redirect stdout
  Process grep("grep", {"grep", "out"}, {1});

  // redirect echo's fd 2 (stdout) to stdin
  // auto resultPipes = grep.launch(mapPipes(echo.launch(), {{1, 0}}));
  auto resultPipes = grep(stdOutToIn(echo()));

  Vector<uint8_t> result;
  resultPipes[1] >> result;

  auto grepOutput = std::string(begin(result), end(result));

  EXPECT_EQ(grepOutput, "check it out\n");
}

TEST_F(ProcessTest, Sedding) {
  Process echo("echo", {"echo", "foo bar foobar baby foo noo loo poo"}, {C8_STDOUT});
  Process sed1("sed", {"sed", "s/foo/massive/", "/dev/stdin"}, {C8_STDOUT});
  Process sed2("sed", {"sed", "s/baby/big boi/", "/dev/stdin"}, {C8_STDOUT});
  Process sed3("sed", {"sed", "s/poo/golden goose egg/", "/dev/stdin"}, {C8_STDOUT});

  auto chain = echo | sed1 | sed2 | sed3;

  Vector<uint8_t> temp;
  String res;
  while (chain >> temp) {
    res.insert(end(res), begin(temp), end(temp));
  }
  EXPECT_EQ(res, "massive bar foobar big boi foo noo loo golden goose egg\n");
}

TEST_F(ProcessTest, WriteToStdIn) {
  auto [input, output] = execute2("sed", "s/foo/goo/", "/dev/stdin");
  input << "foo";
  input.close();
  auto res = getOutput(output);
  // Normalize output because sed versions behave differently :/
  if (res == "goo\n") {
    res = "goo";
  }
  EXPECT_EQ(res, "goo");
}

TEST_F(ProcessTest, NonExisting) {
  bool errorCaught{false};
  try {
    auto [input, output] = execute2("foobarnonreal", "i am not real");
    input << "foo";
    input.close();
    auto res = getOutput(output);
    EXPECT_EQ(res, "loo");
  } catch (const ExecError &error) {
    errorCaught = true;
  }

  EXPECT_EQ(errorCaught, true);
}

static const char *tmpPath() { return std::getenv("TEST_TMPDIR"); }

TEST_F(ProcessTest, KillRunningDaemonKillCommand) {

  Process p{"git", {"git", "daemon", tmpPath()}};
  p();

  // this will fail if the pid is not running
  executeNoRedirect("kill", std::to_string(p.getPid()));
  EXPECT_EQ(0, p.getExitCode());
}

TEST_F(ProcessTest, KillRunningDaemonProcessApi) {

  Process p{"git", {"git", "daemon", tmpPath()}};
  p();

  // without this getExitCode() will hang and the test will timeout
  p.kill();
  EXPECT_EQ(0, p.getExitCode());
}

TEST_F(ProcessTest, PassEnv) {
  String testEnv = "c8-process-env-test=b65d6440-17aa-43db-bbb5-58aa51ebd348";
  ExecuteOptions opts;
  opts.file = "env";
  opts.env = {testEnv.c_str()};
  opts.redirectStout = true;
  auto res = execute(opts);
  String envOutput;

  Vector<uint8_t> bytes;
  while (*res.stdout >> bytes) {
    envOutput.insert(envOutput.end(), bytes.begin(), bytes.end());
  }
  EXPECT_NE(envOutput.find(testEnv), String::npos);
}

// This test fails with the default move constructor because primitives are copied, not swapped,
// on move and the destructor relies on _state (a primitive) to determine whether or not to wait.
// If the destructing process waits, the new one will get an ECHILD signal on waitpid() because
// the child is no longer found since previous already reaped it.
TEST_F(ProcessTest, MoveSemantics) {
  auto returnProcess = []() -> process::Process {
    Process p{"git", {"git", "status"}};
    p();
    return p;
  };

  {
    // std::move() to prevent copy elision to ensure that we trigger move constructor.
    process::Process p2{std::move(returnProcess())};
    p2.kill();
    EXPECT_EQ(0, p2.getExitCode());
  }
}

}  // namespace
}  // namespace c8
