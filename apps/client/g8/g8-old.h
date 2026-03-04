// Copyright (c) 2022 8th Wall, LLC.

// This file is transient to assist in refactoring the mega 3,000 line g8.cc file into
// more manageable chunks.

#include <CLI/CLI.hpp>

#include "c8/c8-log.h"

c8::String repoHost(const c8::String &repo);

CLI::App_p syncCmd(MainContext &);

CLI::App_p patchCmd(MainContext &);

CLI::App_p drySyncCmd(MainContext &);

CLI::App_p newchangeCmd(MainContext &);

CLI::App_p updateCmd(MainContext &);

CLI::App_p changeCmd(MainContext &);

CLI::App_p clientCmd(MainContext &);

CLI::App_p saveCmd(MainContext &);

CLI::App_p statusCmd(MainContext &);

CLI::App_p sendCmd(MainContext &);

CLI::App_p landCmd(MainContext &);

CLI::App_p filesCmd(MainContext &);

CLI::App_p addCmd(MainContext &);

CLI::App_p unaddCmd(MainContext &);

CLI::App_p diffCmd(MainContext &);

CLI::App_p logCmd(MainContext &);

CLI::App_p revertCmd(MainContext &);

CLI::App_p fetchRemoteClientsCmd(MainContext &);

CLI::App_p initialCommitCmd(MainContext &);

CLI::App_p infoCmd(MainContext &);

CLI::App_p prCmd(MainContext &);

CLI::App_p mrCmd(MainContext &);
