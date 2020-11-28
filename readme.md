DMG
=

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0) [![Build Status](https://github.com/majestic53/dmg/workflows/Build/badge.svg)](https://github.com/majestic53/dmg/actions)

About
=

A simple GameBoy emulator, written in C.

Table of Contents
=

1. [Building](https://github.com/majestic53/dmg#building) -- How to build the project
2. [Usage](https://github.com/majestic53/dmg#usage) -- How to use the project
3. [License](https://github.com/majestic53/dmg#license) -- Project license

Building
=

__NOTE__: This project was tested under Linux with the GCC/Clang C compilers. Some modifications to the makefiles will be required to build with a different OS/compiler.

### Preparing to build

The following libraries are required to build this project:

```
libsdl2
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

Usage
=

### Interface

This project is implemented in C and exposes a simple API, described in ```include/dmg.h``` and ```lib/libdmg.a```:

For an example of how to use this interface, see the [launcher tool](https://github.com/majestic53/dmg/tree/master/tool/launcher) under ```tool/launcher```

#### Runtime/Helper Routines

|Name       |Description                       |Signature                                   |
|:----------|:---------------------------------|:-------------------------------------------|
|dmg        |Start the emulator                |```int dmg(const dmg_t *)```                |
|dmg_error  |Retrieve error information        |```const char *dmg_error(void)```           |
|dmg_serial |Notify emulator of serial transfer|```unsigned dmg_serial(unsigned)```         |
|dmg_version|Retrieve version information      |```const dmg_version_t *dmg_version(void)```|

#### Cartridge Support

|Cartridge type|Value|
|:-------------|:----|
|ROM-Only      |0    |
|MBC1          |1-3  |
|MBC3          |15-19|
|MBC5          |25-30|

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
