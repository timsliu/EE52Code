        NAME    DRAMTST

CGROUP  GROUP   CODE
CODE    SEGMENT PUBLIC 'CODE'
        ASSUME  CS:CGROUP

; Passed DRAM starting segment in ES.
;
; Checks ES:0000 to ES:FFFF (64KB). Performs a byte write followed by a byte
; read.
;
; Set a breakpoint at ByteReadError to check for errors.
DRAMByteTest  PROC    NEAR
              PUBLIC  DRAMByteTest
DRAMByteTestStart:
    MOV     BX, 0
    MOV     AL, 0

ByteWrite:
    ADD     AL, 3
    MOV     ES:[BX], AL
    INC     BX

CheckByte:
    CMP     AL, ES:[BX - 1]
    JE      ByteWrite

ByteReadError:
    NOP
    JMP     ByteReadError
DRAMByteTest  ENDP
    
; Passed DRAM starting segment in ES. 
;
; Checks ES:0000 to ES:FFFF (64KB). Performs word writes to the DRAM using an
; LFSR, and then repeatedly reads to verify the DRAM contents.
;
; Set a breakpoint at ReadError to check for errors. The offset at which the
; error occurred will be in BX, and the number of successful read loop
; iterations before failure will be in DX.
DRAMWordTest  PROC    NEAR
              PUBLIC  DRAMWordTest
DRAMTestStart:
    MOV     BX, 0
    MOV     AX, 1
    MOV     CX, 8000h   ; 8000h words in a segment
    MOV     DX, 0       ; Number of successful read loop iterations.

WriteLoop:
    MOV     ES:[BX], AX
    ADD     BX, 2
    SHL     AX, 1
    JNC     Skip1
    XOR     AX, 0B400H ; maximal length LFSR value

Skip1:
    LOOP    WriteLoop

StartRead:
    MOV     AX, 1
    MOV     CX, 8000h
    MOV     BX, 0

ReadLoop:
    CMP     ES:[BX], AX
    JNE     WordReadError
    ADD     BX, 2
    SHL     AX, 1
    JNC     Skip2
    XOR     AX, 0B400H

Skip2:
    LOOP    ReadLoop
    INC     DX
    JMP     StartRead

WordReadError:
    NOP
    JMP     WordReadError
DRAMWordTest  ENDP
        
CODE    ENDS
        END