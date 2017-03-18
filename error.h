#ifndef CONCATEN_ERROR_H
#define CONCATEN_ERROR_H

typedef unsigned long ERROR;
#define EFMT "%lu"

static const ERROR SUCCESS = 0;

//extern const ERROR TOKENIZER_OPS_FAIL;         // 1000
//extern const ERROR  CTOR_FAIL;                 // 1100
//extern const ERROR   CTOR_STR_FAIL;            // 1110
extern const ERROR    CTOR_STR_MALLOC_FAIL;      // 1111
extern const ERROR    CTOR_STR_BAD_STRLEN_FAIL;  // 1112
extern const ERROR    CTOR_STR_NULL_ARG_FAIL;    // 1113
//extern const ERROR   CTOR_FILE_FAIL;           // 1120
extern const ERROR    CTOR_FILE_MALLOC_FAIL;     // 1121
extern const ERROR    CTOR_FILE_BAD_STRLEN_FAIL; // 1122
extern const ERROR    CTOR_FILE_NULL_ARG_FAIL;   // 1123
extern const ERROR    CTOR_FILE_FOPEN_FAIL;      // 1124
//extern const ERROR  READ_CHAR_FAIL;            // 1200
extern const ERROR   FILE_READ_FAIL;             // 1210
extern const ERROR    FILE_READ_EOF_FAIL;        // 1211
//extern const ERROR   STRING_READ_FAIL;         // 1220
extern const ERROR    STRING_READ_EOS_FAIL;      // 1221
//extern const ERROR  NEXT_TOKEN_FAIL;           // 1300
extern const ERROR   NT_MALLOC_FAIL;             // 1301
extern const ERROR   NT_NEW_SB_FAIL;             // 1302
extern const ERROR   NT_SB_FREE_COPY_FAIL;       // 1303
//extern const ERROR TOKENIZER_SYNTAX_FAIL;      // 1500
extern const ERROR  SYN_NO_SEPARATION_FAIL;      // 1501
extern const ERROR  SYN_UNEXPECTED_END_FAIL;     // 1502
extern const ERROR  SYN_EOI_FAIL;                // 1503
//extern const ERROR  SYN_STR_FAIL;              // 1510
extern const ERROR   SYN_STR_MULTILINE_FAIL;     // 1511
//extern const ERROR  SYN_NUM_FAIL;              // 1520
extern const ERROR   SYN_NUM_ILLEGAL_DIGIT_FAIL; // 1521
//extern const ERROR  SYN_RGX_FAIL;              // 1530
extern const ERROR   SYN_RGX_BAD_FLAG_FAIL;      // 1531

#endif //CONCATEN_ERROR_H
