#pragma once

#include "../main.h"
#include "features/drops/DropSystem.h"

extern "C" inline void GetGoldDrops(cube::Creature* creature, float* gold)
{
    pyro::DropSystem::ProcessGoldDrops(creature, gold);
}

GETTER_VAR(void*, ASM_OverwriteGoldDrops_jmpback);

#if defined(__GNUC__) || defined(__clang__)
NAKED_FN void ASM_OverwriteGoldDrops() {
    asm(".intel_syntax \n"
        
        PUSH_ALL

        // Put result on stack
        "movq rax, xmm10 \n"
        "push rax \n"
        "lea rdx, [rsp] \n"

        // Call function
        "mov rcx, rsi \n"

        PREPARE_STACK
        "call GetGoldDrops \n"
        RESTORE_STACK

        // Get result
        "pop rax \n"
        "movq xmm10, rax \n"

        POP_ALL

        // Old code
        "mov ebx, r12d \n"
        "cvttss2si r14, xmm11 \n"

        DEREF_JMP(ASM_OverwriteGoldDrops_jmpback)
    );
}
#else
inline void ASM_OverwriteGoldDrops() {}
#endif

inline void Setup_OverwriteGoldDrops() {
#if defined(__GNUC__) || defined(__clang__)
    WriteFarJMP(CWOffset(0x2A752C), reinterpret_cast<void*>(&ASM_OverwriteGoldDrops));
    ASM_OverwriteGoldDrops_jmpback = reinterpret_cast<void*>(CWOffset(0x2A7540));
#endif
}