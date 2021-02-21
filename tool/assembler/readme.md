DMG Assembler
=

A tool for assembling GameBoy rom files.

Interface
=

Launch from the project root directory:

```
$ dmg-asm [args]
```

#### Arguments

The following arguments are available:

```
dmg-asm [args]

-h	Display help information
-o	Specify output file
-s	Specify source file
-v	Display version information
```

#### Examples

The following example shows how to launch dmg-asm with a source file:

```
$ dmg-asm -s source.s
```

The following example shows how to launch dmg-asm with a source file and output to a specified file:

```
$ dmg-asm -s source.s -o path/to/output/file.gb
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
