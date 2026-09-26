# R4NS0M Simulator — Wii Port

Wii homebrew port prototype for the R4NS0M Simulator project.

## Build on GitHub Actions

1. Create an empty GitHub repository.
2. Upload the contents of this folder to the repository root.
3. Open **Actions** → **Build Wii** → **Run workflow**.
4. Download the `ransom-wii-dol` artifact from the completed run.

The workflow uses the devkitPro/devkitPPC container. No Wii compiler is required on your phone.

## Status

This is a port prototype. The gameplay architecture is implemented, but exact APK behavior,
graphics, audio, and special-item behavior still require further reverse engineering and testing.
