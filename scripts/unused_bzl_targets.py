#!/usr/bin/env python3

"""Report Bazel rules and external dependencies unused by the product roots."""

import ast
import io
import json
import os
import re
import subprocess
import sys
import tokenize
import xml.etree.ElementTree as ET
from pathlib import Path


ROOTS = (
    "//apps/... + //reality/app/xr/js/... + //reality/quality/... + "
    "//reality/engine/... + //c8/ecs/..."
)
ALL_RULES_QUERY = "kind(rule, //...)"
TEST_RULES_QUERY = 'kind(".*test.* rule", //...)'

# Every rule in the root trees is an entry point. //bzl/examples/... is deliberately not an entry
# point, so a rule used only by an example or a test remains a deletion candidate. Omit the test
# rules themselves from the report without preserving their dependency closures.
UNUSED_RULES_QUERY = (
    f"((kind(rule, //...) except deps({ROOTS})) except ({TEST_RULES_QUERY})) "
    "except //scripts:unused-bzl-targets"
)

# Query loaded Starlark files separately. Include //external so this also sees repositories loaded
# by WORKSPACE, such as rules_nodejs. Treat these repositories as used conservatively because
# Bazel's query output does not associate a load() with an individual rule in the loading file.
LOADED_REPOSITORIES_QUERY = 'filter("^@", buildfiles(//... + //external:*))'

# Repository rules created inside another repository macro carry a generator_function attribute.
# Keep the top-level WORKSPACE repositories and omit bind() aliases.
WORKSPACE_REPOSITORIES_QUERY = (
    'attr(generator_function, "^$", kind(rule, //external:*)) '
    "except kind(bind, //external:*)"
)


class CommandResult:
    def __init__(self, description, process):
        self.description = description
        self.returncode = process.returncode
        self.stdout = process.stdout
        self.stderr = process.stderr


def run(workspace, description, *args):
    print(description, file=sys.stderr, flush=True)
    process = subprocess.run(
        args,
        cwd=workspace,
        check=False,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    return CommandResult(description, process)


def bazel_query(workspace, description, expression, output="label"):
    return run(
        workspace,
        description,
        "bazel",
        "query",
        "--keep_going",
        "--noshow_progress",
        "--order_output=no",
        f"--output={output}",
        expression,
    )


def labels(output):
    return {line.strip() for line in output.splitlines() if line.strip()}


def repository_name(label):
    match = re.match(r"^@+([^/]+)//", label)
    return match.group(1) if match else None


def external_rule_name(label):
    prefix = "//external:"
    return label[len(prefix) :] if label.startswith(prefix) else None


def workspace_rule_repository_inputs(query_xml):
    """Return workspace rule labels and the repositories in their direct inputs."""
    repositories_by_rule = {}
    if not query_xml:
        return repositories_by_rule

    root = ET.fromstring(query_xml)
    for rule in root.findall("rule"):
        name = rule.get("name", "")
        if not name.startswith("//"):
            continue
        repositories_by_rule[name] = {
            repository
            for rule_input in rule.findall("rule-input")
            if (repository := repository_name(rule_input.get("name", ""))) is not None
        }
    return repositories_by_rule


def workspace_configuration_dependency_closure(query_xml):
    """Return configuration entry points and their internal rule dependencies."""
    if not query_xml:
        return set()

    root = ET.fromstring(query_xml)
    dependencies_by_rule = {}
    configuration_rules = set()
    for rule in root.findall("rule"):
        name = rule.get("name", "")
        if not name.startswith("//"):
            continue
        dependencies_by_rule[name] = {
            dependency
            for rule_input in rule.findall("rule-input")
            if (dependency := rule_input.get("name", "")).startswith("//")
        }
        rule_class = rule.get("class", "")
        if rule_class == "toolchain" or rule_class.endswith("_flag"):
            configuration_rules.add(name)

    closure = set(configuration_rules)
    pending = list(configuration_rules)
    while pending:
        rule = pending.pop()
        for dependency in dependencies_by_rule.get(rule, ()):
            if dependency in dependencies_by_rule and dependency not in closure:
                closure.add(dependency)
                pending.append(dependency)
    return closure


def workspace_repositories_and_inputs(query_xml):
    """Return top-level WORKSPACE repositories and their repository dependencies."""
    repositories = {}
    if not query_xml:
        return repositories

    root = ET.fromstring(query_xml)
    for rule in root.findall("rule"):
        name = external_rule_name(rule.get("name", ""))
        if name is None:
            continue
        repositories[name] = {
            repository
            for rule_input in rule.findall("rule-input")
            if (repository := repository_name(rule_input.get("name", ""))) is not None
        }
    return repositories


def propagate_repository_references(repository_dependencies, referenced_repositories):
    """Add dependencies of referenced repositories until the set stops growing."""
    pending = list(referenced_repositories)
    while pending:
        repository = pending.pop()
        for dependency in repository_dependencies.get(repository, ()):
            if dependency not in referenced_repositories:
                referenced_repositories.add(dependency)
                pending.append(dependency)


def top_level_bazel_dep_aliases(module_file):
    """Map module names to their apparent repository names from bazel_dep() calls."""
    source = module_file.read_text()
    tokens = list(tokenize.generate_tokens(io.StringIO(source).readline))
    aliases = {}
    depth = 0
    index = 0

    while index + 1 < len(tokens):
        token = tokens[index]
        next_token = tokens[index + 1]
        if (
            depth == 0
            and token.type == tokenize.NAME
            and token.string == "bazel_dep"
            and next_token.type == tokenize.OP
            and next_token.string == "("
        ):
            call_depth = 1
            cursor = index + 2
            kwargs = {}
            while cursor < len(tokens) and call_depth:
                current = tokens[cursor]
                if current.type == tokenize.OP:
                    if current.string in "([{":
                        call_depth += 1
                    elif current.string in ")]}":
                        call_depth -= 1
                if (
                    call_depth == 1
                    and current.type == tokenize.NAME
                    and cursor + 2 < len(tokens)
                    and tokens[cursor + 1].type == tokenize.OP
                    and tokens[cursor + 1].string == "="
                    and tokens[cursor + 2].type == tokenize.STRING
                ):
                    try:
                        kwargs[current.string] = ast.literal_eval(tokens[cursor + 2].string)
                    except (SyntaxError, ValueError):
                        pass
                cursor += 1

            name = kwargs.get("name")
            if isinstance(name, str):
                aliases[name] = kwargs.get("repo_name", name)
            index = cursor
            continue

        if token.type == tokenize.OP:
            if token.string in "([{":
                depth += 1
            elif token.string in ")]}":
                depth -= 1
        index += 1

    return aliases


def direct_modules(module_graph):
    graph = json.loads(module_graph)
    modules = []
    for dependency in graph.get("dependencies", []):
        key = dependency["key"]
        modules.append(key.rsplit("@", 1)[0])
    return modules


def print_section(title, values, summary=None):
    print()
    print(f"{title} ({summary if summary is not None else len(values)})")
    print("=" * 79)
    for value in values:
        print(value)


def target_package(label):
    if not label.startswith("//"):
        return None
    return label[2:].split(":", 1)[0]


def group_unused_directories(all_rules, unused_rules):
    """Separate wholly unused directory subtrees from individual rules."""
    total_by_directory = {}
    unused_by_directory = {}

    def count_directories(label, counts):
        package = target_package(label)
        if not package:
            return
        parts = package.split("/")
        for length in range(1, len(parts) + 1):
            directory = "/".join(parts[:length])
            counts[directory] = counts.get(directory, 0) + 1

    for label in all_rules:
        count_directories(label, total_by_directory)
    for label in unused_rules:
        count_directories(label, unused_by_directory)

    wholly_unused = {
        directory
        for directory, total in total_by_directory.items()
        if total > 1 and unused_by_directory.get(directory) == total
    }
    collapsed_directories = []
    for directory in sorted(wholly_unused, key=lambda value: (value.count("/"), value)):
        parts = directory.split("/")
        if any("/".join(parts[:length]) in wholly_unused for length in range(1, len(parts))):
            continue
        collapsed_directories.append(directory)

    def collapsed_ancestor(label):
        package = target_package(label)
        if not package:
            return None
        return next(
            (
                directory
                for directory in collapsed_directories
                if package == directory or package.startswith(f"{directory}/")
            ),
            None,
        )

    directory_entries = [f"//{directory}/..." for directory in collapsed_directories]
    rule_entries = [label for label in unused_rules if collapsed_ancestor(label) is None]
    return sorted(directory_entries), sorted(rule_entries)


def print_diagnostics(results):
    for result in results:
        if result.returncode == 0:
            continue
        print(file=sys.stderr)
        print(
            f"Diagnostics from '{result.description}' (exit {result.returncode}):",
            file=sys.stderr,
        )
        # Bazel can repeat the same package-loading failure for many targets. Preserve ordering while
        # collapsing identical messages so the useful failure is easy to spot.
        seen = set()
        for line in result.stderr.splitlines():
            if line not in seen:
                print(line, file=sys.stderr)
                seen.add(line)


def main():
    workspace = Path(os.environ.get("BUILD_WORKSPACE_DIRECTORY", Path.cwd()))
    if not (workspace / "MODULE.bazel").is_file():
        print("Error: run this tool from the 8th Wall workspace.", file=sys.stderr)
        return 2

    all_rules_result = bazel_query(
        workspace,
        "Finding workspace rules and their direct inputs...",
        ALL_RULES_QUERY,
        output="xml",
    )
    unused_rules_result = bazel_query(
        workspace,
        "Finding unused workspace rules...",
        UNUSED_RULES_QUERY,
    )
    loaded_repositories_result = bazel_query(
        workspace,
        "Finding repositories loaded by workspace build files...",
        LOADED_REPOSITORIES_QUERY,
    )
    workspace_repositories_result = bazel_query(
        workspace,
        "Finding top-level WORKSPACE repositories...",
        WORKSPACE_REPOSITORIES_QUERY,
        output="xml",
    )
    module_graph_result = run(
        workspace,
        "Finding direct MODULE.bazel dependencies...",
        "bazel",
        "mod",
        "graph",
        "--depth=1",
        "--output=json",
    )
    results = [
        all_rules_result,
        unused_rules_result,
        loaded_repositories_result,
        workspace_repositories_result,
        module_graph_result,
    ]

    loaded_repositories = {
        name
        for label in labels(loaded_repositories_result.stdout)
        if (name := repository_name(label)) is not None
    }
    repository_dependencies = workspace_repositories_and_inputs(
        workspace_repositories_result.stdout
    )
    workspace_repositories = set(repository_dependencies)
    unused_rules = labels(unused_rules_result.stdout)
    # Toolchain registration, command-line flags, aspects, and transitions create configuration-time
    # edges which do not reliably appear in deps().
    unused_rules -= workspace_configuration_dependency_closure(all_rules_result.stdout)
    repositories_by_rule = workspace_rule_repository_inputs(all_rules_result.stdout)
    repositories_referenced_by_unused_targets = set().union(
        *(repositories_by_rule.get(rule, set()) for rule in unused_rules)
    )
    used_repositories = loaded_repositories | set().union(
        *(
            repositories
            for rule, repositories in repositories_by_rule.items()
            if rule not in unused_rules
        )
    )
    propagate_repository_references(
        repository_dependencies,
        repositories_referenced_by_unused_targets,
    )
    propagate_repository_references(repository_dependencies, used_repositories)
    repositories_referenced_only_by_unused_targets = sorted(
        (workspace_repositories & repositories_referenced_by_unused_targets)
        - used_repositories
    )
    unreferenced_workspace_repositories = sorted(
        workspace_repositories
        - used_repositories
        - repositories_referenced_by_unused_targets
    )

    modules_referenced_only_by_unused_targets = []
    unreferenced_modules = []
    if module_graph_result.stdout:
        aliases = top_level_bazel_dep_aliases(workspace / "MODULE.bazel")
        for module in direct_modules(module_graph_result.stdout):
            apparent_name = aliases.get(module, module)
            suffix = "" if module == apparent_name else f" (repository @{apparent_name})"
            display_name = f"{module}{suffix}"
            if (
                apparent_name in repositories_referenced_by_unused_targets
                and apparent_name not in used_repositories
            ):
                modules_referenced_only_by_unused_targets.append(display_name)
            elif (
                apparent_name not in used_repositories
                and apparent_name not in repositories_referenced_by_unused_targets
            ):
                unreferenced_modules.append(display_name)

    unused_directories, individual_unused_rules = group_unused_directories(
        set(repositories_by_rule),
        unused_rules,
    )
    print_section("Wholly unused directories", unused_directories)
    print_section(
        "Unused Bazel rules",
        individual_unused_rules,
    )
    print_section(
        "Top-level WORKSPACE repositories directly referenced only by unused targets",
        repositories_referenced_only_by_unused_targets,
    )
    print_section(
        "Direct MODULE.bazel dependencies used only by unused targets",
        sorted(modules_referenced_only_by_unused_targets),
    )
    print_section(
        "Unreferenced top-level WORKSPACE repositories",
        unreferenced_workspace_repositories,
    )
    print_section(
        "Unreferenced direct MODULE.bazel dependencies",
        sorted(unreferenced_modules),
    )

    failed = [result for result in results if result.returncode != 0]
    print()
    if failed:
        print("INCOMPLETE: one or more Bazel queries failed; diagnostics follow on stderr.")
    else:
        print("Complete: all Bazel queries succeeded.")
    print(
        "Note: candidates may still be used through another external repository, "
        "configuration-time toolchain resolution, dynamic/string labels, or non-Bazel callers."
    )
    print_diagnostics(results)
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
