DMG Disassembler
=

About
=

A tool for disassembling GameBoy ROM files.

Usage
=

### Interface

Launch from the project root directory:

```
$ ./bin/dmg-dasm [args]
```

#### Arguments

The following arguments are available:

```
dmg-dasm [args]

-h	Display help information
-o	Specify an output file
-r	Specify rom file
-v	Display version information
```

#### Examples

The following example shows how to launch dmg-dasm with a ROM file:

```
$ ./bin/dmg-dasm -r rom.gb
```

The following example shows how to launch dmg-dasm with a ROM file and output to a specified file:

```
$ ./bin/dmg-dasm -r rom.gb -o path/to/output/file.s
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
