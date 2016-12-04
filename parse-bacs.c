/*
**	Author: Matthew Parlane, booto
**	Email: matthew_parlane@ hot mail [dot] (com)
*/
#define _GNU_SOURCE
#include <iconv.h>
#include <errno.h>

#include <stdlib.h>
#include <stdint.h>

#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include <locale.h>


#include "util.h"
#include "chans.h"
#include "chans_opcode.h"
#include "debug.h"

static FILE *fp;

char ** exportedMethods = NULL;
char ** importedMethods = NULL;
wchar_t ** stringLiterals = NULL;

/* BEGIN: Matthews temporary placement of structure defines */


	typedef struct stack_node stack_node;
	struct stack_node
	{
		char * string;
		stack_node * next;
	};
	typedef struct {
		u32 offset;
		u8 data[0x20];
	} table5Block;

	stack_node * rootStack = NULL;
	char * ACC = NULL;
	char * ACCBackup = NULL;

	void pushStack(char * string){
		stack_node * newNode = calloc(1,sizeof(stack_node));
		asprintf(&newNode->string, "%s", string);
		if (rootStack){
			newNode->next = rootStack;
		}
		rootStack = newNode;
	}
	char * popStack(){
		if(!rootStack) return NULL;
		stack_node * temp = rootStack->next;
		char * ret = rootStack->string;
		free(rootStack);
		rootStack = temp;
		return ret;
	}
	void flushACC(int offset){
		if(ACC != NULL){
			wprintf(L"%*s%s;\n", offset,"", ACC);
			free(ACC);
		}
		ACC = NULL;
	}
	void freeACC(int print, int offset){
		if(ACC){
			free(ACC);
		}
		ACC = NULL;
	}

	char * freeGetACC(){

		asprintf(&ACCBackup,"%s", ACC);
		if(ACC){
			free(ACC);
		}
		ACC = NULL;
		return ACCBackup;
	}

/* END: Matthews temporary placement of structure defines */



table5Block * byteBlocks = NULL;

int startOfLine(int offset){
	int block = (offset+255)/256 - 1;
	int blockoff = offset & 0xFF;
	int byte = (blockoff)/8;
	int byteoff = 8 - (offset & 0x07) - 1;
	u8 blockbyte = byteBlocks[block].data[byte];
	return (blockbyte >> byteoff) & 1;
}


char * getFunctionCall(int numOfArgs){
	int i = 0;
	char * temp = NULL;
	char * tempPop = NULL;
	char * functionCall = NULL;
	for(i=0; i<numOfArgs; i++){
		if(i){
			tempPop = popStack();
			asprintf(&temp, "%s, %s", tempPop, functionCall);
			free(functionCall);
			free(tempPop);
			functionCall = temp;
		}else{
			tempPop = popStack();
			asprintf(&functionCall, "%s", tempPop);
			free(tempPop);
		}
	}
	if(!numOfArgs) asprintf(&functionCall, "");
	return functionCall;
}

void parseByteCodeToCode(u8 * data, int length, int offset, int pad){
	int i = 0, j = 0, argcount = 0;
	char * funcname = NULL;
	char * args = NULL;
	char * temp = NULL;
	char * tempPop = NULL;
	i = 0;
	//wprintf(L"\tBytecode Segmented:\n");
	while(i<length){
		chans_opcode_t *co = chans_get_opcode(data+i);

		if(startOfLine(offset + i)){
			flushACC(pad);
		}

		if(co==NULL)
		{
			dbgprintf("unknown opcode: %hhx\n", *(data+i));
			exit(1);
		}
		switch(co->opcode)
		{
			case CO_RETURN:
				wprintf(L"%*sreturn %s;\n", pad,"", ACC);
				freeACC(0,pad);
				break;
			case CO_NEW:
				argcount = *U8P(data+i+1);
				funcname = freeGetACC();
				args = getFunctionCall(argcount);
				asprintf(&ACC, "new %s(%s)", funcname,args);
				free(funcname);
				free(args);
				break;
      case CO_03:
        wprintf(L"%*sunknown_op_03 %d;\n", pad,"", be16(*U16P(data+i+1)));
        break;
			case CO_PUSH_ACC:
				pushStack(ACC);
				freeACC(0,pad);
				//wprintf(L"       PUSHED TO STACK: NOW ACC = %s\n", ACC);
				break;
			case CO_ADD:
				temp = freeGetACC();
				tempPop = popStack();
				asprintf(&ACC, "(%s + %s)", tempPop, temp);
				free(temp);
				free(tempPop);
				break;
			case CO_SUB:
				temp = freeGetACC();
				tempPop = popStack();
				asprintf(&ACC, "(%s - %s)", tempPop, temp);
				free(temp);
				free(tempPop);
				break;
			case CO_MUL:
				temp = freeGetACC();
				tempPop = popStack();
				asprintf(&ACC, "(%s * %s)", tempPop, temp);
				free(temp);
				free(tempPop);
				break;
			case CO_DIV:
				temp = freeGetACC();
				tempPop = popStack();
				asprintf(&ACC, "(%s / %s)", tempPop, temp);
				free(temp);
				free(tempPop);
				break;
			case CO_AND:
				temp = freeGetACC();
				tempPop = popStack();
				asprintf(&ACC, "(%s & %s)", tempPop, temp);
				free(temp);
				free(tempPop);
				break;
			case CO_SHIFTL:
				temp = freeGetACC();
				tempPop = popStack();
				asprintf(&ACC, "%s << %s", tempPop, temp);
				free(temp);
				free(tempPop);
				break;
			case CO_SHIFTR:
				temp = freeGetACC();
				tempPop = popStack();
				asprintf(&ACC, "%s >> %s", tempPop, temp);
				free(temp);
				free(tempPop);
				break;
			case CO_EQ:
				temp = freeGetACC();
				tempPop = popStack();
				asprintf(&ACC, "%s == %s", tempPop, temp);
				free(temp);
				free(tempPop);
				break;
			case CO_NEQ:
				temp = freeGetACC();
				tempPop = popStack();
				asprintf(&ACC, "(%s != %s)", tempPop, temp);
				free(temp);
				free(tempPop);
				break;
			case CO_LT:
				temp = freeGetACC();
				tempPop = popStack();
				asprintf(&ACC, "(%s < %s)", tempPop, temp);
				free(temp);
				free(tempPop);
				break;
			case CO_GT:
				temp = freeGetACC();
				tempPop = popStack();
				asprintf(&ACC, "(%s > %s)", tempPop, temp);
				free(temp);
				free(tempPop);
				break;
			case CO_LTE:
				temp = freeGetACC();
				tempPop = popStack();
				asprintf(&ACC, "(%s <= %s)", tempPop, temp);
				free(temp);
				free(tempPop);
				break;
			case CO_GTE:
				temp = freeGetACC();
				tempPop = popStack();
				asprintf(&ACC, "(%s >= %s)", tempPop, temp);
				free(temp);
				free(tempPop);
				break;
			case CO_NOT:
				temp = freeGetACC();
				asprintf(&ACC, "!(%s)", temp);
				free(temp);
				break;
			case CO_LOAD_S8:
				freeACC(1,pad);
				asprintf(&ACC, "%d", *U8P(data+i+1));
				break;
			case CO_ADD_IMM:
				temp = freeGetACC();
				asprintf(&ACC, "(%s + %d)", temp, *U8P(data+i+1));
				free(temp);
				break;
			case CO_SUB_IMM:
				temp = freeGetACC();
				asprintf(&ACC, "(%s - %d)", temp, *U8P(data+i+1));
				free(temp);
				break;
			case CO_MUL_IMM:
				temp = freeGetACC();
				asprintf(&ACC, "(%s * %d)", temp, *U8P(data+i+1));
				free(temp);
				break;
			case CO_DIV_IMM:
				temp = freeGetACC();
				asprintf(&ACC, "(%s / %d)", temp, *U8P(data+i+1));
				free(temp);
				break;
			case CO_MOD_IMM:
				temp = freeGetACC();
				asprintf(&ACC, "(%s %% %d)", temp, *U8P(data+i+1));
				free(temp);
				break;
			case CO_EQ_IMM:
				temp = freeGetACC();
				asprintf(&ACC, "(%s == %d)", temp, *U8P(data+i+1));
				free(temp);
				break;
			case CO_NEQ_IMM:
				temp = freeGetACC();
				asprintf(&ACC, "(%s != %d)", temp, *U8P(data+i+1));
				free(temp);
				break;
			case CO_LT_IMM:
				temp = freeGetACC();
				asprintf(&ACC, "(%s < %d)", temp, *U8P(data+i+1));
				free(temp);
				break;
			case CO_GT_IMM:
				temp = freeGetACC();
				asprintf(&ACC, "(%s > %d)", temp, *U8P(data+i+1));
				free(temp);
				break;
			case CO_LTE_IMM:
				temp = freeGetACC();
				asprintf(&ACC, "(%s <= %d)", temp, *U8P(data+i+1));
				free(temp);
				break;
			case CO_GTE_IMM:
				temp = freeGetACC();
				asprintf(&ACC, "(%s >= %d)", temp, *U8P(data+i+1));
				free(temp);
				break;
			case CO_LOAD_U16:
				freeACC(1,pad);
				asprintf(&ACC, "%d", be16(*U16P(data+i+1)));
				break;
			case CO_LOAD_U32:
				freeACC(1,pad);
				asprintf(&ACC, "%ld", be32(*U32P(data+i+1)));
				break;
			case CO_LOAD_F64:
				freeACC(1,pad);
				asprintf(&ACC, "%f", bef64(*F64P(data+i+1)));
				break;
			case CO_LOAD_STR:
				freeACC(1,pad);
				asprintf(&ACC, "\"%ls\"", stringLiterals[be16(*U16P(data+i+1))]);
				//wprintf(L"       NOW ACC = %s\n", ACC);
				break;
			case CO_GET_LOCAL_SYMBOL:
				freeACC(1,pad);
				asprintf(&ACC, "%s", exportedMethods[be16(*U16P(data+i)) & 0xfff]);
				//wprintf(L"       NOW ACC = %s\n", ACC);
				break;
			case CO_CALL_ACC_ARGS:
				argcount = *U8P(data+i+1);
				funcname = freeGetACC();
				args = getFunctionCall(argcount);
				asprintf(&ACC, "%s(%s)", funcname,args);
				free(funcname);
				free(args);
				break;
			case CO_GET_EXT_SYMBOL_DEREF:
				temp = freeGetACC();

				asprintf(&ACC, "%s.%s",temp, importedMethods[be16(*U16P(data+i+1))]);
				free(temp);
				break;
			case CO_SET_EXT_SYMBOL_DEREF:
				tempPop = popStack();
				wprintf(L"%*s%s.%s = %s;\n", pad,"", ACC, importedMethods[be16(*U16P(data+i+1))], tempPop);
				free(tempPop);
				freeACC(0,pad);
				break;
			case CO_SET_LOCAL_SYMBOL:
				wprintf(L"%*s%s = %s;\n", pad,"",exportedMethods[be16(*U16P(data+i)) & 0xfff], ACC);
				freeACC(0,pad);
				break;
			case CO_ARRAY_OP:
				switch(*U8P(data+i+1)){
					case 0x3e:
						temp = freeGetACC();
						tempPop = popStack();
						asprintf(&ACC, "%s[%s]", tempPop, temp);
						free(temp);
						free(tempPop);
						break;
					case 0x3f:
						temp = popStack();
						tempPop = popStack();
						wprintf(L"%*s%s[%s] = %s;\n", pad,"", tempPop, ACC, temp);
						free(temp);
						free(tempPop);
						freeACC(0,pad);
						break;
					default:
						wprintf(L" UNKNOWN ARRAY OP");
						break;
				}
				break;
			case CO_CALL_EXT_SYMBOL_ARGS:
				argcount = *U8P(data+i+3);
				funcname = freeGetACC();
				args = getFunctionCall(argcount);
				asprintf(&ACC, "%s.%s(%s)",funcname,importedMethods[be16(*U16P(data+i+1))], args);
				free(funcname);
				free(args);
				break;
			case CO_GET_TEMP:
				freeACC(1,pad);
				asprintf(&ACC, "unamedVar_%d", be16(*U16P(data+i)) & 0xfff);
				break;
			case CO_SET_TEMP:
				wprintf(L"%*sunamedVar_%d = %s; \n", pad,"", be16(*U16P(data+i)) & 0xfff, ACC);
				freeACC(0,pad);
				break;
			case CO_EOC:
				flushACC(pad);
				freeACC(1,pad);
				break;
			case CO_POP_AND_BRANCH:
				// here goes :(
				tempPop = freeGetACC();
				wprintf(L"%*sif(%s){\n", pad,"", tempPop);
				parseByteCodeToCode(&data[i+co->length], (be16(*U16P(data+i)) & 0xfff), offset+i+co->length, pad+5);
				freeACC(1,pad+5);
				chans_opcode_t *cotemp = chans_get_opcode(data + i + (be16(*U16P(data+i)) & 0xfff));
				if(cotemp != NULL && cotemp->opcode == CO_GOTO){
					wprintf(L"%*s}else{\n", pad,"");
					parseByteCodeToCode(data + i + (be16(*U16P(data+i)) & 0xfff), (be16(*U16P(data + i + (be16(*U16P(data+i)) & 0xfff))) & 0xfff), offset + i + (be16(*U16P(data+i)) & 0xfff), pad);
					freeACC(1,pad+5);
					//wprintf(L"start = 0x%04X\n", i + (be16(*U16P(data+i)) & 0xfff));
					i += (be16(*U16P(data+i)) & 0xfff) + be16(*U16P(data + i + (be16(*U16P(data+i)) & 0xfff))) & 0xfff;

				}else{
					i += be16(*U16P(data+i)) & 0xfff;
				}
				wprintf(L"%*s}\n", pad,"");
				free(tempPop);
				break;
			case CO_GOTO:
				break;
			default:
				wprintf(L"%*s0x%02X Wtf code?\n", pad,"", co->opcode);
				break;
		}
		i += co->length;
	}

}

void parseByteCode(u8 * data, int length, int offset){
	int i = 0, j = 0;
/*
	wprintf(L"\tBytecode:\n\t\t");
	for(i=0; i<length; i++){
		wprintf(L"%02X ", data[i]);
		if(!((i+1)%16))
			wprintf(L"\n\t\t");
	}
	wprintf(L"\n");
	*/
	i = 0;
	wprintf(L"\tBytecode Segmented:\n");
	while(i<length){
		chans_opcode_t *co = chans_get_opcode(data+i);

		if(co==NULL)
		{

			wprintf(L"unknown opcode: %hhx\n", *(data+i));
			break;
		}

		wprintf(L"        ");
		wprintf(L"%dx%03X (",startOfLine(offset + i), i);

		wprintf(L"%02X", data[i]);
		for(j=1; j < co->length; j++)
		{
			wprintf(L" %02X", data[i+j]);
		}
		wprintf(L")");
		for(j=co->length*3; j<30; j++){
			wprintf(L" ");
		}
		switch(co->param_type)
		{
			default:
			case ARG_UNK:
			{
				wprintf(L"UNKNOWN");
			}
			case ARG_NONE:
			{
				wprintf(co->desc);
				break;
			}
			case ARG_U8:
			{
				wprintf(co->desc, *U8P(data+i+1));
				break;
			}
			case ARG_U16:
			{
				wprintf(co->desc, be16(*U16P(data+i+1)));
				break;
			}
			case ARG_U32:
			{
				wprintf(co->desc, be32(*U32P(data+i+1)));
				break;
			}
			case ARG_U64:
			{
				wprintf(co->desc, be64(*U64P(data+i+1)));
				break;
			}
			case ARG_F64:
			{
				wprintf(co->desc, bef64(*F64P(data+i+1)));
				break;
			}
			case ARG_EXT16U8:
			{
				wprintf(co->desc, importedMethods[be16(*U16P(data+i+1))], *U8P(data+i+3));
				break;
			}
			case ARG_TEMP12:
			{
				wprintf(co->desc, be16(*U16P(data+i)) & 0xfff);
				break;
			}
			case ARG_LOCAL12:
			{
				wprintf(co->desc, exportedMethods[be16(*U16P(data+i)) & 0xfff]);
				break;
			}
			case ARG_EXT16:
			{
				wprintf(co->desc, importedMethods[be16(*U16P(data+i+1))]);
				break;
			}
			case ARG_STR16:
			{
				wprintf(co->desc, stringLiterals[be16(*U16P(data+i+1))]);
				break;
			}
			case ARG_ARRAY_OP8:
			{
				switch(*U8P(data+i+1))
				{
					wprintf(co->desc);
					case 0x3e:
					{
						wprintf(L" ACC = POP[ACC]");
						break;
					}
					case 0x3f:
					{
						wprintf(L" POP[ACC] = POP");
						break;
					}
					default:
					{
						wprintf(L" UNKNOWN ARRAY OP");
						break;
					}
				}
				break;
			}
			case ARG_JMP1:
			{
				s16 offset = (be16(*U16P(data+i)) & 0xfff) + 1;
				if(offset & 0x800) offset = offset + 0xf000;
				wprintf(co->desc, offset, i + offset);
				break;
			}
			case ARG_JMP2:
			{
				u16 offset = (be16(*U16P(data+i))  & 0xfff) + 2;
				wprintf(L"0x%04x: ", be16(*U16P(data+i)));
				wprintf(co->desc, offset, i + offset);
				break;
			}

		}

		wprintf(L"\n");
		i += co->length;
	}


}

int main(int argc, char * argv[]){

	const char   fromcode[] = "UTF-16BE";
	const char   tocode[] = "WCHAR_T";
	iconv_t      cd;
	u8 * FDS = NULL;

	if (!setlocale(LC_CTYPE, "")) {
		return 1;
	}

	if ((iconv_t)(-1) == (cd = iconv_open(tocode, fromcode))) {
		wprintf(L"Failed to iconv_open %s to %s.\n", fromcode, tocode);
		exit(EXIT_FAILURE);
	}

	bacs_header HEAD;

	int offset = 0;
	int i = 0, j = 0;
	if (argc == 2) {
		fp = fopen(argv[1], "rb");
	} else {
		wprintf(L"Usage: %s <file>\n", argv[0]);
		return EXIT_FAILURE;
	}


	fread(&HEAD, sizeof(bacs_header), 1, fp);
	wprintf(L"File type: %.4s\n\n", (char *) &HEAD.head);

	// Grab bytecode section
	FDS = calloc(1, be32(HEAD.fds_size));
	fseek(fp, 0x20 + be32(HEAD.fds_offset),SEEK_SET);
	fread(FDS, be32(HEAD.fds_size), 1, fp);


	//Table 1: Local Methods

	table1_chunk * methodTable;
	fseek(fp, 0x20 + be32(HEAD.table1_offset),SEEK_SET);

	methodTable = calloc(sizeof(table1_chunk), be32(HEAD.table1_count));
	for(i=0; i<be32(HEAD.table1_count);i++){
		fread(&methodTable[i], 8, 1, fp);
	}


	//Table 2: Imported Methods/Variables
	table2_key tempkey;
	char * tempname = NULL;
	long curpos = 0;

	offset = 0x20 + be32(HEAD.table2_offset);
	fseek(fp, offset,SEEK_SET);
	importedMethods = calloc(be32(HEAD.table2_count), sizeof(char **));
	for(i=0; i<be32(HEAD.table2_count);i++){
		fread(&tempkey, sizeof(table2_key), 1, fp);
		if(tempkey.length != 0){
			curpos=ftell (fp);
			importedMethods[i] = calloc(1, tempkey.length + 1);
			fseek(fp, offset+be16(tempkey.offset), SEEK_SET);
			fread(importedMethods[i], tempkey.length, 1, fp);
			fseek(fp, curpos, SEEK_SET);
		}
	}


	//Table 3: String Literals
	u16 length;
	wchar_t * nameout = NULL;
	size_t in_size = 0;
	size_t out_size = 0;
	offset = 0x20 + be32(HEAD.table3_offset);
	fseek(fp, offset,SEEK_SET);
	stringLiterals = calloc(be32(HEAD.table3_count), sizeof(wchar_t **));

	for(i=0; i<be32(HEAD.table3_count);i++){

		fread(&length, 2, 1, fp);
		tempname = calloc(1, be16(length));
		fread(tempname, be16(length), 1, fp);

		stringLiterals[i] = nameout = calloc(1, be16(length)*2 + 4);
		in_size = be16(length);
		out_size = be16(length)*2;

		char         *inptr = tempname;
		char         *outptr = (char *) nameout;

		if (-1 == iconv(cd, &inptr, &in_size, &outptr, &out_size)) {
			memset(nameout, 191, be16(length)/2);
			wprintf(L"Failed to convert characters to new code set.\n");

			wprintf(L"errno: %d\n", errno);
			//printf("byte length: %d\n", be16(length));
			int j = 0;
			for(j=0; j<be16(length);j++){
				//printf("0x%02X ", (u8) tempname[j]);
			}
			//printf("\n");
			//exit(EXIT_FAILURE);
		}



		free(tempname);


		int j = 0;
		wchar_t tempchar = 0;
		for(j=0; j<be16(length)/2;j++){
			tempchar = nameout[j];
			if (tempchar == 10 || tempchar == 13){
				nameout[j] = 0xB6;
			}
		}

		tempname = NULL;
		nameout = NULL;
	}


	//Table 4: Exported Methods/Variables
	offset = 0x20 + be32(HEAD.table4_offset);
	fseek(fp, offset,SEEK_SET);
	exportedMethods = calloc(be32(HEAD.table4_count), sizeof(char **));
	for(i=0; i<be32(HEAD.table4_count);i++){
		fread(&tempkey, sizeof(table2_key), 1, fp);
		if(tempkey.length != 0){
			curpos=ftell (fp);
			exportedMethods[i] = tempname = calloc(1, tempkey.length + 1);
			fseek(fp, offset+be16(tempkey.offset), SEEK_SET);
			fread(tempname, tempkey.length, 1, fp);
			fseek(fp, curpos, SEEK_SET);
			tempname = NULL;
		}
	}
	int blockCount = (be32(HEAD.fds_size)+255)/256;

	//Table 5: Exported Methods/Variables
	byteBlocks = calloc(sizeof(table5Block), blockCount);
	offset = 0x20 + be32(HEAD.table5_offset);
	fseek(fp, offset,SEEK_SET);
	for(i=0; i<blockCount;i++){
		fread(&byteBlocks[i], 0x24, 1, fp);
	}






	int bytelength = 0;
	wprintf(L"Local Methods (Table 1):\n");

	wprintf(L"(Real Main Method)\n", i, i);
	wprintf(L"\tStart: 0x%04X\n", 1);

	if(be32(HEAD.table1_count)){
		wprintf(L"\tLength: 0x%04X\n\n", be32(methodTable[0].offset)-1);

		wprintf(L"%*sfunction entryPoint(){\n", 5, "");
		parseByteCodeToCode(&FDS[1], be32(methodTable[0].offset)-1, 1, 10);
		wprintf(L"%*s}\n\n", 5, "");

		parseByteCode(&FDS[1], be32(methodTable[0].offset)-1, 1);
	}else{
		wprintf(L"\tLength: 0x%04X\n\n", be32(HEAD.fds_size)-1);

		wprintf(L"%*sfunction entryPoint(){\n", 5, "");
		parseByteCodeToCode(&FDS[1], be32(HEAD.fds_size)-1, 1, 10);
		wprintf(L"%*s}\n\n", 5, "");

		parseByteCode(&FDS[1], be32(HEAD.fds_size)-1, 1);
	}

	for(i=0; i<be32(HEAD.table1_count);i++){
		wprintf(L"    (0x%02X)\n", i, i);
		wprintf(L"\tStart: 0x%04X\n", be32(methodTable[i].offset));
		if(i+1 == be32(HEAD.table1_count)){
			bytelength = be32(HEAD.fds_size) - be32(methodTable[i].offset);
		}else{
			bytelength = be32(methodTable[i+1].offset) - be32(methodTable[i].offset);
		}
		wprintf(L"\tLength: 0x%04X\n", bytelength);
		wprintf(L"\tMethod ID: 0x%02X (%s)\n", be16(methodTable[i].table4_method), exportedMethods[be16(methodTable[i].table4_method)]);
		wprintf(L"\tParam Count: %d\n",methodTable[i].param_count);
		wprintf(L"\tTemp Count: 0x%02X\n\n", methodTable[i].temp_count);

		wprintf(L"%*sfunction %s(){\n", 5, "", exportedMethods[be16(methodTable[i].table4_method)]);
		parseByteCodeToCode(&FDS[be32(methodTable[i].offset)], bytelength, be32(methodTable[i].offset), 10);
		wprintf(L"%*s}\n\n", 5, "");

		parseByteCode(&FDS[be32(methodTable[i].offset)], bytelength, be32(methodTable[i].offset));
	}
	wprintf(L"\n");


	wprintf(L"Imported Methods/Variables (Table 2):\n");
	for(i=0; i<be32(HEAD.table2_count);i++){
		wprintf(L"(0x%02X):", i);
		if(importedMethods[i] != 0){
			wprintf(L" Length: %d (%s)", (int) strlen(importedMethods[i]), importedMethods[i]);
		}
		wprintf(L"\n");
	}
	wprintf(L"\n");


	wprintf(L"String Literals (Table 3):\n");

	for(i=0; i<be32(HEAD.table3_count);i++){
		wprintf(L"(0x%02X) Length: %d\n%ls\n", i, (int) wcslen(stringLiterals[i]), stringLiterals[i]);
	}
	wprintf(L"\n");


	wprintf(L"Exported Methods/Variables (Table 4):\n");
	for(i=0; i<be32(HEAD.table4_count);i++){
		wprintf(L"(0x%02X) ", i);
		wprintf(L"Size = %02d.", (int) strlen(exportedMethods[i]));
		if(exportedMethods[i] != NULL){
			wprintf(L" (%s)", exportedMethods[i]);
		}
		wprintf(L"\n");
	}
	wprintf(L"\n");

	//Table 5: Line Starts
	for(i=0; i<blockCount;i++){
		wprintf(L"%d)\n", i);
		wprintf(L"\tOffset: 0x%02X\n", be32(byteBlocks[i].offset));
		wprintf(L"\tBytes: 0x %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n", byteBlocks[i].data[0], byteBlocks[i].data[1], byteBlocks[i].data[2], byteBlocks[i].data[3], byteBlocks[i].data[4], byteBlocks[i].data[5], byteBlocks[i].data[6], byteBlocks[i].data[7], byteBlocks[i].data[8], byteBlocks[i].data[9], byteBlocks[i].data[10], byteBlocks[i].data[11], byteBlocks[i].data[12], byteBlocks[i].data[13], byteBlocks[i].data[14], byteBlocks[i].data[15]);
	}

	free(methodTable);

	free(FDS);

	fclose(fp);

	iconv_close(cd);
}
