<!--
SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
SPDX-License-Identifier: MIT
-->

## DMG (DMG Gameboy emulator)

[![License: MIT](https://shields.io/badge/license-MIT-blue.svg?style=flat)](LICENSES/MIT.txt) [![Build Status](https://github.com/majestic53/dmg/workflows/Build/badge.svg)](https://github.com/majestic53/dmg/actions/workflows/build.yml) [![Test Status](https://github.com/majestic53/dmg/workflows/Test/badge.svg)](https://github.com/majestic53/dmg/actions/workflows/test.yml)

### A lightweight Gameboy emulator, written in C.

__NOTE__: `DMG` is a still a work-in-progress. Some features may be incomplete or may contain bugs.

## Features

* Support for input and serial peripherals (see [Keybindings](https://github.com/majestic53/dmg#keybindings))
* Support for multiple mapper types (see [Mappers](https://github.com/majestic53/dmg#mappers))
* Support for multiple palettes (see [Palettes](https://github.com/majestic53/dmg#palettes))

## Work-in-progress

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
   -p, --palette     Set palette type
   -v, --version     Show version information
```

### Examples

```bash
# To launch with a cartridge, run the following command
./dmg cartridge.gb
# To launch with the serial link enabled, run the following command
./dmg cartridge.gb --link
# To launch with the Gameboy palette, run the following command
./dmg cartridge.gb --palette gb
# To launch with the Gameboy Pocket palette, run the following command
./dmg cartridge.gb --palette gbp
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

## Palettes

The following palettes are supported:

|Id |Name                  |Colors              |
|:--|:---------------------|:-------------------|
|gb |Gameboy palette       |![gb](docs/gb.png)  |
|gbp|Gameboy Pocket palette|![gbp](docs/gbp.png)|

### License

Copyright (C) 2023 David Jolly. Released under the [MIT License](LICENSES/MIT.txt).
