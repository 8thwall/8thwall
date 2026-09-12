Here we define all our third party python dependencies for the entire repo

To add or edit python dependencies:
* Ensure poetry is installed (https://python-poetry.org/)
* Modify pyproject.toml.  Can do this directly or via poetry commands (eg. `poetry add requests`)
* Run `update_requirements_txt` which will update requirements.txt
* Done.  These changes should be picked up now by bazel commands
