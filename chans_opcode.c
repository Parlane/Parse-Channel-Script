#include <stdlib.h>
#include "chans_opcode.h"
#include "util.h"

chans_opcode_t chans_4bit_opcode_list[] =
{
	{CO_GET_LOCAL_SYMBOL, 2, ARG_LOCAL12, L"ACC = %s"},
	{CO_GET_TEMP, 2, ARG_TEMP12, L"ACC = temp_%03hx"},
	{CO_SET_LOCAL_SYMBOL, 2, ARG_LOCAL12, L"%s = ACC"},
	{CO_SET_TEMP, 2, ARG_TEMP12, L"temp_%03hx = ACC"},
	{CO_POP_AND_BRANCH, 2, ARG_JMP2, L"IF(!POP) PC = PC + 0x%04hx [dst: 0x%08x]"},
	{CO_POP_AND_BRANCH2, 2, ARG_JMP1, L"IF(!POP) PC = PC + 0x%04hx + 1 [dst: 0x%08x]"},
	{CO_GOTO, 2, ARG_JMP2, L"PC = PC + %04hx [dst: 0x%08x]"},
};

chans_opcode_t chans_8bit_opcode_list[] =
{
	{CO_EOC, 1, ARG_NONE, L"END OF CODE"},
	{CO_RETURN, 1, ARG_NONE, L"RETURN ACC"},
	{CO_NEW, 2, ARG_U8, L"NEW [0x%02hhx args]"},
	{CO_PUSH_ACC, 1, ARG_NONE, L"PUSH ACC"},
	{CO_ADD, 1, ARG_NONE, L"ACC = POP + ACC"},
	{CO_SUB, 1, ARG_NONE, L"ACC = POP - ACC"},
	{CO_MUL, 1, ARG_NONE, L"ACC = POP * ACC"},
	{CO_DIV, 1, ARG_NONE, L"ACC = POP / ACC"},
	{CO_MOD, 1, ARG_NONE, L"ACC = POP MOD ACC"},
	{CO_AND, 1, ARG_NONE, L"ACC = POP & ACC"},
	{CO_OR, 1, ARG_NONE, L"ACC = POP | ACC"},
	{CO_XOR, 1, ARG_NONE, L"ACC = POP ^ ACC"},
	{CO_SHIFTL, 1, ARG_NONE, L"ACC = POP << ACC [NEEDS CONFIRMATION]"},
	{CO_SHIFTR, 1, ARG_NONE, L"ACC = POP >> ACC [NEEDS CONFIRMATION]"},
	{CO_EQ, 1, ARG_NONE, L"ACC = (POP == ACC)"},
	{CO_NEQ, 1, ARG_NONE, L"ACC = (POP != ACC)"},
	{CO_LT, 1, ARG_NONE, L"ACC = (POP < ACC)"},
	{CO_GT, 1, ARG_NONE, L"ACC = (POP > ACC)"},
	{CO_NOT, 1, ARG_NONE, L"ACC = !ACC"},
	{CO_LOAD_S8, 2, ARG_U8, L"ACC = 0x%02hhx"},
	{CO_ADD_IMM, 2, ARG_U8, L"ACC = ACC + 0x%02hhx"},
	{CO_SUB_IMM, 2, ARG_U8, L"ACC = ACC - 0x%02hhx"},
	{CO_MUL_IMM, 2, ARG_U8, L"ACC = ACC * 0x%02hhx"},
	{CO_DIV_IMM, 2, ARG_U8, L"ACC = ACC / 0x%02hhx"},
	{CO_MOD_IMM, 2, ARG_U8, L"ACC = ACC MOD 0x%02hhx"},
	{CO_AND_IMM, 2, ARG_U8, L"ACC = ACC & 0x%02hhx"},
	{CO_OR_IMM, 2, ARG_U8, L"ACC = ACC | 0x%02hhx"},
	{CO_XOR_IMM, 2, ARG_U8, L"ACC = ACC ^ 0x%02hhx"},
	{CO_EQ_IMM, 2, ARG_U8, L"ACC = (ACC == 0x%02hhx)"},
	{CO_NEQ_IMM, 2, ARG_U8, L"ACC = (ACC != 0x%02hhx)"},
	{CO_LT_IMM, 2, ARG_U8, L"ACC = (ACC < 0x%02hhx)"},
	{CO_GT_IMM, 2, ARG_U8, L"ACC = (ACC > 0x%02hhx)"},
	{CO_LTE_IMM, 2, ARG_U8, L"ACC = (ACC <= 0x%02hhx)"},
	{CO_GTE_IMM, 2, ARG_U8, L"ACC = (ACC >= 0x%02hhx) [NEEDS CONFIRMATION]"},
	{CO_LOAD_U16, 3, ARG_U16, L"ACC = 0x%04hx"},
	{CO_LOAD_U32, 5, ARG_U32, L"ACC = 0x%08x"},
	{CO_LOAD_F64, 9, ARG_F64, L"ACC = %f"},
	{CO_LOAD_STR, 3, ARG_STR16, L"ACC = \"%ls\""},
	{CO_ARRAY_OP, 2, ARG_ARRAY_OP8, L"ARRAY_OPERATION"},

	{CO_CALL_EXT_SYMBOL_ARGS, 4, ARG_EXT16U8, L"CALL ACC.%s WITH 0x%02hhx PARAMS FROM STACK"},
	{CO_CALL_ACC_ARGS, 2, ARG_U8, L"CALL ACC WITH 0x%02hhx PARAMS FROM STACK"},
	{CO_GET_EXT_SYMBOL_DEREF, 3, ARG_EXT16, L"GET ACC.%s"},
	{CO_SET_EXT_SYMBOL_DEREF, 3, ARG_EXT16, L"SET ACC.%s = POP"},
};


chans_opcode_t *chans_get_opcode(char *location)
{
	u8 opcode = *location;
	chans_opcode_t *rv = NULL;
	if(opcode < 0x40)
	{
		for(size_t i = 0; i < sizeof(chans_8bit_opcode_list)/sizeof(chans_8bit_opcode_list[0]); i++)
		{
			if(chans_8bit_opcode_list[i].opcode == opcode)
			{
				rv = &chans_8bit_opcode_list[i];
				break;
			}
		}
	}
	else
	{
		opcode = opcode & 0xf0;
		for(size_t i = 0; i < sizeof(chans_4bit_opcode_list)/sizeof(chans_4bit_opcode_list[0]); i++)
		{
			if(chans_4bit_opcode_list[i].opcode == opcode)
			{
				rv = &chans_4bit_opcode_list[i];
				break;
			}
		}
	}

	return rv;
}

