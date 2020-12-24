DMG 0.2.8-alpha
Copyright (C) 2020 David Jolly
==============================

A simple GameBoy emulator, written in C.

Source: https://github.com/majestic53/dmg.git


Using the Library
=================

This project is implemented in C and exposes a simple API, described in include/dmg.h and lib/libdmg.a:

|Name       |Description                          |Signature                                               |
|-----------|-------------------------------------|--------------------------------------------------------|
|dmg_load   |Load emulator instance               |int dmg_load(const dmg_t *)                             |
|dmg_unload |Unload emulator instance             |void dmg_unload(void)                                   |
|dmg_action |Send emulator instance action request|int dmg_action(const dmg_action_t *, dmg_action_t *)    |
|dmg_run    |Run emulator instance                |int dmg_run(const unsigned short *, unsigned)           |
|dmg_step   |Step emulator instance               |int dmg_step(unsigned, const unsigned short *, unsigned)|
|dmg_error  |Retrieve emulator instance error     |const char *dmg_error(void)                             |
|dmg_version|Retrieve emulator instance version   |const dmg_version_t *dmg_version(void)                  |


Using the Tools
===============

Launcher
--------

dmg [args]

-b	Specify bootrom binary
-c	Enable serial capture
-d	Enable debug prompt
-h	Display help information
-i	Specify input save file path
-o	Specify output save file path
-p	Specify color palette
-r	Specify rom binary
-s	Specify display scale
-v	Display version information

For details see: https://github.com/majestic53/dmg/blob/master/tool/launcher/readme.md

Utilities
---------

dmg-asm [args]

-h	Display help information
-o	Specify output file
-s	Specify source file
-v	Display version information

For details see: https://github.com/majestic53/dmg/blob/master/tool/utility/asm/readme.md

dmg-dasm [args]

-h	Display help information
-o	Specify output file
-r	Specify rom file
-v	Display version information

For details see: https://github.com/majestic53/dmg/blob/master/tool/utility/dasm/readme.md

dmg-rom-fix [args]

-h	Display help information
-r	Specify rom file
-v	Display version information

For details see: https://github.com/majestic53/dmg/blob/master/tool/utility/rom_fix/readme.md

dmg-rom-info [args]

-h	Display help information
-r	Specify rom file
-v	Display version information

For details see: https://github.com/majestic53/dmg/blob/master/tool/utility/rom_info/readme.md

dmg-save-info [args]

-h	Display help information
-s	Specify save file
-v	Display version information

For details see: https://github.com/majestic53/dmg/blob/master/tool/utility/save_info/readme.md


Trademark
=========

Nintendo name/logo are trademarks of Nintendo Co., Ltd.


License
=======

DMG is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DMG is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
