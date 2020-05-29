start:
    CLS
    LD V0, 0x0A ; set V0 to 0xA
    LD V1, 0x00 ; set V1 to 0x0
    LD V2, 0x00 ; set V2 to 0x0
    LD V3, 0x00 ; set V3 to 0x0
loop:
    LD V0, K ; wait for key press
    LD F, V0 ; set I to point to digit VO (value)
    DRW V1, V2, 0x5 ; draw sprite
    ADD V1, 0x08 ; increase V1 by 8
    ADD V2, 0x05 ; increase V2 by 5
    ADD V3, 0x01 ; increase V3 by 1
    SE V3, 0x03 ; if V3 has value 3
    JP loop
    LD V1, 0x20
    RND V2, 0x0F ; random number between 0 and 15
    ADD V2, 0x0A ; add 10
    LD I, heart_data_1 
    DRW V1, V2, 0x5 ; draw sprite
    ADD V2, 0x05 ; increase V2 by 5
    LD I, heart_data_2
    DRW V1, V2, 0x1 ; draw sprite
    LD V0, 0x0A
    LD ST, V0
    LD VF, K ; wait for key press
    JP start
heart_data_1:
    .byte 0x24
    .byte 0x5a
    .byte 0x81
    .byte 0x42
    .byte 0x24
heart_data_2:
    .byte 0x18