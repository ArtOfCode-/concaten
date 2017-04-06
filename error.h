#ifndef CONCATEN_ERROR_H
#define CONCATEN_ERROR_H

typedef unsigned int ERROR;
#define EFMT "%u"

static const ERROR NO_ERROR = 0;

/*-------------------*
 | Core class errors |
 +-------------------*/

// General errors (0-999)
extern const ERROR USER_DEFINED_ERROR_TYPE;

static const ERROR CB_CTOR_MALLOC_FAIL                  = 101;
static const ERROR CB_COPY_MALLOC_FAIL                  = 102;
static const ERROR CB_APND_EXPAND_FAIL                  = 103;
static const ERROR CB_INST_BAD_IDX_FAIL                 = 111;
static const ERROR CB_INST_EXPAND_FAIL                  = 112;
static const ERROR CB_PREP_EXPAND_FAIL                  = 122;
static const ERROR CB_RMV_BAD_IDX_FAIL                  = 131;
static const ERROR CB_GET_BAD_IDX_FAIL                  = 141;
static const ERROR CB_SET_BAD_IDX_FAIL                  = 151;
static const ERROR DST_PUSH_MALLOC_FAIL                 = 201;
static const ERROR DST_POP_EMPTY_FAIL                   = 202;
static const ERROR TST_PUSH_MALLOC_FAIL                 = 301;
static const ERROR TST_PUSH_SAVE_FAIL                   = 302;
static const ERROR TST_POP_EMPTY_FAIL                   = 303;
static const ERROR TST_POP_SAVE_FAIL                    = 304;
static const ERROR TST_PEEK_EMPTY_FAIL                  = 305;
static const ERROR TST_PUSH_LEVEL_MALLOC_FAIL           = 306;
static const ERROR TST_PUSH_LEVEL_SAVE_FAIL             = 307;
static const ERROR TST_POP_LEVEL_EMPTY_FAIL             = 308;
static const ERROR TST_POP_LEVEL_SAVE_FAIL              = 309;
static const ERROR TST_RSTR_NOT_SAVING_FAIL             = 310;
static const ERROR TST_RSTR_POP_FAIL                    = 311;
static const ERROR TST_RSTR_PUSH_FAIL                   = 312;
static const ERROR TST_RSTR_POP_LEVEL_FAIL              = 313;
static const ERROR TST_RSTR_PUSH_LEVEL_FAIL             = 314;
static const ERROR TST_PEEK_MALLOC_FAIL                 = 315;
static const ERROR CTNO_CTOR_MALLOC_FAIL                = 501;
static const ERROR CTNO_CTOR_PM_COPY_FAIL               = 502;
static const ERROR CTNO_COPY_PROPS_FAIL                 = 503;
static const ERROR CTNO_COPY_DATA_FAIL                  = 504;
static const ERROR CTNO_COPY_CLAIM_FAIL                 = 505;
static const ERROR CTNO_GET_NO_KEY_FAIL                 = 506;
static const ERROR CTNO_GET_LITERAL_FAIL                = 507;
static const ERROR CTNO_GET_PM_GET_FAIL                 = 508;
static const ERROR CTNO_SET_LITERAL_FAIL                = 509;
static const ERROR CTNO_SET_CYCLE_FAIL                  = 510;
static const ERROR CTNO_SET_ADD_FAIL                    = 511;
static const ERROR CTNO_SET_CLAIM_FAIL                  = 512;
static const ERROR CTNO_SET_PM_GET_FAIL                 = 513;
static const ERROR CTNO_CLAIM_MAX_REFCOUNT              = 514;
static const ERROR PM_CTOR_MALLOC_FAIL                  = 551;
static const ERROR PM_COPY_CREATE_FAIL                  = 552;
static const ERROR PM_COPY_MALLOC_FAIL                  = 553;
static const ERROR PM_NESTED_REHASH_FAIL                = 554;
static const ERROR PM_RH_CREATE_FAIL                    = 555;
static const ERROR PM_RH_BAD_SIZE_FAIL                  = 556;
static const ERROR PM_SET_REHASH_FAIL                   = 557;
static const ERROR PM_SET_MALLOC_FAIL                   = 558;
static const ERROR PM_GET_NO_KEY_FAIL                   = 559;
static const ERROR PM_RMV_NO_KEY_FAIL                   = 560;
static const ERROR MM_CTOR_MALLOC_FAIL                  = 571;
static const ERROR MM_NESTED_REHASH_FAIL                = 572;
static const ERROR MM_RH_CREATE_FAIL                    = 573;
static const ERROR MM_RH_BAD_SIZE_FAIL                  = 574;
static const ERROR MM_SET_REHASH_FAIL                   = 575;
static const ERROR MM_GET_NO_KEY_FAIL                   = 576;
static const ERROR MM_RMV_NO_KEY_FAIL                   = 577;
static const ERROR SB_CTOR_BAD_CAP_FAIL                 = 711;
static const ERROR SB_CTOR_MALLOC_FAIL                  = 712;
static const ERROR SB_APND_MULT_OVERFLOW_FAIL           = 721;
static const ERROR SB_APND_MALLOC_FAIL                  = 722;
static const ERROR SB_TS_REALLOC_FAIL                   = 731;
static const ERROR TKNR_CTOR_STR_MALLOC_FAIL            = 801;
static const ERROR TKNR_CTOR_STR_BAD_STRLEN_FAIL        = 802;
static const ERROR TKNR_CTOR_STR_NULL_ARG_FAIL          = 803;
static const ERROR TKNR_CTOR_FILE_MALLOC_FAIL           = 804;
static const ERROR TKNR_CTOR_FILE_BAD_STRLEN_FAIL       = 805;
static const ERROR TKNR_CTOR_FILE_NULL_ARG_FAIL         = 806;
static const ERROR TKNR_CTOR_FILE_FOPEN_FAIL            = 807;
static const ERROR TKNR_FILE_READ_FAIL                  = 808;
static const ERROR TKNR_FILE_READ_EOF_FAIL              = 809;
static const ERROR TKNR_STRING_READ_EOS_FAIL            = 810;
static const ERROR TKNR_NT_MALLOC_FAIL                  = 811;
static const ERROR TKNR_NT_NEW_SB_FAIL                  = 812;
static const ERROR TKNR_NT_SB_FREE_COPY_FAIL            = 813;
static const ERROR TKNR_NT_INPUT_END_FAIL               = 814;
static const ERROR TKNR_SYN_NO_SEPARATION_FAIL          = 815;
static const ERROR TKNR_SYN_UNEXPECTED_END_FAIL         = 816;
static const ERROR TKNR_SYN_STR_MULTILINE_FAIL          = 817;
static const ERROR TKNR_SYN_NUM_ILLEGAL_DIGIT_FAIL      = 818;
static const ERROR TKNR_SYN_RGX_BAD_FLAG_FAIL           = 819;
static const ERROR TKNR_TKN_COPY_MALLOC_RAW_FAIL        = 820;
static const ERROR TKNR_TKN_COPY_MALLOC_ORG_FAIL        = 821;
static const ERROR TTO_WORDS_VALUELESS_FAIL             = 850;
static const ERROR TTO_UNKNOWN_TYPE_FAIL                = 851;
static const ERROR TTO_NOT_IMPLEMENTED_FAIL             = 852;
static const ERROR TTO_STRING_ESCAPE_FAIL               = 853;
static const ERROR TTO_ESCAPE_END_FAIL                  = 854;
static const ERROR TTO_MALLOC_FAIL                      = 855;
static const ERROR TTO_ESCAPE_BAD_HEX_FAIL              = 856;
static const ERROR TTO_OUT_OF_RANGE_FAIL                = 857;
static const ERROR TTO_INVALID_BASE_FAIL                = 858;
static const ERROR TTO_INVALID_DIGIT_FAIL               = 859;
static const ERROR TTO_FLP_CONVERT_FAIL                 = 860;
static const ERROR RN_NEW_CB_COPY_FAIL                  = 900;


#endif //CONCATEN_ERROR_H
