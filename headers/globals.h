/*
// Created by Raz Sagi on 29/07/2025.
*/

#ifndef GLOBALS_H
#define GLOBALS_H

#define MAX_LINE_LENGTH 80
#define LINE_BUFFER (MAX_LINE_LENGTH + 2)
#define MAX_LABEL_LENGTH 31
#define SYM_TABLE_MAX 1024
#define MAX_ENTRIES 512

#define PREPROC_OK 0
#define PREPROC_ERR_OPEN_IN 1
#define PREPROC_ERR_OPEN_OUT 2
#define PREPROC_ERR_OUT_MEMORY 3
#define PREPROC_ERR_LINE_TOO_LONG 4
#define PREPROC_ERR_MACRO_NAME_INVALID  5

#define SRC_LINE_MAX MAX_LINE_LENGTH
#define READ_BUF_SIZE 512
#define IC_INIT 100
#define DC_INIT 0
#define MAX_REGS 8
#define MAX_OPERANDS 2

#define IC_START 100
#define DC_START 0
#define MAX_DATA 256

#define OP_NONE -1
#define OP_IMM 0
#define OP_DIR 1
#define OP_IDX 2
#define OP_REG 3

#define BASE4_STR_MAX 32
#define FILE_EXT_OB ".ob"
#define FILE_EXT_ENT ".ent"
#define FILE_EXT_EXT ".ext"

#define WORD_BITS     10
#define OPCODE_SHIFT  6
#define SRC_SHIFT     4
#define DST_SHIFT     2
#define ARE_SHIFT     0

#define ARE_ABS 0
#define ARE_EXT 1
#define ARE_REL 2

#define PAYLOAD_BITS (WORD_BITS -2)
#define PAYLOAD_MASK ((1u << PAYLOAD_BITS) - 1u)
#define IMM_MIN (-(1L << (PAYLOAD_BITS - 1)))
#define IMM_MAX ((1L << (PAYLOAD_BITS - 1)) - 1L)



#define ADDR_ENC(x) ((unsigned)(x))









#endif
