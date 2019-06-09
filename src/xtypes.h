#ifndef _XTYPES_H
#define _XTYPES_H

typedef enum {
    XType_None,
    XType_LEXToken,
    XType_LEX,
    XType_LEXIter,
    XType_String,
    XType_StringIter,
    XType_Number,
    XType_FPNumber,
    XType_HashTableEntry,
    XType_HashTable,
    XType_VarSym
} XT_ObjType;

typedef enum {
    XLTT__ERR = -1,
    XLTT__ENDOFITER = 0,
    XLTT_NNUM,
    XLTT_IDENTIFIER,
    XLTT_CHAR,
    XLTT_UCHAR,
    XLTT_INT,
    XLTT_UINT,
    XLTT_LONG,
    XLTT_FLOAT,
    XLTT_DOUBLE,
    XLTT_COMMA,
    XLTT_LBRACKET,
    XLTT_RBRACKET
} XT_LEXTokenType;

typedef enum {
    XSYT_char,
    XSYT_uchar,
    XSYT_int,
    XSYT_uint,
    XSYT_long,
    XSYT_float,
    XSYT_double
} XT_SymType;

typedef long XT_LEXTokenListSize;
typedef long XT_LEXTokenValue;

typedef XT_LEXTokenValue XT_Number;
typedef double XT_FPNumber;

typedef long XT_StringSize;

typedef XT_Number XT_HTHashKey;
typedef XT_HTHashKey XT_HTSize;

#endif