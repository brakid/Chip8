:start
    CLS
    LD V0, 0x00
    LD V1, 0x00
    LD V2, 0x00
    LD F, V0
    DRW V1, V2, 0x5
    LD VF, K ; expected to be 0
    DRW V1, V2, 0x5
    LD VF, K ; expected to be 1
:end
    LD V0, K
    JP end