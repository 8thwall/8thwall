#include <CLI/CLI.hpp>

#include "apps/g8/g8-helpers.h"

CLI::App_p keepHouseCmd(MainContext &);
CLI::App_p inspectCmd(MainContext &);
CLI::App_p upgradeCmd();
CLI::App_p cloneCmd(MainContext &);
CLI::App_p newClientCmd(MainContext &);
CLI::App_p presubmitCmd(MainContext &);
CLI::App_p syncMasterCmd(MainContext &);
CLI::App_p lsCmd(MainContext &);
