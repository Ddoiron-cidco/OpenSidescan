# version.md

## Purpose
Track all repository changes in one place.
This file must be updated whenever a change is planned or completed.

## Change Log

| Date (UTC) | Status | Area | Change |
|---|---|---|---|
| 2026-02-19 | Done | Docs | Added repository governance and baseline analysis file: `AGENTS.md`. |
| 2026-02-19 | Done | Docs | Added repository version audit file: `versions.md`. |
| 2026-02-19 | Done | Docs | Added change tracking file: `version.md`. |
| 2026-02-19 | Done | CI | Migrated CI from Jenkins to GitHub Actions in `.github/workflows/ci.yml` with Linux and Windows jobs mapped from `Jenkinsfile`. |
| 2026-02-19 | Done | CI | Added conditional Windows signing/packaging flow in GitHub Actions (`sign_and_package_windows`) for executable and installer signing. |
| 2026-02-19 | Done | Security | Added optional certificate-secret signing path (`WINDOWS_SIGN_CERT_B64`, `WINDOWS_SIGN_CERT_PASSWORD`) with fallback to machine certificate store signing. |
| 2026-02-19 | Done | CI | Removed all `self-hosted` runners and moved Windows jobs to `windows-latest`. |
| 2026-02-19 | Done | Security | Disabled in-workflow signing and added CI note to use either a cloud/HSM signing service or a dedicated signing host. |
| 2026-02-19 | Done | CI | Added Windows dependency bootstrap in CI (`Qt 5.15.2` via `install-qt-action`, `OpenCV`/`Eigen` via `vcpkg`) and legacy Eigen path provisioning (`C:\\LIBS\\eigen-3.4.0`). |
| 2026-02-19 | Planned | Build | Define a non-empty canonical project version in `CMakeLists.txt` and propagate it to packaging metadata. |
| 2026-02-19 | Planned | CI | Replace hardcoded self-hosted runner labels with organization-standard labels after runner registration is finalized. |
| 2026-02-19 | Planned | Testing | Remove internal dataset path assumptions (`/opt`, internal host) from Linux unit-test CMake configuration. |

## Update Rule
When a change starts, add a `Planned` entry.
When it is completed, update that entry to `Done` (or add a new `Done` line with details).
