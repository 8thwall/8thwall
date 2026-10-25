import os
import subprocess
from pathlib import Path


cwd = Path.cwd()
execroot = next(
    (path for path in (cwd, *cwd.parents) if (path / "external" / "npm-capnpc-js").exists()),
    None,
)
if execroot is None:
    raise RuntimeError(f"Could not locate the Bazel execroot from {cwd}")

node = execroot / os.environ["NODE"]
script = execroot / "external/npm-capnpc-js/node_modules/capnpc-js/bin/capnpc-js.js"
raise SystemExit(subprocess.run((node, script), check=False).returncode)
