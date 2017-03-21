#ifndef CONCATEN_ERROR_H
#define CONCATEN_ERROR_H

typedef unsigned long ERROR;
#define EFMT "%lu"

static const ERROR NO_ERROR = 0;

/*-------------------*
 | Core class errors |
 +-------------------*/

// CONCATEN_FAIL is for errors that can't be categorized better
// (also for errors that occur in my code, as opposed to being reported
// from libraries in use by it)
// TODO Contract error ranges to minimum sensible w/ enough room
//extern const ERROR CONCATEN_FAIL;                      // 0
//extern const ERROR  CODE_BLOCK_FAIL;                   //  1000
  extern const ERROR  CB_CTOR_MALLOC_FAIL;               //   1001;
  extern const ERROR  CB_COPY_MALLOC_FAIL;               //   1002;
  extern const ERROR  CB_APND_EXPAND_FAIL;               //   1003;
//extern const ERROR  CB_INSERT_FAIL;                    //   1010;
  extern const ERROR   CB_INST_BAD_IDX_FAIL;             //    1011;
  extern const ERROR   CB_INST_EXPAND_FAIL;              //    1012;
//extern const ERROR  CB_PREPEND_FAIL;                   //   1020;
  extern const ERROR   CB_PREP_EXPAND_FAIL;              //    1022;
//extern const ERROR  CB_REMOVE_FAIL;                    //   1020;
  extern const ERROR   CB_RMV_BAD_IDX_FAIL;              //    1031;
//extern const ERROR  CB_GET_FAIL;                       //   1040;
  extern const ERROR   CB_GET_BAD_IDX_FAIL;              //    1041;
//extern const ERROR  CB_SET_FAIL;                       //   1050;
  extern const ERROR   CB_SET_BAD_IDX_FAIL;              //    1051;
//extern const ERROR  DATA_STACK_FAIL;                   //  2000
  extern const ERROR   DST_PUSH_MALLOC_FAIL;             //   2001
  extern const ERROR   DST_POP_EMPTY_FAIL;               //   2002
//extern const ERROR  TOKEN_STACK_FAIL;                  //  3000
//extern const ERROR  SCOPE_STACK_FAIL;                  //  4000
//extern const ERROR  OBJECT_FAIL;                       //  5000
  extern const ERROR CTNO_CTOR_MALLOC_FAIL;              //   5001
  extern const ERROR CTNO_COPY_PROPS_FAIL;               //   5002
  extern const ERROR CTNO_COPY_DATA_FAIL;                //   5003
  extern const ERROR CTNO_GET_NO_KEY_FAIL;               //   5004
  extern const ERROR CTNO_GET_LITERAL_FAIL;              //   5005
  extern const ERROR CTNO_SET_LITERAL_FAIL;              //   5006
  extern const ERROR CTNO_SET_CYCLE_FAIL;                //   5007
  extern const ERROR CTNO_SET_ADD_FAIL;                  //   5008
  extern const ERROR CTNO_SET_CLAIM_FAIL;                //   5009
  extern const ERROR CTNO_CLAIM_MAX_REFCOUNT;            //   5010
//extern const ERROR  PROP_MAP_FAIL;                     //  5500
//extern const ERROR  METHOD_MAP_FAIL                    //  5750
  extern const ERROR   MM_CTOR_MALLOC_FAIL;              //   5751
  extern const ERROR   MM_NESTED_REHASH_FAIL;            //   5752
//extern const ERROR   MM_REHASH_FAIL;                   //   5760
  extern const ERROR    MM_RH_CREATE_FAIL;               //    5761
  extern const ERROR    MM_RH_BAD_SIZE_FAIL;             //    5762
//extern const ERROR   MM_SET_FAIL;                      //   5770
  extern const ERROR    MM_SET_BAD_KEY_FAIL;             //    5771
  extern const ERROR    MM_SET_REHASH_FAIL;              //    5772
//extern const ERROR   MM_GET_FAIL;                      //   5780
  extern const ERROR    MM_GET_NO_KEY_FAIL;              //    5781
//extern const ERROR   MM_REMOVE_FAIL;                   //   5790
  extern const ERROR    MM_RMV_NO_KEY_FAIL;              //    5791
//extern const ERROR  RUNNABLE_FAIL;                     //  6000
//extern const ERROR  STRINGBUILDER_FAIL;                //  7000
//extern const ERROR   SB_CTOR_FAIL;                     //   7010
  extern const ERROR    SB_CTOR_BAD_CAP_FAIL;            //    7011
  extern const ERROR    SB_CTOR_MALLOC_FAIL;             //    7012
//extern const ERROR   SB_APPEND_FAIL;                   //   7020
  extern const ERROR    SB_APND_MULT_OVERFLOW_FAIL;      //    7021
  extern const ERROR    SB_APND_MALLOC_FAIL;             //    7022
//extern const ERROR   SB_TO_STRING_FAIL;                //   7030
  extern const ERROR    SB_TS_MALLOC_FAIL;               //    7031
//extern const ERROR  TOKENIZER_FAIL;                    //  8000
//extern const ERROR   TKNR_CTOR_FAIL;                   //   8100
//extern const ERROR    TKNR_CTOR_STR_FAIL;              //    8110
  extern const ERROR     TKNR_CTOR_STR_MALLOC_FAIL;      //     8111
  extern const ERROR     TKNR_CTOR_STR_BAD_STRLEN_FAIL;  //     8112
  extern const ERROR     TKNR_CTOR_STR_NULL_ARG_FAIL;    //     8113
//extern const ERROR    TKNR_CTOR_FILE_FAIL;             //    8120
  extern const ERROR     TKNR_CTOR_FILE_MALLOC_FAIL;     //     8121
  extern const ERROR     TKNR_CTOR_FILE_BAD_STRLEN_FAIL; //     8122
  extern const ERROR     TKNR_CTOR_FILE_NULL_ARG_FAIL;   //     8123
  extern const ERROR     TKNR_CTOR_FILE_FOPEN_FAIL;      //     8124
//extern const ERROR   READ_CHAR_FAIL;                   //   8200
  extern const ERROR    TKNR_FILE_READ_FAIL;             //    8210
  extern const ERROR     TKNR_FILE_READ_EOF_FAIL;        //     8211
//extern const ERROR    TKNR_STRING_READ_FAIL;           //    8220
  extern const ERROR     TKNR_STRING_READ_EOS_FAIL;      //     8221
//extern const ERROR   TKNR_NEXT_TOKEN_FAIL;             //   8300
  extern const ERROR    TKNR_NT_MALLOC_FAIL;             //    8301
  extern const ERROR    TKNR_NT_NEW_SB_FAIL;             //    8302
  extern const ERROR    TKNR_NT_SB_FREE_COPY_FAIL;       //    8303
  extern const ERROR    TKNR_NT_INPUT_END_FAIL;          //    8304
//extern const ERROR  TKNR_SYNTAX_FAIL;                  //  8500
  extern const ERROR   TKNR_SYN_NO_SEPARATION_FAIL;      //   8501
  extern const ERROR   TKNR_SYN_UNEXPECTED_END_FAIL;     //   8502
//extern const ERROR   TKNR_SYN_STR_FAIL;                //   8510
  extern const ERROR    TKNR_SYN_STR_MULTILINE_FAIL;     //    8511
//extern const ERROR   TKNR_SYN_NUM_FAIL;                //   8520
  extern const ERROR    TKNR_SYN_NUM_ILLEGAL_DIGIT_FAIL; //    8521
//extern const ERROR   TKNR_SYN_RGX_FAIL;                //   8530
  extern const ERROR    TKNR_SYN_RGX_BAD_FLAG_FAIL;      //    8531

#endif //CONCATEN_ERROR_H
