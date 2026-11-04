# Code signing of the QtIFW installer (macOS)

The `qtifw` (Qt Installer Framework) distribution creates the installer on
the macOS hosts as an app bundle (`<name>.app`). The Mach-O rewrite of the
deployed binaries (the install names and the rpaths, issue #166) invalidates
the code signature, so the generated installer must be signed before the
release. The code signing is configured with the `qifSign` option
(issue #865).

## The `qifSign` option

The `qifSign` option (the `qtifw` options group) sets the code signing
identity of the Qt Installer Framework `binarycreator` (the `-s` option).
The option is available on the macOS hosts only: on the other platforms it
prints a warning and is ignored.

The option is given in two forms:

``` bash
cqtdeployer -bin myApp qtifw qifSign
```

The single `qifSign` option without the identity value creates the ad-hoc
signature (no signing certificate required - the only signing form of the
macOS CI hosts without the Developer ID certificate).

``` bash
cqtdeployer -bin myApp qtifw -qifSign 'Developer ID Application: MyCompany (ABC1234)'
```

The `-qifSign` option with the identity value signs the bundle with the
specified identity. The command line parser of the cqtdeployer rejects
the option values with the leading `-` character (the single `-` token),
so the ad-hoc identity is not passed as the option value: use the single
`qifSign` form for the ad-hoc signature.

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
identity (the single `qifSign` form) is created by the codesign step of the
CQtDeployer after the bundle is created: the command line parser of the
`binarycreator` rejects the identity values that start with the `-`
character (the upstream Qt Installer Framework limitation, documented in
the [QIF](QIF.md) note), and the command line parser of the cqtdeployer
rejects the option values with the leading `-` character too (the single
`-` token).

## The verification of the signature

``` bash
codesign -v --verbose=2 <path/to/the/installer>.app
```

See also: [QIF](QIF.md), [Options](Options.md), [CQtDeployer for macOS](macOS.md).
