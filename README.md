## Chip 8 interpreter and assembler

Following the specification as in: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

### Assembler (written in Python)
Translates the mnemonics as described in http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.1 into the corresponding OP codes
It generates a binary file containing the OP codes bytes of the parsed program

Supports data in the assembler file using the directives .byte or .word (2 byte long)

#### Usage:
Example usage: ´´´./assembler.py abc_display.s abc_display.out´´´


### Interpreter (written in C++)
Reads a binary in the memory and executes it.
Current limitations:
* at each cycle, a key press is required - even when no keyboard input is expected - TODO: non-blocking keyboard reads, currently solved by adding a command line argument
* printing the screen is not happening all the time

#### Usage:
Build the interpreter via ´´´make´´´

Example usage: ´´´./chip8cpu.out abc_display.out nonblocking´´´
