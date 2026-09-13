.code

; =============================================================================
; MASM x64 Trampolines and Mid-function Hooks for xp-progression
; Built with Microsoft Macro Assembler (ml64.exe)
; =============================================================================

; Helper Macros
PUSH_ALL MACRO
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
ENDM

POP_ALL MACRO
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
ENDM

PREPARE_STACK MACRO
    mov rax, rsp
    and rsp, -16
    push rax
    sub rsp, 28h
ENDM

RESTORE_STACK MACRO
    add rsp, 28h
    pop rsp
ENDM

; =============================================================================
; External Declarations (C++ Functions and Jump Target Pointers)
; =============================================================================

EXTERN XP_Overwrite:PROC
EXTERN sub_336F0:PROC
EXTERN LevelDisplayOverwriteCreature:PROC
EXTERN ASM_LevelDisplayOverwrite_jmpback:QWORD

EXTERN OverwriteItemName:PROC

EXTERN GetGoldDrops:PROC
EXTERN ASM_OverwriteGoldDrops_jmpback:QWORD

EXTERN RegionTextDrawOverwrite:PROC
EXTERN ASM_RegionTextDrawOverwrite_jmpback:QWORD
EXTERN ASM_RegionTextDrawOverwrite_bail:QWORD

; =============================================================================
; Trampolines Implementation
; =============================================================================

ASM_XP_Overwrite PROC
    ; Store rdx
    push rdx

    ; Make space on the stack
    mov rdx, rsp
    sub rsp, 110h

    ; Store xmm registers
    movaps xmmword ptr [rdx - 10h], xmm0
    movaps xmmword ptr [rdx - 20h], xmm1
    movaps xmmword ptr [rdx - 30h], xmm2
    movaps xmmword ptr [rdx - 40h], xmm3
    movaps xmmword ptr [rdx - 50h], xmm4
    movaps xmmword ptr [rdx - 60h], xmm5
    movaps xmmword ptr [rdx - 70h], xmm6
    movaps xmmword ptr [rdx - 80h], xmm7
    movaps xmmword ptr [rdx - 90h], xmm8
    movaps xmmword ptr [rdx - 0A0h], xmm9
    movaps xmmword ptr [rdx - 0B0h], xmm10
    movaps xmmword ptr [rdx - 0C0h], xmm11
    movaps xmmword ptr [rdx - 0D0h], xmm12
    movaps xmmword ptr [rdx - 0E0h], xmm13
    movaps xmmword ptr [rdx - 0F0h], xmm14
    movaps xmmword ptr [rdx - 100h], xmm15

    ; Store r15
    push r15

    PUSH_ALL

    PREPARE_STACK
    call XP_Overwrite
    RESTORE_STACK

    ; Store rax to r15 stored on the stack
    mov [rsp], rax

    POP_ALL

    ; Restore rax and pop r15
    mov rax, r15
    pop r15

    ; Restore XMM registers
    movaps xmm0, xmmword ptr [rdx - 10h]
    movaps xmm1, xmmword ptr [rdx - 20h]
    movaps xmm2, xmmword ptr [rdx - 30h]
    movaps xmm3, xmmword ptr [rdx - 40h]
    movaps xmm4, xmmword ptr [rdx - 50h]
    movaps xmm5, xmmword ptr [rdx - 60h]
    movaps xmm6, xmmword ptr [rdx - 70h]
    movaps xmm7, xmmword ptr [rdx - 80h]
    movaps xmm8, xmmword ptr [rdx - 90h]
    movaps xmm9, xmmword ptr [rdx - 0A0h]
    movaps xmm10, xmmword ptr [rdx - 0B0h]
    movaps xmm11, xmmword ptr [rdx - 0C0h]
    movaps xmm12, xmmword ptr [rdx - 0D0h]
    movaps xmm13, xmmword ptr [rdx - 0E0h]
    movaps xmm14, xmmword ptr [rdx - 0F0h]
    movaps xmm15, xmmword ptr [rdx - 100h]

    ; Restore stack
    add rsp, 110h

    ; Restore rdx
    pop rdx

    ret
ASM_XP_Overwrite ENDP

ASM_LevelDisplayOverwrite PROC
    xor r8d, r8d
    mov dl, 1
    lea rcx, [rbp + 78h]
    call sub_336F0

    lea rdx, [rbp - 80h]
    mov rcx, [r14]
    call LevelDisplayOverwriteCreature

    jmp qword ptr [ASM_LevelDisplayOverwrite_jmpback]
ASM_LevelDisplayOverwrite ENDP

ASM_OverwriteItemName PROC
    ; rdi: item
    ; rsi: wstring
    mov rdx, rsi
    lea rcx, [rbp + 60h]
    call OverwriteItemName

    ; Old code
    mov rax, rsi
    mov rcx, [rbp + 280h]
    xor rcx, rsp
    mov rbx, [rsp + 3C8h]
    add rsp, 390h
    pop rdi
    pop rsi
    pop rbp
    ret
ASM_OverwriteItemName ENDP

ASM_OverwriteGoldDrops PROC
    PUSH_ALL

    ; Put result on stack
    movq rax, xmm10
    push rax
    lea rdx, [rsp]

    ; Call function
    mov rcx, rsi

    PREPARE_STACK
    call GetGoldDrops
    RESTORE_STACK

    ; Get result
    pop rax
    movq xmm10, rax

    POP_ALL

    ; Old code
    mov ebx, r12d
    cvttss2si r14, xmm11

    jmp qword ptr [ASM_OverwriteGoldDrops_jmpback]
ASM_OverwriteGoldDrops ENDP

ASM_RegionTextDrawOverwrite PROC
    ; String is already set in rdx
    mov rcx, [r13 + 2D0h]
    call RegionTextDrawOverwrite

    test esi, esi
    jz region_bail

    jmp qword ptr [ASM_RegionTextDrawOverwrite_jmpback]

region_bail:
    jmp qword ptr [ASM_RegionTextDrawOverwrite_bail]
ASM_RegionTextDrawOverwrite ENDP

END
