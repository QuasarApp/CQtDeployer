# appimagetool binaries

Prebuilt `appimagetool` executables (AppImage type 1 tool of the AppImage
project) for the Linux platforms CQtDeployer is released for.

The `appimage` option (issue #898) creates AppImage packages with the
`appimagetool`. All the tools required to create an AppImage are bundled
within the CQtDeployer package (much like the Qt Installer Framework
binaries of issue #867), so the AppImage packages can be created on hosts
where the AppImage tools are not installed by default. When the tools are
already present on the system (for example as the `appimage` package of
the Debian distributions) the system tools are used, because the system
PATH is searched before the bundled tools (see the `toolKitEnv()` method
of the AppImage distribution, `src/Deploy/src/Distributions/appimage.cpp`).

The AppImageKit project publishes the `appimagetool` as a self-contained
AppImage (the tool bundles its own interpreter and the squashfs tools, so
a single file per platform is the complete tool set):
https://github.com/AppImage/AppImageKit (release `continuous`). The
`linuxdeploy` alternative is not used, because all of its published
releases dropped the `--appimage` feature (it is "not yet implemented" in
the type-2 based builds), so it cannot create the AppImage packages.

The CMake build (`src/CQtDeployer/CMakeLists.txt`, target `copyAppImage`)
copies the platform-matching `appimagetool.AppImage` from this folder and
the wrapper script (`binaries/appimagetool`) into `Deploy/AppImage`, which
is bundled into the installer (package `QIF`) and the flat zip packages,
and into the build tree next to the built executable (development builds).
The wrapper script starts the AppImage directly and falls back to the
`--appimage-extract-and-run` mode of the AppImage runtime when the host
has no FUSE support, so the tool works on the FUSE-less hosts as well.

The `Deploy/AppImage` folder is tracked in the repository (it is NOT
git-ignored), so changes of the staged binaries are visible in the git
diff. It currently mirrors the `linux-x86_64` binary; on every build the
`copyAppImage` target removes its stale staged content and then stages the
binary of the platform being built, so the packaged artifacts always
contain the matching platform binary.

## Files

| Platform (folder) | Files | Notes |
|---|---|---|
| `linux-x86_64/` | `appimagetool.AppImage` | Linux x86_64 |
| `linux-aarch64/` | `appimagetool.AppImage` | Linux aarch64 (arm64) |
| `appimagetool` | (file) | the wrapper script, see above |

## Provenance (verified 2026-10-30)

Source: AppImageKit project, release `continuous`
(published 2023-03-08):

| Folder | Asset | Size (bytes) | SHA256 |
|---|---|---|---|
| `linux-x86_64/` | `appimagetool-x86_64.AppImage` | 8811712 | `b90f4a8b18967545fda78a445b27680a1642f1ef9488ced28b65398f2be7add2` |
| `linux-aarch64/` | `appimagetool-aarch64.AppImage` | 6115712 | `a48972e5ae91c944c5a7c80214e7e0a42dd6aa3ae979d8756203512a74ff574d` |

Asset URLs:

    https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
    https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-aarch64.AppImage

The stored files are the downloaded assets renamed to `appimagetool.AppImage`.
The x86_64 binary was verified by running it against a test AppDir (it
creates a valid AppImage, the AppImage extracts back to the AppDir content
with the `--appimage-extract` option), the aarch64 binary was verified as
a valid `ARM aarch64` ELF executable.

## Updating the binaries

To refresh the binaries to a newer AppImageKit build:

1. Download the two platform assets of the `continuous` release of
   https://github.com/AppImage/AppImageKit/releases.
2. Verify the SHA256 sums and store the files as
   `linux-x86_64/appimagetool.AppImage` and
   `linux-aarch64/appimagetool.AppImage`.
3. Update the table above.
