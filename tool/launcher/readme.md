DMG Launcher
=

![Demo](https://github.com/majestic53/dmg/blob/master/docs/demo.png)

About
=

A launcher for running GameBoy ROMs/BOOTROM binaries, with support for various palettes and more.

Usage
=

### Interface

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
-v	Display version information
```

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

#### Examples

The following example shows how to launch DMG with a ROM file:

```
$ ./bin/dmg -r rom.gb
```

The following example shows how to launch DMG with a ROM/BOOTROM file:

```
$ ./bin/dmg -r rom.gb -b bootrom.gb
```

The following example shows how to launch DMG with the serial capture enabled:

```
$ ./bin/dmg -r rom.gb -c
```

The following example shows how to launch DMG with the debug prompt enabled:

```
$ ./bin/dmg -r rom.gb -d
```

The following example shows how to launch DMG and import a save file:

```
$ ./bin/dmg -r rom.gb -i save.dmg
```

The following example shows how to launch DMG and export a save file:

```
$ ./bin/dmg -r rom.gb -o save.dmg
```

The following example shows how to launch DMG with an alternative color palette:

```
$ ./bin/dmg -r rom.gb -p 1
```

The following example shows how to launch DMG with the display scaled by 2x:

```
$ ./bin/dmg -r rom.gb -s 2
```

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
