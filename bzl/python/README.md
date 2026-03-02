
Here we define all our third party python dependencies for the entire repo

To add or edit python dependencies:
* Ensure poetry is installed (https://python-poetry.org/)
* Modify pyproject.toml.  Can do this directly or via poetry commands (eg. `poetry add requests`)
* Run `update_requirements_txt` which will update requirements.txt
* Done.  These changes should be picked up now by bazel commands

The `a4lidartag_requirements.txt` file lives here since it is referenced in the `WORKSPACE` file. However, these dependencies are not added to the monorepo ones since they include heavy ones otherwise unused, like `opencv` and `torch`. Note, these are updated with a different script (`argeo/photon/Post-Processing/calibration/a4lidartag_ext/README.md` for more details).
