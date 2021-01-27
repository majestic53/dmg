DMG Launcher
=

![Demo](https://github.com/majestic53/dmg/blob/master/docs/demo.png)

A launcher for running GameBoy rom/bootrom binaries, with support for various palettes and more.

Interface
=

Launch from the project root directory:

```
$ ./bin/dmg [args]
```

#### Arguments

The following arguments are available:

```
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
-n	Specify client serial port
-m	Specify server serial port
-v	Display version information
```

#### Examples

The following example shows how to launch dmg with a rom file:

```
$ ./bin/dmg -r rom.gb
```

The following example shows how to launch dmg with a rom/bootrom file:

```
$ ./bin/dmg -r rom.gb -b bootrom.gb
```

The following example shows how to launch dmg with the serial capture enabled:

```
$ ./bin/dmg -r rom.gb -c
```

The following example shows how to launch dmg with the debug prompt enabled:

```
$ ./bin/dmg -r rom.gb -d
```

The following example shows how to launch dmg and import a save file:

```
$ ./bin/dmg -r rom.gb -i save.dmg
```

The following example shows how to launch dmg and export a save file:

```
$ ./bin/dmg -r rom.gb -o save.dmg
```

The following example shows how to launch dmg with an alternative color palette:

```
$ ./bin/dmg -r rom.gb -p 1
```

The following example shows how to launch dmg with the display scaled by 2x:

```
$ ./bin/dmg -r rom.gb -s 2
```

The following example shows how to launch dmg with as a serial server, using port ```1234```
```
$ ./bin/dmg -r rom.gb -m 1234
```

The following example shows how to launch dmg with as a serial client, using port ```1234```
```
$ ./bin/dmg -r rom.gb -n 1234
```

#### Debug Prompt

The following commands are available:

```
q	Exit debug prompt
z	Disassemble instruction(s) at address
h	Display help information
p	Display processor information
r	Read byte(s) from address/register
c	Run emulator until breakpoint
s	Step emulator through instructions
v	Display version information
w	Write byte(s) to address/register
```

#### Examples

The following example shows how to run the emulator with a breakpoint at address ```0x0100``` and ```0x0200```:

```
(0) c 100 200
```

The following example shows how to step the emulator through 10 instructions, with a breakpoint at address ```0x0100```:

```
(0) s 10 100
```
The following example shows how to read from address ```0xa000```:

```
(0) r a000
```

The following example shows how to read from address ```0xa000-0xa01f```:

```
(0) r a000 20
```

The following example shows how to read from register ```$pc```:

```
(0) r $pc
```

The following example shows how to write ```0xcc``` to address ```0xa000```:

```
(0) w a000 cc
```

The following example shows how to write ```0xcc``` to address ```0xa000-0xa01f```:

```
(0) w a000 cc 20
```

The following example shows how to write ```0xaabb``` to register ```$pc```:

```
(0) w $pc aabb
```

The following example shows how to disassemble ```3``` instructions at address ```0x0100```:

```
(0) z 100 3
```

The following example shows how to disassemble the instruction at ```$pc```:

```
(0) z $pc
```

#### Keys

The following keys-bindings are available:

|Key       |Binding    |
|:---------|:----------|
|A         |X          |
|B         |Z          |
|Select    |C          |
|Start     |Space      |
|Right     |Right-Arrow|
|Left      |Left-Arrow |
|Up        |Up-Arrow   |
|Down      |Down-Arrow |
|Fullscreen|F11        |

#### Palettes

The following color palettes are available:

|Id|Palette    |                                                                                          |                                                                                                    |
|:-|:----------|:-----------------------------------------------------------------------------------------|:---------------------------------------------------------------------------------------------------|
|0 |Grey       |![Grey](https://github.com/majestic53/dmg/blob/master/docs/palette_grey.png)              |![Grey Demo](https://github.com/majestic53/dmg/blob/master/docs/palette_grey_demo.png)              |
|1 |Green      |![Green](https://github.com/majestic53/dmg/blob/master/docs/palette_green.png)            |![Green Demo](https://github.com/majestic53/dmg/blob/master/docs/palette_green_demo.png)            |
|2 |Olive-Green|![Olive-Green](https://github.com/majestic53/dmg/blob/master/docs/palette_green_olive.png)|![Olive-Green Demo](https://github.com/majestic53/dmg/blob/master/docs/palette_green_olive_demo.png)|
|3 |LCD-Green  |![LCD-Green](https://github.com/majestic53/dmg/blob/master/docs/palette_green_lcd.png)    |![LCD-Green Demo](https://github.com/majestic53/dmg/blob/master/docs/palette_green_lcd_demo.png)    |
|4 |Teal       |![Teal](https://github.com/majestic53/dmg/blob/master/docs/palette_teal.png)              |![Teal Demo](https://github.com/majestic53/dmg/blob/master/docs/palette_teal_demo.png)              |
|5 |Blue       |![Blue](https://github.com/majestic53/dmg/blob/master/docs/palette_blue.png)              |![Blue Demo](https://github.com/majestic53/dmg/blob/master/docs/palette_blue_demo.png)              |
|6 |Purple     |![Purple](https://github.com/majestic53/dmg/blob/master/docs/palette_purple.png)          |![Purple Demo](https://github.com/majestic53/dmg/blob/master/docs/palette_purple_demo.png)          |
|7 |Red        |![Red](https://github.com/majestic53/dmg/blob/master/docs/palette_red.png)                |![Red Demo](https://github.com/majestic53/dmg/blob/master/docs/palette_red_demo.png)                |

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
