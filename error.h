#ifndef CONCATEN_ERROR_H
#define CONCATEN_ERROR_H

typedef unsigned long ERROR;
#define EFMT "%lu"

static const ERROR SUCCESS = 0;

/*-------------------*
 | Core class errors |
 +-------------------*/

// CONCATEN_FAIL is for errors that can't be categorized better
// (also for errors that occur in my code, as opposed to being reported
// from libraries in use by it)
//extern const ERROR CONCATEN_FAIL; // 0
//extern const ERROR CODE_BLOCK_FAIL; // 1000
//extern const ERROR DATA_STACK_FAIL; // 2000
//extern const ERROR TOKEN_STACK_FAIL; // 3000
//extern const ERROR SCOPE_STACK_FAIL; // 4000
//extern const ERROR OBJECT_FAIL; // 5000
//extern const ERROR PROP_MAP_FAIL; // 5500
//extern const ERROR METHOD_MAP_FAIL; // 5750
//extern const ERROR RUNNABLE_FAIL; // 6000

//extern const ERROR TOKENIZER_FAIL;               // 7000
//extern const ERROR  CTOR_FAIL;                   //  7100
//extern const ERROR   CTOR_STR_FAIL;              //   7110
  extern const ERROR    CTOR_STR_MALLOC_FAIL;      //    7111
  extern const ERROR    CTOR_STR_BAD_STRLEN_FAIL;  //    7112
  extern const ERROR    CTOR_STR_NULL_ARG_FAIL;    //    7113
//extern const ERROR   CTOR_FILE_FAIL;             //   7120
  extern const ERROR    CTOR_FILE_MALLOC_FAIL;     //    7121
  extern const ERROR    CTOR_FILE_BAD_STRLEN_FAIL; //    7122
  extern const ERROR    CTOR_FILE_NULL_ARG_FAIL;   //    7123
  extern const ERROR    CTOR_FILE_FOPEN_FAIL;      //    7124
//extern const ERROR  READ_CHAR_FAIL;              //  7200
  extern const ERROR   FILE_READ_FAIL;             //   7210
  extern const ERROR    FILE_READ_EOF_FAIL;        //    7211
//extern const ERROR   STRING_READ_FAIL;           //   7220
  extern const ERROR    STRING_READ_EOS_FAIL;      //    7221
//extern const ERROR  NEXT_TOKEN_FAIL;             //  7300
  extern const ERROR   NT_MALLOC_FAIL;             //   7301
  extern const ERROR   NT_NEW_SB_FAIL;             //   7302
  extern const ERROR   NT_SB_FREE_COPY_FAIL;       //   7303
  extern const ERROR   NT_INPUT_END_FAIL;          //   7304
//extern const ERROR TOKENIZER_SYNTAX_FAIL;        // 7500
  extern const ERROR  SYN_NO_SEPARATION_FAIL;      //  7501
  extern const ERROR  SYN_UNEXPECTED_END_FAIL;     //  7502
//extern const ERROR  SYN_STR_FAIL;                //  7510
  extern const ERROR   SYN_STR_MULTILINE_FAIL;     //   7511
//extern const ERROR  SYN_NUM_FAIL;                //  7520
  extern const ERROR   SYN_NUM_ILLEGAL_DIGIT_FAIL; //   7521
//extern const ERROR  SYN_RGX_FAIL;                //  7530
  extern const ERROR   SYN_RGX_BAD_FLAG_FAIL;      //   7531

#endif //CONCATEN_ERROR_H
