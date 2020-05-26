# Luma3DS
*Noob-proof (N)3DS "Custom Firmware"*

## What it is

**Luma3DS** is a program to patch the system software of (New) Nintendo 3DS handheld consoles "on the fly", adding features (such as per-game language settings and debugging capabilities for developers) and removing restrictions enforced by Nintendo (such as the region lock).
It also allows you to run unauthorized ("homebrew") content by removing signature checks.
To use it, you will need a console capable of running homebrew software on the ARM9 processor. We recommend [Plailect's guide](https://3ds.hacks.guide/) for details on how to get your system ready.

Since Luma3DS v8.0, Luma3DS has its own in-game menu, triggerable by `L+Down+Select` (see the [release notes](https://github.com/AuroraWright/Luma3DS/releases/tag/v8.0)).

We're implementing our own changes to it, with some extra features.

---

## Extra features

- Plugin loader (courtesy of Nanquitas, port to 10.1.3 from mariohackandglitch, small fixes from HIDE810)
- Blue light filter changes (courtesy of vertexmachina)

I'm hoping to add streaming capabilities from Byebyesky's fork but I'm gonna hold off on that until later.

---

### Compiling
* Prerequisites
    1. git | Download [here](https://git-scm.com)
    2. [makerom](https://github.com/jakcron/Project_CTR) | Compiled version for latest release is available [here](https://github.com/jbmagination/Luma3DS/releases/tag/compile)
    3. [firmtool](https://github.com/TuxSH/firmtool) | Compiled version for latest release is available [here](https://github.com/jbmagination/Luma3DS/releases/tag/compile)
    4. Up-to-date devkitARM+libctru | Requires [devkitPro pacman](https://devkitpro.org/wiki/devkitPro_pacman) with the `3ds-dev` group of packages installed
  
Make sure everything is added to your PATH.
    
1. Clone the repository with `git clone https://github.com/jbmagination/Luma3DS.git`
2. Run `make`
    
The produced file is called `boot.firm` and is meant to be copied to the root of your SD card, for usage with boot9strap.

---

## Setup / Usage / Features

See https://github.com/AuroraWright/Luma3DS/wiki

---

## Credits

See https://github.com/AuroraWright/Luma3DS/wiki/Credits

---

## Licensing

This software is licensed under the terms of the GPLv3.
You can find a copy of the license in the LICENSE.txt file.

Files in the GDB stub are instead double-licensed as MIT or "GPLv2 or any later version", in which case it is specified in the file header.
