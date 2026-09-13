#pragma once

#include <vector>
#include <cwchar>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <map>
#include <windows.h>
#include "cwsdk.h"

// Assembly and compiler portability macros
#if defined(__GNUC__) || defined(__clang__)
    #define no_optimize __attribute__((optimize("O0")))
    #define NAKED_FN __attribute__((naked))
    #define USED_VAR __attribute__((used))
#else
    #define no_optimize
    #define NAKED_FN
    #define USED_VAR
#endif

#define PUSH_ALL "push rax\npush rbx\npush rcx\npush rdx\npush rsi\npush rdi\npush rbp\npush r8\npush r9\npush r10\npush r11\npush r12\npush r13\npush r14\npush r15\n"
#define POP_ALL "pop r15\npop r14\npop r13\npop r12\npop r11\npop r10\npop r9\npop r8\npop rbp\npop rdi\npop rsi\npop rdx\npop rcx\npop rbx\npop rax\n"

#define PREPARE_STACK "mov rax, rsp \n and rsp, 0xFFFFFFFFFFFFFFF0 \n push rax \n sub rsp, 0x28 \n"
#define RESTORE_STACK "add rsp, 0x28 \n pop rsp \n"

#define GETTER_VAR(vartype, varname)\
    extern "C" vartype varname = 0;\
    extern "C" inline vartype Get_##varname(){ return varname; }

