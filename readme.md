DMG
=

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0) [![Lines](https://tokei.rs/b1/github/majestic53/dmg?category=code)](https://tokei.rs/b1/github/majestic53/dmg?category=code) [![Files](https://tokei.rs/b1/github/majestic53/dmg?category=files)](https://tokei.rs/b1/github/majestic53/dmg?category=files) [![Build Status](https://travis-ci.com/majestic53/dmg.svg?branch=master)](https://travis-ci.com/majestic53/dmg)

About
=

A DMG GameBoy emulator with support for the following mappers:

|Mapper  |Value|
|:-------|:----|
|ROM-Only|0    |
|MBC1    |1-3  |

Table of Contents
=

1. [Building](https://github.com/majestic53/dmg#building) -- How to build the project
2. [Usage](https://github.com/majestic53/dmg#usage) -- How to use the project
3. [License](https://github.com/majestic53/dmg#license) -- Project license

Building
=

__NOTE__: Tested under Linux with the GCC/Clang C compilers. Modification to the makefiles might be required to build with a different OS/compiler.

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

Where COMPILER corrisponds to either ```clang```, ```gcc``` or the compiler of your choice.

Where BUILD corrisponds to either ```release``` or ```debug```. If no BUILD argument is specified, a release build will be created.

Usage
=

### Interface

The project is implemented in C and exposes a simple API, described in ```include/dmg.h``` and ```lib/libdmg.a```:

For an example, see the launcher tool under ```tool/```

#### Runtime Routines

|Name|Description |Signature                   |
|:---|:-----------|:---------------------------|
|dmg |Run emulator|```int dmg(const dmg_t *)```|

#### Helper Routines

|Name       |Description              |Signature                                   |
|:----------|:------------------------|:-------------------------------------------|
|dmg_error  |Retrieve emulator error  |```const char *dmg_error(void)```           |
|dmg_version|Retrieve emulator version|```const dmg_version_t *dmg_version(void)```|

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
