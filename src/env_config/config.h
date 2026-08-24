#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <array>
/*
0."rax"  1."r10"  2."r9"  3."r8"
4."rcx"  5."rdx"  6."rsi"  7."rdi"

9."r11" 

"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", 
"xmm5", "xmm6", "xmm7", "xmm8", "xmm9", 
"xmm10", "xmm11", "xmm12", "xmm13", "xmm14"
function calling:
    linux: rdi, rsi, rdx, rcx, r8, and r9
    windows: rcx, rdx, r8, and r9
*/
// 9(common)-16(float)
#define X(name) extern char name[];
#define GENERAL_REGS_NUMBER 14
#define XMM_REGS_NUMBER 16
#include "regs_def.h"


#define INT_SIZE 8
#define CHAR_SIZE 1
#define FLOAT_SIZE 8

inline std::array general_regs = {
    rax, rbx, rcx, rdx, rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15
};

inline std::array xmm_regs = {
    xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15
};
inline std::array caller_saved = {
    rax, rcx, rdx, rsi, rdi, r8, r9, r10, r11, 
    xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15
};
inline std::array callee_saved = {
    rbx, r12, r13, r14, r15
};
inline std::array INTEGER_passing = {rdi, rsi, rdx, rcx, r8, r9};
inline std::array SSE_passing = {xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7};
#endif
