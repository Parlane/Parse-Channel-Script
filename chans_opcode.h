#ifndef CHANS_OPCODE_H
#define CHANS_OPCODE_H

#include <wchar.h>

#define ARG_NONE 0
#define ARG_U8  1
#define ARG_U16 3
#define ARG_U32 5
#define ARG_U64 6
#define ARG_F64 7
#define ARG_EXT16U8 10
#define ARG_TEMP12 11
#define ARG_LOCAL12 12
#define ARG_EXT16 13
#define ARG_STR16 14
#define ARG_ARRAY_OP8 15
#define ARG_JMP1 16
#define ARG_JMP2 17
#define ARG_UNK 18

#define CO_EOC      0x00
#define CO_RETURN   0x01
#define CO_NEW      0x02

#define CO_PUSH_ACC 0x04
#define CO_05 0x05

#define CO_ADD      0x06
#define CO_SUB      0x07
#define CO_MUL      0x08
#define CO_DIV      0x09
#define CO_MOD      0x0a
#define CO_AND      0x0b
#define CO_OR       0x0c
#define CO_XOR      0x0d
#define CO_SHIFTL   0x0e
#define CO_SHIFTR   0x0f

#define CO_EQ       0x10
#define CO_NEQ      0x11
#define CO_LT       0x12
#define CO_GT       0x13
#define CO_LTE      0x14
#define CO_GTE      0x15

#define CO_NOT      0x17
#define CO_LOAD_S8  0x18
#define CO_ADD_IMM  0x19
#define CO_SUB_IMM  0x1a
#define CO_MUL_IMM  0x1b
#define CO_DIV_IMM  0x1c
#define CO_MOD_IMM  0x1d
#define CO_AND_IMM  0x1e
#define CO_OR_IMM   0x1f
#define CO_XOR_IMM  0x20
#define CO_EQ_IMM   0x21
#define CO_NEQ_IMM   0x22
#define CO_LT_IMM   0x23
#define CO_GT_IMM   0x24
#define CO_LTE_IMM  0x25
#define CO_GTE_IMM  0x26

#define CO_LOAD_U16 0x27
#define CO_LOAD_U32 0x28

#define CO_LOAD_F64 0x2a

#define CO_LOAD_STR 0x2c
#define CO_ARRAY_OP 0x2d


#define CO_CALL_EXT_SYMBOL_ARGS  0x30
#define CO_CALL_ACC_ARGS         0x31
#define CO_GET_EXT_SYMBOL_DEREF 0x32
#define CO_SET_EXT_SYMBOL_DEREF 0x33


// 12 bit operands
#define CO_GET_LOCAL_SYMBOL 0x40
#define CO_GET_TEMP         0x50
#define CO_SET_LOCAL_SYMBOL 0x60
#define CO_SET_TEMP         0x70
#define CO_8x               0x80
#define CO_POP_AND_BRANCH   0xa0
#define CO_Cx  0xc0
#define CO_POP_AND_BRANCH2  0xd0
#define CO_GOTO             0xe0




typedef struct
{
	int opcode;
	int length;
	int param_type;
	wchar_t *desc;
} chans_opcode_t;


extern chans_opcode_t chans_opcode_list[]; 

chans_opcode_t *chans_get_opcode(char *location);


#endif
