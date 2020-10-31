DMG Launcher
=

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
-h	Display help information
-p	Specify color palette
-r	Specify rom binary
-s	Specify display scale
-v	Display version information
```

#### Palettes

The following color palettes are available:

|Id|Colors                                                                                     |
|:-|:------------------------------------------------------------------------------------------|
|0 |![Grey](https://github.com/majestic53/dmg/blob/master/docs/palette_grey.png)              |
|1 |![Light-green](https://github.com/majestic53/dmg/blob/master/docs/palette_green_light.png)|
|2 |![Dark-green](https://github.com/majestic53/dmg/blob/master/docs/palette_green_dark.png)  |
|3 |![LCD-green](https://github.com/majestic53/dmg/blob/master/docs/palette_green_lcd.png)    |
|4 |![Teal](https://github.com/majestic53/dmg/blob/master/docs/palette_teal.png)              |

#### Examples

The following example shows how to launch dmg with a rom file:

```
$ ./bin/dmg -r rom.gb
```

The following example shows how to launch dmg with a rom and bootrom file:

```
$ ./bin/dmg -r rom.gb -b bootrom.gb
```

The following example shows how to launch dmg with the display scaled by 2x:

```
$ ./bin/dmg -r rom.gb -s 2
```

The following example shows how to launch dmg with the serial capture enabled:

```
$ ./bin/dmg -r rom.gb -c
```

The following example shows how to launch dmg with an alternative color palette:

```
$ ./bin/dmg -r rom.gb -p 1
```
