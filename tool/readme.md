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
-h	Display help information
-r	Specify rom binary
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
