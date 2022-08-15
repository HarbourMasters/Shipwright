; $MODE = "UniformRegister"

; $NUM_SPI_PS_INPUT_CNTL = 2
; Frag_UV R0
; $SPI_PS_INPUT_CNTL[0].SEMANTIC = 0
; $SPI_PS_INPUT_CNTL[0].DEFAULT_VAL = 1
; Frag_Color R1
; $SPI_PS_INPUT_CNTL[1].SEMANTIC = 1
; $SPI_PS_INPUT_CNTL[1].DEFAULT_VAL = 1

; $SAMPLER_VARS[0].name = "Texture"
; $SAMPLER_VARS[0].type = "SAMPLER2D"
; $SAMPLER_VARS[0].location = 0

; Note: R1 is swapped for endianness

00 TEX: ADDR(48) CNT(1) VALID_PIX
    0  SAMPLE R0, R0.xy0x, t0, s0
01 ALU: ADDR(32) CNT(4)
    1  x: MUL R0.x, R0.x, R1.w
       y: MUL R0.y, R0.y, R1.z
       z: MUL R0.z, R0.z, R1.y
       w: MUL R0.w, R0.w, R1.x
02 EXP_DONE: PIX0, R0
END_OF_PROGRAM
