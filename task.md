# Task issue_456 Progress

## Architecture & Key Decisions

- New option `-license {path/to/licenseFile}` (registered in the options table
  of the `DeployCore::avilableOptions()`, Part 3 "Control of packages options").
  The option value is the path to the license file (plain text, e.g. `LICENSE.txt`).
- **Qt Installer Framework installer (the `qtifw` option, issue #456)**: the
  license file is copied into the `meta` directory of every package of the
  generated installer template (the original file name is kept) and the
  `<Licenses>` element (the `<License name="...">` title = the base name of
  the file, the `file="..."` value = the file name relative to the meta
  directory) is injected into the generated `package.xml` of the package
  before the `</Package>` end tag. The installer then shows the license
  agreement before the installation starts (all the host platforms).
  - **Important (the binarycreator validation fix)**: the Qt Installer
    Framework versions 4.x read the license agreement from the `package.xml`
    file of the packages (the `meta` directory of the package), the
    `config.xml` file of the installer does **not** support the license
    agreement elements: the `binarycreator` tool parses the `config.xml` in
    the strict mode (`Settings::fromFileAndPrefix` with the
    `StrictParseMode`) and rejects the unknown elements (the first
    implementation with the `<LicenseAgreement>` element in the
    `config.xml` was rejected by the `binarycreator` tool). The format of
    the injected `<Licenses>` block matches the official example of the Qt
    Installer Framework 4.11 documentation (`doc/examples/package.xml`).
- **DEB packages (the `deb` option, issue #456)**: the license file is copied
  into the standard location of the license text of the Debian policy:
  `usr/share/doc/<package>/copyright` of every generated deb package.
- The option works with the command line (`cqtdeployer ... -license
  ./LICENSE.txt qtifw deb`) and with the JSON configuration file (the
  `confFile` mechanism resolves the relative paths relatively the config file
  directory, no extra code is needed).
- Error handling: when the license file is not found the deploy fails with
  the critical message (the same strict behavior as the qifBanner/qifLogo
  copy steps). When the custom package template already contains the
  `<Licenses>` element the license file is copied only.
- The `deployRawTemplate` (the `getDefaultTemplate` mode) gets the license
  too, so the extracted default templates contain the license configuration.

## Checklist / Sub-tasks

- [x] Read the issue #456 and the context of the issue #455
- [x] Update the repository (git pull) and create the task_456 branch from main
- [x] Publish the initial checklist of the issue
- [x] Add the `-license` option to the options table (deploycore.cpp)
- [x] Add the license injection into the QIF installer (qif.cpp/h): the
  license file into the meta directory of the package + the `Licenses`
  element into the package.xml (the config.xml injection does not work: the
  binarycreator of the Qt Installer Framework 4.x rejects the unknown
  elements of the config.xml, the license agreement is read from the
  package.xml of the packages)
- [x] Add the license file into the DEB packages (deb.cpp/h)
- [x] Add the unit tests (the Qifw test class + the new Deb test class)
- [x] Update the documentation (md/Options.md, md/QIF.md, md/DEB.md, md/Changelog.md)
- [x] Build the project and run the test suite (QifwTest: 9/9 passed,
  DebTest: 4/4 passed; the smoke run of the created installer shows the
  `License "LICENSE" accepted by user` line of the installation log)
- [x] Run the git diff check
- [ ] Push the task_456 branch and create the PR
- [ ] Set the waiting-for-review label and post the result comment

## Blockers / Questions

- None.
