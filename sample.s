start:
    CLS ; comment
    LD V0, 0xFF
    LD V1, 0x1
    LD V2, 0x00
loop:
    SUB V0, V1
    SE V0, V2
    JP loop
    CALL sub
    JP start
sub:
    LD VF, 0x42
    RET