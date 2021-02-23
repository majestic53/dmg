DMG
=

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0) [![Build Status](https://github.com/majestic53/dmg/workflows/Build/badge.svg)](https://github.com/majestic53/dmg/actions)

A GameBoy emulator and assembler/disassembler, written in C.

Features supported
=

#### Emulator

* Support for all processor instructions
* Support for hardware peripherals (joypad/serial/timer)
	* No audio support at the moment
* Support for save states
* Support for multiple color-palettes
* Support for common cartridge types:

|Cartridge type|Value |
|:-------------|:-----|
|ROM-Only      |0, 8-9|
|MBC1          |1-3   |
|MBC2          |5-6   |
|MBC3          |15-19 |
|MBC5          |25-30 |

#### Assembler/Disassembler

* Support for all processor instructions
* Basic preprocessor directives

#### Utilities

* ROM header parsing/checksum validation/correction
* Save state parsing/validation

Latest Release
=

The latest release can be found here: [v0.2.2-alpha](https://github.com/majestic53/dmg/releases/tag/v0.2.2-alpha)

Building from Source
=

__NOTE__: This project was tested under Linux with the GCC/Clang C compilers. Some modifications to the makefiles will be required to build with a different OS/compiler.

### Preparing to build

The following libraries are required to build this project:

```
libsdl2
libreadline
libpthread
```

### Building

Run the following commands from the projects root directory:

```
$ export CC=<COMPILER>
$ make <BUILD>
```

|Field   |Supported values          |Note                                                        |
|:-------|:-------------------------|:-----------------------------------------------------------|
|COMPILER|```gcc```, ```clang```    |                                                            |
|BUILD   |```release```, ```debug```|If no argument is specified, a release build will be created|

#### Examples

The following example shows how to compile the DMG release build using GCC:

```
$ export CC=gcc
$ make
```

Using the Library
=

This project is implemented in C and exposes a simple API, described in ```include/dmg.h``` and ```lib/libdmg.a```:

For an example of how to use this interface, see the [launcher tool](https://github.com/majestic53/dmg/tree/master/tool/launcher) under ```tool/launcher```

#### Routines

|Name       |Description                          |Signature                                                 |
|:----------|:------------------------------------|:---------------------------------------------------------|
|dmg_load   |Load emulator instance               |```int dmg_load(const dmg_t *)```                         |
|dmg_unload |Unload emulator instance             |```void dmg_unload(void)```                               |
|dmg_action |Send emulator instance action request|```int dmg_action(const dmg_action_t *, dmg_action_t *)```|
|dmg_run    |Run emulator instance                |```int dmg_run(const uint16_t *, uint32_t)```             |
|dmg_step   |Step emulator instance               |```int dmg_step(uint32_t, const uint16_t *, uint32_t)```  |
|dmg_error  |Retrieve emulator instance error     |```const char *dmg_error(void)```                         |
|dmg_version|Retrieve emulator instance version   |```const dmg_version_t *dmg_version(void)```              |

Trademark
=

Nintendo name/logo are trademarks of Nintendo Co., Ltd.

License
=

DMG is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DMG is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
