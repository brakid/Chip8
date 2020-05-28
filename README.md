## Chip 8 interpreter and assembler

Following the specification as in: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

### Assembler (written in Python)
Translates the mnemonics as described in http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.1 into the corresponding OP codes
It generates a binary file containing the OP codes bytes of the parsed program

Supports data in the assembler file using the directives ```.byte``` or ```.word``` (2 byte long)

Supports labels to code addresses for jumps and for data access.

#### Usage:
Example usage: ```./assembler.py abc_display.s abc_display.out```

### Interpreter (written in C++)
Reads a binary in the memory and executes it. Uses ```ncurses```library https://en.wikipedia.org/wiki/Ncurses for display management and non-blocking keyboard inputs.

Ncurses usage explanation: http://www.cs.ukzn.ac.za/~hughm/os/notes/ncurses.html

#### Usage:
Build the interpreter via ```make```

Example usage: ```./chip8cpu.out abc_display.out 50```
