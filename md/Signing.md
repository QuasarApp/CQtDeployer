# Code signing of the QtIFW installer (macOS)

The `qtifw` (Qt Installer Framework) distribution creates the installer on
the macOS hosts as an app bundle (`<name>.app`). The Mach-O rewrite of the
deployed binaries (the install names and the rpaths, issue #166) invalidates
the code signature, so the generated installer must be signed before the
release. The code signing is configured with the `qifSign` option
(issue #865).

## The `qifSign` option

The `-qifSign` option (the `qtifw` options group) sets the code signing
identity of the Qt Installer Framework `binarycreator` (the `-s` option).
The option is available on the macOS hosts only: on the other platforms it
prints a warning and is ignored.

``` bash
cqtdeployer -bin myApp qtifw -qifSign 'Developer ID Application: MyCompany (ABC1234)'
```

The option requires the identity value: the command line without the value
exits with the `Missing argument for -qifSign` error (the exit code 4, the
same rule of the options parser for all the string options).

## The signing identity values

| Identity | Signature |
|----------|-----------|
| `-` | The ad-hoc signature (no signing certificate required) |
| `Developer ID Application: <Name> (<TeamID>)` | The Developer ID signature - the distribution outside of the Mac App Store (the notarization is recommended) |
| `3rd Party Mac Developer Application: <Name> (<TeamID>)` | The Mac App Store distribution signature |
| `Apple Development: <Name> (<TeamID>)` | The development signature (the local builds and the testing) |

Any code signing identity of the keychain can be used. List the available
identities of the current host:

``` bash
security find-identity -v -p codesigning
```

The identity without the leading `-` character is passed to the `-s` option
of the `binarycreator`: the tool signs the generated app bundle with the
`codesign --force --deep --sign <identity> <bundle>` command. The ad-hoc `-`
identity is created by the codesign step of the CQtDeployer after the bundle
is created: the command line parser of the `binarycreator` rejects the
identity values that start with the `-` character (the upstream Qt Installer
Framework limitation, documented in the [QIF](QIF.md) note).

## The verification of the signature

``` bash
codesign -v --verbose=2 <path/to/the/installer>.app
```

See also: [QIF](QIF.md), [Options](Options.md), [CQtDeployer for macOS](macOS.md).
