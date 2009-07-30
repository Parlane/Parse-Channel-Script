#ifndef CHANS_H
#define CHANS_H

#include "util.h"

typedef struct {
	//START: Main Header
	u32     head; // 0x00000000
	u32     version; // 0x00000004
	u32     file_size; // 0x00000008
	u8      unk[0x14]; // 0x0000000C
	//END: Main Header

	// All offsets are from 0x20
	u32     field_0; // 0x00000020
	u32     field_4; // 0x00000024
	u32     field_8; // 0x00000028

	// First data section (this is the bytecode)
	u32     fds_size;  // 0x0000002C
	u32     fds_offset; // 0x00000030

	u32     table4_count; // 0x00000034

	u32     field_18; // 0x00000038
	u32     field_1C; // 0x0000003C

	// Table 1 is made up of 8 byte chunks
	u32     table1_count; // 0x00000040
	u32     table1_offset; // 0x00000044

	// Table 2 is the method name list
	u32     table2_count; // 0x00000048
	u32     table2_offset; // 0x0000004C

	// Table 3 is a string literal table.
	// Each string is in utf-16, preceded by a u16 which specifies BYTE length of utf-18 string
	u32     table3_count; // 0x00000050
	u32     table3_offset; // 0x00000054

	u32     field_38; // 0x00000058
	u32     field_3C; // 0x0000005C

	u32     table4_offset; // 0x00000060

	u32     table5_offset; // 0x00000064

	u32 	field_48; // 0x00000068
	u32		field_4C;  // 0x0000006C
	u8 		field_50[0x10]; // 0x00000070

} bacs_header;

typedef struct {
	u32 offset;
	u16 table4_method;
	u8 param_count;
	u8 temp_count;
} table1_chunk;

typedef struct {
	u8 length;
	u8 padding;
	u16 offset;
} table2_key;


#endif
