# Task issue_925 Progress

## Architecture & Key Decisions

- The issue #925 requires a new `AGENTS.md` file at the root of the
  repository containing: 1) the project architecture, 2) the project
  structure, 3) the name of the maintainer, 4) the contribution process,
  5) instructions for building and testing the project.
- The content is based on the existing project sources (CMakeLists.txt,
  src/, submodules/, tests/, .github/workflows/), the existing
  documentation (README.md, CONTRIBUTING.md, md/) and the git history.
- The file targets AI agents / automated coding assistants, so it is
  kept concise, factual and machine-actionable (concrete commands,
  concrete paths, no placeholders).

## Checklist / Sub-tasks

- [x] Read the requirements of the issue #925
- [x] Update the repository (git pull) and create the task_925 branch from main
- [x] Publish the initial checklist of the issue
- [x] Investigate the project architecture and structure (src, submodules, tests, CI)
- [x] Identify the maintainer of the project (Andrei Yankovich / EndrII)
- [x] Collect the contribution workflow (CONTRIBUTING.md, CI, PR process)
- [x] Collect the build and testing instructions (CMake options, Qt version, test suites)
- [x] Write the AGENTS.md file with all 5 required sections
- [x] Run the build and the test suite to verify the documented commands
  (Qt 6.10.3 Linux host: the configure + build succeeded, the full test
  suite of the 16 test classes ran with 0 failed tests)
- [x] Run the git diff check and verify the content
- [x] Push the task_925 branch and create the PR (https://github.com/QuasarApp/CQtDeployer/pull/927)
- [x] Set the waiting-for-review label and post the result comment

## Blockers / Questions

- None.