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
-r	Specify rom binary
-s	Specify display scale
-v	Display version information
```

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
