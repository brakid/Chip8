start:
    CLS
    LD V0, 0x00
    LD V1, 0x01
    LD V2, 0x02
    LD V3, 0x03
    LD V4, 0x04
    LD V5, 0x05
    LD V6, 0x06
    LD V7, 0x07
    LD V8, 0x08
    LD V9, 0x09
    LD VA, 0x0A
    LD VB, 0x0B
    LD VC, 0x0C
    LD VD, 0x0D
    LD VE, 0x0E
    LD VF, 0x0F
    LD I, 0x050
    LD [I], VF
    CALL function
    LD I, 0x050
    LD VF, [I]
    LD V0, K ; expected = 0
    LD V1, K ; expected = 1
    LD V2, K ; expected = 2
    LD V3, K ; expected = 3
    LD V4, K ; expected = 4
    LD V5, K ; expected = 5
    LD V6, K ; expected = 6
    LD V7, K ; expected = 7
    LD V8, K ; expected = 8
    LD V9, K ; expected = 9
    LD VA, K ; expected = A
    LD VB, K ; expected = B
    LD VC, K ; expected = C
    LD VD, K ; expected = D
    LD VE, K ; expected = E
    LD VF, K ; expected = F
    CALL printing
end:
    LD V0, K
    JP end

function:
    LD I, 0x050
    LD V0, 0x01
    LD V1, 0x00 ; x coordinate
    LD V2, 0x00 ; y coordinate
    LD V3, 0x10 ; 16 repetitions
    LD V4, 0x00 ; loop variable
loop:
    DRW V1, V2, 0x1 ; draw sprite
    ADD V2, 0x01
    ADD I, V0 ; increase memory pointer by 1
    ADD V4, 0x01
    SE V3, V4 ; if V3 == V4 == 16
    JP loop
    RET

printing:
    LD V0, 0x90 ; 144
    LD I, 0x000
    LD B, V0
    LD V2, [I] ; V0 = 1, V1 = 4, V2 = 4
    LD V3, 0x00
    LD V4, 0x14
    LD F, V0
    DRW V3, V4, 0x5
    ADD V3, 0x0A
    LD F, V1
    DRW V3, V4, 0x5
    ADD V3, 0x0A
    LD F, V2
    DRW V3, V4, 0x5
    RET