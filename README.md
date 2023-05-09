<!--
SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
SPDX-License-Identifier: MIT
-->

## DMG Gameboy emulator

[![License: MIT](https://shields.io/badge/license-MIT-blue.svg?style=flat)](LICENSES/MIT.txt) [![Build Status](https://github.com/majestic53/dmg/workflows/Build/badge.svg)](https://github.com/majestic53/dmg/actions/workflows/build.yml)

The goal of `DMG` is to emulate the original Gameboy hardware with enough accuracy to run most common games, while being performant and natively compilable across multiple platforms.

__NOTE__: `DMG` is a still a work-in-progress. Some features may be incomplete or may contain bugs. Testing was done using the GCC compiler, under Linux. Modification to the makefiles might be required to build with a different operating system and/or compiler.

## Work-in-progress

* Support for MBC3 RTC
* Support for audio

## Latest release

The latest release can be found [here](https://github.com/majestic53/dmg/releases).

## Build from source

`DMG` is dependant on the `SDL2` library. If you are on a Debian-based systems, install this library using apt:

```bash
sudo apt-get install libsdl2-dev
```

If you are on an Arch-based systems, install this library using pacman:

```bash
sudo pacman -S sdl2
```

Once all dependencies are met, clone the repo and build:

```bash
git clone https://github.com/majestic53/dmg
cd dmg
make
```

## Usage

The following interface is supported:

```
Usage: dmg [options] file

Options:
   -h, --help        Show help information
   -l, --link        Enable serial link
   -v, --version     Show version information
```

### Examples

```bash
# To launch with a cartridge, run the following command
./dmg cartridge.gb
```

### Keybindings

The following keybindings are supported:

|Button |Keyboard|
|:------|:-------|
|A      |L       |
|B      |K       |
|Select |C       |
|Start  |Space   |
|Right  |D       |
|Left   |A       |
|Up     |W       |
|Down   |S       |

### Mappers

The following mappers are supported:

|Id   |Name                                       |Description       |
|:----|:------------------------------------------|:-----------------|
|0,8-9|[MBC0](https://gbdev.io/pandocs/nombc.html)|32KB ROM/8KB RAM  |
|1-3  |[MBC1](https://gbdev.io/pandocs/MBC1.html) |2MB ROM/32KB RAM  |
|5-6  |[MBC2](https://gbdev.io/pandocs/MBC2.html) |256KB ROM/512B RAM|
|15-19|[MBC3](https://gbdev.io/pandocs/MBC3.html) |2MB ROM/32KB RAM  |
|25-30|[MBC5](https://gbdev.io/pandocs/MBC5.html) |8MB ROM/128KB RAM |

### License

Copyright (C) 2023 David Jolly. Released under the [MIT License](LICENSES/MIT.txt).
