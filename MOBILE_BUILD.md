# Phone-only build

You can do the entire build from a phone.

### GitHub

Upload these files/folders to the root of your repository:

- `.github/`
- `source/`
- `include/`
- `assets/`
- `Makefile`
- `README.md`
- `LICENSE` (if applicable)

Then run:

**GitHub → Actions → Build Wii → Run workflow**

The workflow uploads the resulting `.dol` as an artifact.

### Important

The repository does not contain Nintendo proprietary SDKs. It uses the open devkitPro/libogc
Wii homebrew toolchain.
