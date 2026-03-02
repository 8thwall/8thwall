# Pybind11
Pybind11 is an easy python-c++ binding framework to provide native class access to Python.

Pybind11
https://github.com/pybind/pybind11

In repo/niantic, pybind11 Bazel extension exists as part of Tensorflow dependency, which does not
seem to be the latest pybind11.

The original Pybind11 Bazel extension
https://github.com/pybind/pybind11_bazel

# How to build and run

*ONLY BUILD for MacOS and Linux for now*

The Pybind11 in this repo seems only to pick the local python env, while the repo/niantic
WORKSPACE uses Python 3.8 (as of Sep, 2023). If using different version of Python locally,
you cannnot `bazel run` as pybind11 builds the native module using local Python version and
`bazel run` uses Python 3.9. Either manually copy .so file and run on the local command line
env or use same version of Python locally.

To run the example, use this command
`bazel run //bzl/examples/pybind11:example`

# Notes
The latest Pybind11 Bazel extension has attribute to set to use the hermetic python, but the version
in the Tensorflow does not have that option. We may consider to bring this latest pybind11 bazel
extension in Workspace for better python version handling with bazel.
