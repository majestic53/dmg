DMG Utilities
=

Usage
=

### Interface

Launch from the project root directory:

```
$ ./bin/dmg-rom-info [args]
$ ./bin/dmg-save-info [args]
```

#### Arguments

The following arguments are available:

```
dmg-rom-info [args]

-h	Display help information
-r	Specify rom file
-v	Display version information
```

```
dmg-save-info [args]

-h	Display help information
-s	Specify save file
-v	Display version information
```

#### Examples

The following example shows how to launch dmg-rom-info with a rom file:

```
$ ./bin/dmg-rom-info -r rom.gb
```

The following example shows how to launch dmg-save-info with a save file:

```
$ ./bin/dmg-save-info -r save.dmg
```
