#include <filesystem>
#include <iostream>

#include "load-spz.h"

std::string help = R"""(spz - Convert ply <-> spz files.

Usage:
  spz <input> [<output>]
  spz -h | --help

If no output is provided, the output file will have the same basename and location as the input
file, but with the extension changed to the opposite format (ply -> spz, spz -> ply). If an output
file is provided, the format will be inferred from the extension.
)""";

struct Args {
  bool printHelpAndExit = false;
  int errCode = 0;
  std::string input;
  std::string output;
};

enum class FileType {
  UNKNOWN = 0,
  PLY = 1,
  SPZ = 2,
};

FileType fileType(const std::string &path) {
  auto ext = std::filesystem::path(path).extension();
  if (ext == ".ply") {
    return FileType::PLY;
  } else if (ext == ".spz") {
    return FileType::SPZ;
  }
  return FileType::UNKNOWN;
}

Args parseArgs(int argc, const char **argv) {
  Args args;
  if (argc < 2 || argc > 3) {
    std::cerr << "spz: Invalid invocation." << std::endl << std::endl;
    args.printHelpAndExit = true;
    args.errCode = 1;
    return args;
  }

  if (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help") {
    args.printHelpAndExit = true;
    args.errCode = 1;
    return args;
  }
  args.input = argv[1];
  if (!std::filesystem::exists(args.input)) {
    std::cerr << "spz: Input file does not exist." << std::endl;
    args.errCode = 1;
    return args;
  }

  auto ext = fileType(args.input);
  if (ext == FileType::UNKNOWN) {
    std::cerr << "spz: Couldn't infer file type of input" << std::endl;
    args.errCode = 1;
    return args;
  }

  if (argc == 3) {
    args.output = argv[2];
  } else {
    if (ext == FileType::PLY) {
      args.output = args.input.substr(0, args.input.size() - 4) + ".spz";
    } else {
      args.output = args.input.substr(0, args.input.size() - 4) + ".ply";
    }
  }

  return args;
}

int main(int argc, const char **argv) {
  auto args = parseArgs(argc, argv);
  if (args.printHelpAndExit || args.errCode != 0) {
    std::cout << help << std::endl;
    return args.errCode;
  }

  spz::UnpackOptions u = {.to = spz::CoordinateSystem::RUB};
  auto gaussianCloud = fileType(args.input) == FileType::PLY ? spz::loadSplatFromPly(args.input, u)
                                                             : spz::loadSpz(args.input, u);

  if (gaussianCloud.numPoints <= 0) {
    std::cerr << "spz: Failed to load input file." << std::endl;
    return 1;
  }

  spz::PackOptions p = {.from = spz::CoordinateSystem::RUB};
  int success = fileType(args.output) == FileType::PLY
    ? spz::saveSplatToPly(gaussianCloud, p, args.output)
    : spz::saveSpz(gaussianCloud, p, args.output);

  if (!success) {
    std::cerr << "spz: Failed to save output file." << std::endl;
    return 1;
  } else {
    std::cout << "spz: Conversion successful. Wrote: '" << args.output << "'" << std::endl;
  }

  return 0;
}
