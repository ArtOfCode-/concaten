#ifndef CONCATEN_ERROR_H
#define CONCATEN_ERROR_H

typedef unsigned long ERROR;
#define EFMT "%lu"

static const ERROR NO_ERROR = 0;

/*-------------------*
 | Core class errors |
 +-------------------*/

// TODO Contract error ranges to minimum sensible w/ enough room
// General errors (0-999)
extern const ERROR USER_DEFINED_ERROR_TYPE;

// CodeBlock error codes (1000-1999)
extern const ERROR CB_CTOR_MALLOC_FAIL;              // 1001
extern const ERROR CB_COPY_MALLOC_FAIL;              // 1002
extern const ERROR CB_APND_EXPAND_FAIL;              // 1003
extern const ERROR CB_INST_BAD_IDX_FAIL;             // 1011
extern const ERROR CB_INST_EXPAND_FAIL;              // 1012
extern const ERROR CB_PREP_EXPAND_FAIL;              // 1022
extern const ERROR CB_RMV_BAD_IDX_FAIL;              // 1031
extern const ERROR CB_GET_BAD_IDX_FAIL;              // 1041
extern const ERROR CB_SET_BAD_IDX_FAIL;              // 1051

// DataStack error codes (2000-2999)
extern const ERROR DST_PUSH_MALLOC_FAIL;             // 2001
extern const ERROR DST_POP_EMPTY_FAIL;               // 2002

// TokenStack error codes (3000-3999)
extern const ERROR TST_PUSH_MALLOC_FAIL;             // 3001
extern const ERROR TST_PUSH_SAVE_FAIL;               // 3002
extern const ERROR TST_POP_EMPTY_FAIL;          // 3003
extern const ERROR TST_POP_SAVE_FAIL;                // 3004
extern const ERROR TST_PEEK_EMPTY_FAIL;              // 3005
extern const ERROR TST_PUSH_LEVEL_MALLOC_FAIL;       // 3006
extern const ERROR TST_PUSH_LEVEL_SAVE_FAIL;         // 3007
extern const ERROR TST_POP_LEVEL_EMPTY_FAIL;         // 3008
extern const ERROR TST_POP_LEVEL_SAVE_FAIL;          // 3009
extern const ERROR TST_RSTR_NOT_SAVING_FAIL;         // 3010
extern const ERROR TST_RSTR_POP_FAIL;                // 3011
extern const ERROR TST_RSTR_PUSH_FAIL;               // 3012
extern const ERROR TST_RSTR_POP_LEVEL_FAIL;          // 3013
extern const ERROR TST_RSTR_PUSH_LEVEL_FAIL;         // 3014

// ScopeStack error codes (4000-4999)
// TODO ScopeStack errors

// Object error codes (5000-5499)
extern const ERROR CTNO_CTOR_MALLOC_FAIL;            // 5001
extern const ERROR CTNO_COPY_PROPS_FAIL;             // 5002
extern const ERROR CTNO_COPY_DATA_FAIL;              // 5003
extern const ERROR CTNO_GET_NO_KEY_FAIL;             // 5004
extern const ERROR CTNO_GET_LITERAL_FAIL;            // 5005
extern const ERROR CTNO_SET_LITERAL_FAIL;            // 5006
extern const ERROR CTNO_SET_CYCLE_FAIL;              // 5007
extern const ERROR CTNO_SET_ADD_FAIL;                // 5008
extern const ERROR CTNO_SET_CLAIM_FAIL;              // 5009
extern const ERROR CTNO_CLAIM_MAX_REFCOUNT;          // 5010

// Prop map error codes (5500-5749)
extern const ERROR PM_CTOR_MALLOC_FAIL;              // 5501
extern const ERROR PM_COPY_MALLOC_FAIL;              // 5502
extern const ERROR PM_NESTED_REHASH_FAIL;            // 5503
extern const ERROR PM_RH_CREATE_FAIL;                // 5504
extern const ERROR PM_RH_BAD_SIZE_FAIL;              // 5505
extern const ERROR PM_SET_REHASH_FAIL;               // 5508
extern const ERROR PM_GET_NO_KEY_FAIL;               // 5509
extern const ERROR PM_RMV_NO_KEY_FAIL;               // 5510

// Method map error codes (5750-5999)
extern const ERROR MM_CTOR_MALLOC_FAIL;              // 5751
extern const ERROR MM_NESTED_REHASH_FAIL;            // 5752
extern const ERROR MM_RH_CREATE_FAIL;                // 5761
extern const ERROR MM_RH_BAD_SIZE_FAIL;              // 5762
extern const ERROR MM_SET_BAD_KEY_FAIL;              // 5771
extern const ERROR MM_SET_REHASH_FAIL;               // 5772
extern const ERROR MM_GET_NO_KEY_FAIL;               // 5781
extern const ERROR MM_RMV_NO_KEY_FAIL;               // 5791

// Runnable error codes (6000-6999)
// TODO runnable errors

// StringBuilder error codes (7000-7999)
extern const ERROR SB_CTOR_BAD_CAP_FAIL;             // 7011
extern const ERROR SB_CTOR_MALLOC_FAIL;              // 7012
extern const ERROR SB_APND_MULT_OVERFLOW_FAIL;       // 7021
extern const ERROR SB_APND_MALLOC_FAIL;              // 7022
extern const ERROR SB_TS_MALLOC_FAIL;                // 7031

// Tokenizer error codes (8000-8999)
extern const ERROR TKNR_CTOR_STR_MALLOC_FAIL;        // 8111
extern const ERROR TKNR_CTOR_STR_BAD_STRLEN_FAIL;    // 8112
extern const ERROR TKNR_CTOR_STR_NULL_ARG_FAIL;      // 8113
extern const ERROR TKNR_CTOR_FILE_MALLOC_FAIL;       // 8121
extern const ERROR TKNR_CTOR_FILE_BAD_STRLEN_FAIL;   // 8122
extern const ERROR TKNR_CTOR_FILE_NULL_ARG_FAIL;     // 8123
extern const ERROR TKNR_CTOR_FILE_FOPEN_FAIL;        // 8124
extern const ERROR TKNR_FILE_READ_FAIL;              // 8210
extern const ERROR TKNR_FILE_READ_EOF_FAIL;          // 8211
extern const ERROR TKNR_STRING_READ_EOS_FAIL;        // 8221
extern const ERROR TKNR_NT_MALLOC_FAIL;              // 8301
extern const ERROR TKNR_NT_NEW_SB_FAIL;              // 8302
extern const ERROR TKNR_NT_SB_FREE_COPY_FAIL;        // 8303
extern const ERROR TKNR_NT_INPUT_END_FAIL;           // 8304
extern const ERROR TKNR_SYN_NO_SEPARATION_FAIL;      // 8501
extern const ERROR TKNR_SYN_UNEXPECTED_END_FAIL;     // 8502
extern const ERROR TKNR_SYN_STR_MULTILINE_FAIL;      // 8511
extern const ERROR TKNR_SYN_NUM_ILLEGAL_DIGIT_FAIL;  // 8521
extern const ERROR TKNR_SYN_RGX_BAD_FLAG_FAIL;       // 8531

#endif //CONCATEN_ERROR_H
