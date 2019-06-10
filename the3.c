#include <stdlib.h>
#include <stdio.h>

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

typedef void (*XT_ObjectDestrucorFunPtr)(void *);

typedef struct _xtypeobject {
    XT_ObjType type;
    XT_ObjectDestrucorFunPtr destructor;
} XTypeObject;

#define XObject_HEAD XTypeObject ob_head;

typedef struct _xobject {
    XObject_HEAD
} XObject;

#define XObject_CAST(objptr) ((XObject *)(objptr))
#define XTypeObject_CAST(objptr) ((XTypeObject *)(objptr))

#define XObject_TYPE(objptr) (XObject_CAST(objptr)->ob_head.type)
#define XObject_DESTRUCTOR(objptr) (XObject_CAST(objptr)->ob_head.destructor)

XObject *XObject_Creat(void) {
    XObject *obj;

    obj = (XObject *) malloc(sizeof(XObject));

    if (obj == NULL)
        return NULL;

    obj->ob_head.type = XType_None;

    return obj;
}

void XObject_Forget(XObject *obj) {
    /*printf("\n[CALL] XObject_Forget");*/

    if (obj == NULL) {
        /*printf("(NULL)\n");*/
        return;
    }
        
    /*printf("(%p) (type: %d)\n", (void *)obj, obj->ob_head.type);*/

    if (XObject_TYPE(obj) != XType_None && XObject_DESTRUCTOR(obj) != NULL)
        XObject_DESTRUCTOR(obj)(obj);
}

#define XSTRING_EXTEND_SIZE (XT_StringSize)16
#define XSTRING_INITIAL_SIZE (XT_StringSize)16

#define XSTRING_ENDOFITER   '\0'

typedef struct _xstringobject {
    XObject_HEAD
    XT_StringSize size;
    XT_StringSize __last_indx;
    char *buf;
} XStringObject;

typedef struct _xstringiterobject {
    XObject_HEAD
    XT_StringSize iterindex;
    XStringObject *str;
} XStringIterObject;

#define XStringObject_CAST(objptr) ((XStringObject *)(objptr))
#define XStringIterObject_CAST(objptr) ((XStringIterObject *)(objptr))

#define XString_ScanLine(stream, str) XString_ScanUntilChar(stream, str, '\n')

void XString_Forget(void *str) {
    XStringObject *strobj;

    if (str == NULL)
        return;

    strobj = XStringObject_CAST(str);

    free(strobj->buf);
    free(str);
}

void XStringIter_Forget(void *iter) {
    if (iter == NULL)
        return;
        
    free(iter);
}

void _XString_FlushStringBuffer(XStringObject *str) {
    XT_StringSize indx;
    XStringObject *strobj;

    strobj = XStringObject_CAST(str);

    for (indx = 0; indx < strobj->size; indx++)
        strobj->buf[indx] = 0;
}

XObject *XString_Creat(void) {
    XStringObject *str;

    str = (XStringObject *) malloc(sizeof(XStringObject));

    if (str == NULL)
        return NULL;

    str->buf = (char *) malloc(XSTRING_INITIAL_SIZE * sizeof(char));

    if (str->buf == NULL)
        return NULL;

    XObject_TYPE(str) = XType_String;
    XObject_DESTRUCTOR(str) = XString_Forget;

    str->size = XSTRING_INITIAL_SIZE;
    str->__last_indx = 0;

    _XString_FlushStringBuffer(str);

    return XObject_CAST(str);
}

XT_StringSize _XString_Extend(XObject *str) {
    XStringObject *strobj;

    strobj = XStringObject_CAST(str);
    
    strobj->size += XSTRING_EXTEND_SIZE;
    strobj->buf = (char *) realloc(strobj->buf, sizeof(char) * strobj->size);

    return strobj->size;
}

XT_StringSize XString_PushChar(XObject *str, char ch) {
    XStringObject *strobj;

    strobj = XStringObject_CAST(str);

    if (strobj->size <= strobj->__last_indx)
            _XString_Extend(str);

    strobj->buf[strobj->__last_indx++] = ch;

    return strobj->size;
}

XT_StringSize XString_ScanUntilChar(FILE *stream, XObject *str, char stopchar) {
    char ch;
    XStringObject *strobj;

    strobj = XStringObject_CAST(str);
    strobj->__last_indx = 0;

    while ((ch = fgetc(stream)) != stopchar)
        XString_PushChar(str, ch);

    return strobj->__last_indx;
}

XObject *XString_GetIter(XObject *str) {
    XStringIterObject *iter;

    iter = (XStringIterObject *) malloc(sizeof(XStringIterObject));

    if (iter == NULL)
        return NULL;

    XObject_TYPE(iter) = XType_StringIter;
    XObject_DESTRUCTOR(iter) = XStringIter_Forget;

    iter->iterindex = 0;
    iter->str = XStringObject_CAST(str);

    return XObject_CAST(iter);
}

char XStringIter_IterNext(XObject *iter) {
    char ret;
    XStringIterObject *iterobj;

    iterobj = XStringIterObject_CAST(iter);

    if (iterobj->iterindex >= iterobj->str->__last_indx)
        return XSTRING_ENDOFITER;
    
    ret = iterobj->str->buf[iterobj->iterindex++];

    return ret;
}

int _is_number(char ch) {
    if (ch >= '0' && ch <= '9')
        return 1;

    return 0;
}

int _is_letter(char ch) {
    if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        return 1;

    return 0;
}

int _is_space(char ch) {
    if (ch == ' ' || ch == '\n' || ch == '\t')
        return 1;

    return 0;
}

XT_Number char_to_num(char ch) {
    return ch - '0';
}

int XString_CompareWith(XObject *str, char *cmpstr, XT_StringSize size) {
    XT_StringSize indx;
    XStringObject *strobj;

    strobj = XStringObject_CAST(str);

    if (strobj->__last_indx != size)
        return 0;

    for (indx = 0; indx < strobj->__last_indx && indx < size; indx++)
        if (strobj->buf[indx] != cmpstr[indx])
            return 0;

    return 1;
}

char *XString_GetString(XObject *str) {
    XT_StringSize indx;
    char *buf;

    buf = (char *) malloc(XStringObject_CAST(str)->__last_indx * sizeof(char) + 1);

    if (buf == NULL)
        return NULL;

    for (indx = 0; indx < XStringObject_CAST(str)->__last_indx; indx++)
        buf[indx] = XStringObject_CAST(str)->buf[indx];

    buf[indx] = '\0';

    return buf;
}

XObject *XString_Duplicate(XObject *str) {
    char ch;
    XObject *newstr, *striter;

    newstr = XString_Creat();
    striter = XString_GetIter(str);

    while ((ch = XStringIter_IterNext(striter)) != XSTRING_ENDOFITER)
        XString_PushChar(newstr, ch);

    XObject_Forget(striter);

    return newstr;
}

typedef struct _xnumber {
    XObject_HEAD
    XT_Number val;
} XNumberObject;

typedef struct _xfpnumber {
    XObject_HEAD
    XT_FPNumber val;
} XFPNumberObject;

#define XNumberObject_CAST(objptr) ((XNumberObject *)(objptr))
#define XFPNumberObject_CAST(objptr) ((XFPNumberObject *)(objptr))

void XNumber_Forget(void *num) {
    if (num == NULL)
        return;

    free(num);
}

void XFPNumber_Forget(void *fpnum) {
    if (fpnum == NULL)
        return;
        
    free(fpnum);
}

XObject *XNumber_Creat(XT_Number *valptr) {
    XNumberObject *num;

    num = (XNumberObject *) malloc(sizeof(XNumberObject));

    if (num == NULL)
        return NULL;

    XObject_TYPE(num) = XType_Number;
    XObject_DESTRUCTOR(num) = XNumber_Forget;

    num->val = *valptr;

    return XObject_CAST(num);
}

XObject *XNumber_FromString(XObject *str) {
    char ch;
    XT_Number nval;
    XObject *striter;

    striter = XString_GetIter(str);

    for (nval = 0; (ch = XStringIter_IterNext(striter)) != XSTRING_ENDOFITER; )
        nval = char_to_num(ch) + 10 * nval;

    XObject_Forget(striter);

    return XNumber_Creat(&nval);
}

XObject *XFPNumber_Creat(XT_FPNumber *valptr) {
    XFPNumberObject *fpnum;

    fpnum = (XFPNumberObject *) malloc(sizeof(XFPNumberObject));

    if (fpnum == NULL)
        return NULL;

    XObject_TYPE(fpnum) = XType_FPNumber;
    XObject_DESTRUCTOR(fpnum) = XFPNumber_Forget;

    fpnum->val = *valptr;

    return XObject_CAST(fpnum);
}

typedef struct _xlextokenobject {
    XObject_HEAD
    XT_LEXTokenType type;
    XObject *dataobject;
    struct _xlextokenobject *next;
    struct _xlextokenobject *prev;
} XLEXTokenObject;

typedef struct _xlexobject {
    XObject_HEAD
    XLEXTokenObject *head;
    XT_LEXTokenListSize size;
    XLEXTokenObject *__last;
} XLEXObject;

typedef struct _xlexiterobject {
    XObject_HEAD
    XLEXTokenObject *currtok;
    XT_LEXTokenListSize iterindex;
} XLEXIterObject;

#define XLEXObject_CAST(objptr) ((XLEXObject *)(objptr))
#define XLEXTokenObject_CAST(objptr) ((XLEXTokenObject *)(objptr))
#define XLEXIterObject_CAST(objptr) ((XLEXIterObject *)(objptr))

void XLEXToken_Forget(void *tok) {
    if (tok == NULL)
        return;
        
    if (XLEXTokenObject_CAST(tok)->dataobject != NULL)
        XObject_Forget(XLEXTokenObject_CAST(tok)->dataobject);

    free(tok);
}

void XLEX_Forget(void *lex) {
    XLEXTokenObject *tok, *nexttok;

    if (lex == NULL)
        return;

    for (tok = XLEXObject_CAST(lex)->head; tok != NULL; tok = nexttok) {
        if (tok->next != NULL)
            nexttok = tok->next;
        else
            nexttok = NULL;

        XObject_Forget(XObject_CAST(tok));
    }
        

    free(lex);
}

void XLEXIter_Forget(void *iter) {
    if (iter == NULL)
        return;
        
    free(iter);
}

XObject *XLEX_Creat(void) {
    XLEXObject *obj;

    obj = (XLEXObject *) malloc(sizeof(XLEXObject));

    if (obj == NULL)
        return NULL;

    XObject_TYPE(obj) = XType_LEX;
    XObject_DESTRUCTOR(obj) = XLEX_Forget;

    obj->head = NULL;
    obj->size = 0;
    obj->__last = NULL;

    return XObject_CAST(obj);
}

XObject *XLEXToken_Creat(XT_LEXTokenType ttype, XObject *dataobject) {
    XLEXTokenObject *tobj;

    tobj = (XLEXTokenObject *) malloc(sizeof(XLEXTokenObject));

    if (tobj == NULL)
        return NULL;

    XObject_TYPE(tobj) = XType_LEXToken;
    XObject_DESTRUCTOR(tobj) = XLEXToken_Forget;

    tobj->type = ttype;
    tobj->dataobject = dataobject;

    tobj->next = NULL;
    tobj->prev = NULL;    

    return XObject_CAST(tobj);
}

XT_LEXTokenType XLEXToken_TypeFromString(XObject *str) {
    int flag_num = 0, flag_char = 0;
    char ch;
    XObject *striter;

    if (XStringObject_CAST(str)->__last_indx == 0)
        return XLTT__ERR;

    striter = XString_GetIter(str);

    while ((ch = XStringIter_IterNext(striter)) != XSTRING_ENDOFITER) {
        if (_is_letter(ch))
            flag_char = 1;
        else if (_is_number(ch))
            flag_num = 1;

        if (flag_num && flag_char)
            break;
    }

    XObject_Forget(striter);

    if (flag_num && flag_char)
        return XLTT_IDENTIFIER;
    else if (flag_num && !flag_char)
        return XLTT_NNUM;
    else {
        if (XString_CompareWith(str, "char", 4))
            return XLTT_CHAR;
        else if (XString_CompareWith(str, "uchar", 5))
            return XLTT_UCHAR;
        else if (XString_CompareWith(str, "int", 3))
            return XLTT_INT;
        else if (XString_CompareWith(str, "uint", 4))
            return XLTT_UINT;
        else if (XString_CompareWith(str, "long", 4))
            return XLTT_LONG;
        else if (XString_CompareWith(str, "float", 5))
            return XLTT_FLOAT;
        else if (XString_CompareWith(str, "double", 6))
            return XLTT_DOUBLE;
        else 
            return XLTT_IDENTIFIER;
    }

    return XLTT__ERR;
}

XT_LEXTokenListSize XLEX_AppendNode(XObject *lex, XT_LEXTokenType ttype, XObject *dataobject) {
    XLEXObject *lexobj;
    XObject *tobj, *_tmpobj;

    lexobj = XLEXObject_CAST(lex);

    if (ttype == XLTT__ERR)
        return -1;

    if (ttype == XLTT_NNUM) {
        _tmpobj = dataobject;
        dataobject = XNumber_FromString(dataobject);
        XObject_Forget(_tmpobj);
    }

    tobj = XLEXToken_Creat(ttype, dataobject);

    if (tobj == NULL)
        return -1;

    if (lexobj->size == 0)
        lexobj->head = XLEXTokenObject_CAST(tobj);
    else {
        XLEXTokenObject_CAST(tobj)->prev = lexobj->__last;
        lexobj->__last->next = XLEXTokenObject_CAST(tobj);
    }

    lexobj->__last = XLEXTokenObject_CAST(tobj);
    lexobj->size++;

    return lexobj->size;
}

XObject *XLEX_GetIter(XObject *lex) {
    XLEXObject *lexobj;
    XLEXIterObject *iobj;

    lexobj = XLEXObject_CAST(lex);

    iobj = (XLEXIterObject *) malloc(sizeof(XLEXIterObject));

    if (iobj == NULL)
        return NULL;

    XObject_TYPE(iobj) = XType_LEXIter;
    XObject_DESTRUCTOR(iobj) = XLEXIter_Forget;

    iobj->currtok = lexobj->head;
    iobj->iterindex = 0;

    return XObject_CAST(iobj);
}

XObject *XLEXIter_IterNext(XObject *iter, int *statcode) {
    XLEXIterObject *iterobj;
    XLEXTokenObject *ret;

    iterobj = XLEXIterObject_CAST(iter);

    if (iterobj == NULL) {
        *statcode = XLTT__ERR;
        return NULL;
    }

    if (iterobj->currtok == NULL) {
        *statcode = XLTT__ENDOFITER;
        return NULL;
    }

    ret = iterobj->currtok;
    iterobj->currtok = ret->next;
    iterobj->iterindex++;

    return XObject_CAST(ret);
}

XObject *XLEX_LexString(XObject *str) {
    char ch;
    XObject *striter, *lex, *buffer;
    XT_LEXTokenType ttype;

    striter = XString_GetIter(str);
    lex = XLEX_Creat();

    buffer = XString_Creat();

    while ((ch = XStringIter_IterNext(striter)) != XSTRING_ENDOFITER) {
        if (_is_space(ch)) {
            if (XStringObject_CAST(buffer)->__last_indx > 0) {
                ttype = XLEXToken_TypeFromString(buffer);
                XLEX_AppendNode(lex, ttype, buffer);
                buffer = XString_Creat();
            } else
                continue;
        } else if (ch == ',') {
            if (XStringObject_CAST(buffer)->__last_indx > 0) {
                ttype = XLEXToken_TypeFromString(buffer);
                XLEX_AppendNode(lex, ttype, buffer);
                buffer = XString_Creat();
            }
            XLEX_AppendNode(lex, XLTT_COMMA, NULL);
        } else if (ch == '[') {
            if (XStringObject_CAST(buffer)->__last_indx > 0) {
                ttype = XLEXToken_TypeFromString(buffer);
                XLEX_AppendNode(lex, ttype, buffer);
                buffer = XString_Creat();
            }
            XLEX_AppendNode(lex, XLTT_LBRACKET, NULL);
        } else if (ch == ']') {
            if (XStringObject_CAST(buffer)->__last_indx > 0) {
                ttype = XLEXToken_TypeFromString(buffer);
                XLEX_AppendNode(lex, ttype, buffer);
                buffer = XString_Creat();
            }
            XLEX_AppendNode(lex, XLTT_RBRACKET, NULL);
        } else {
            XString_PushChar(buffer, ch);
        }

    }


    if (XStringObject_CAST(buffer)->__last_indx > 0) {
        ttype = XLEXToken_TypeFromString(buffer);
        XLEX_AppendNode(lex, ttype, buffer);
    }

    XObject_Forget(striter);

    return XObject_CAST(lex);
}

#define XHT_HASH_MULTIPLIER (XT_HTHashKey)61803

typedef XT_HTHashKey (*XT_HTHashFuncPtr)(XStringObject *, XT_HTSize);

typedef struct _xhtentry {
    XObject_HEAD
    XT_HTHashKey key;
    XObject *dataobject;
    struct _xhtentry *next;
    struct _xhtentry *prev;
} XHTEntryObject;

typedef struct _xhashtable {
    XObject_HEAD
    XT_HTSize size;
    XT_HTHashFuncPtr hashfn;
    XHTEntryObject **hashbucket;
} XHashTableObject;

#define XHTEntryObject_CAST(objptr) ((XHTEntryObject *)(objptr))
#define XHashTableObject_CAST(objptr) ((XHashTableObject *)(objptr))

void XHTEntry_Forget(void *ent) {
    if (ent == NULL)
        return;

    XObject_Forget(XHTEntryObject_CAST(ent)->dataobject);
    free(ent);
}

void XHashTable_Forget(void *ht) {
    XT_HTSize indx;
    XHTEntryObject *ent, *nextent;

    if (ht == NULL)
        return;

    for (indx = 0; indx < XHashTableObject_CAST(ht)->size; indx++) {
        for (ent = XHashTableObject_CAST(ht)->hashbucket[indx]; ent != NULL; ) {
            nextent = ent->next;
            XObject_Forget(XObject_CAST(ent));
            ent = nextent;
        }
    }

    free(XHashTableObject_CAST(ht)->hashbucket);
    free(ht);
}

XObject *XHTEntry_Creat(XObject *dataobject) {
    XHTEntryObject *ent;

    ent = (XHTEntryObject *) malloc(sizeof(XHTEntryObject));

    if (ent == NULL)
        return NULL;

    XObject_TYPE(ent) = XType_HashTableEntry;
    XObject_DESTRUCTOR(ent) = XHTEntry_Forget;

    ent->dataobject = dataobject;
    ent->key = -1;
    ent->next = NULL;
    ent->prev = NULL;

    return XObject_CAST(ent);
}

XObject *XHashTable_Creat(XT_HTSize size, XT_HTHashFuncPtr hashfn) {
    XT_HTSize indx;
    XHashTableObject *ht;
    XHTEntryObject **bucket;

    ht = (XHashTableObject *) malloc(sizeof(XHashTableObject));
    bucket = (XHTEntryObject**) malloc(size * sizeof(XHTEntryObject*));

    if (ht == NULL || bucket == NULL)
        return NULL;

    XObject_TYPE(ht) = XType_HashTable;
    XObject_DESTRUCTOR(ht) = XHashTable_Forget;

    ht->hashbucket = bucket;
    ht->hashfn = hashfn;
    ht->size = size;

    for (indx = 0; indx < ht->size; indx++)
        ht->hashbucket[indx] = NULL;

    return XObject_CAST(ht);
}

XT_HTHashKey XHashTable_DefaultHashFunc(XStringObject *str, XT_HTSize tablesize) {
    char ch;
    XT_StringSize indx;
    unsigned long charsum;
    XObject *striter;

    striter = XString_GetIter(XObject_CAST(str));

    for (indx = 0, charsum = 0; (ch = XStringIter_IterNext(striter)) != XSTRING_ENDOFITER && indx <= 5; indx++)
        charsum += indx * indx * char_to_num(ch);

    XObject_Forget(striter);

    return (XT_HTHashKey)(XHT_HASH_MULTIPLIER * charsum % tablesize);
}

XT_HTHashKey XHashTable_InsertObject(XObject *ht, XObject *dataobject, XStringObject *key) {
    XHashTableObject *htobj;
    XT_HTHashKey keyhash;
    XHTEntryObject *last_entry;
    XObject *new_entry;

    htobj = XHashTableObject_CAST(ht);
    keyhash = htobj->hashfn(key, htobj->size);

    new_entry = XHTEntry_Creat(dataobject);

    if (new_entry == NULL) 
        return -1;

    XHTEntryObject_CAST(new_entry)->key = keyhash;

    if (htobj->hashbucket[keyhash] == NULL)
        htobj->hashbucket[keyhash] = XHTEntryObject_CAST(new_entry);
    else {
        for (last_entry = htobj->hashbucket[keyhash]; last_entry->next != NULL; last_entry = last_entry->next);
        last_entry->next = XHTEntryObject_CAST(new_entry);
        XHTEntryObject_CAST(new_entry)->prev = last_entry;
    }

    return keyhash;
}

#define XSYMBOL_REPR_SIZE   64

struct __xvarsym_arrspec {
    int arr_dim;
    XObject *dim_ref[2];
};

typedef struct _xvarsym {
    XObject_HEAD
    XStringObject *name;
    XT_SymType type;
    XT_Number bytesize;
    XT_Number file_pos;
    struct __xvarsym_arrspec arrspec;
    XObject *varobj;
} XVarSymObject;

#define Arrspec_Dim(sp) (sp.arr_dim)
#define Arrspec_Ref(sp) (sp.dim_ref)

#define XVarSymObject_CAST(objptr) ((XVarSymObject *)(objptr))
#define XVarSymObject_ArrDim(objptr) (XVarSymObject_CAST(objptr)->arrspec.arr_dim)
#define XVarSymObject_DimRef(objptr) (XVarSymObject_CAST(objptr)->arrspec.dim_ref)

XT_Number __var_size(XT_SymType type) {
    switch (type) {
        case XSYT_char:
            return (XT_Number)sizeof(char);
        case XSYT_uchar:
            return (XT_Number)sizeof(unsigned char);
        case XSYT_int:
            return (XT_Number)sizeof(int);
        case XSYT_uint:
            return (XT_Number)sizeof(unsigned int);
        case XSYT_long:
            return (XT_Number)sizeof(long);
        case XSYT_float:
            return (XT_Number)sizeof(float);
        case XSYT_double:
            return (XT_Number)sizeof(double);
        default:
            return -1;
    }
}

void XVarSym_Forget(void *sym) {
    if (sym == NULL)
        return;

    XObject_Forget(XObject_CAST(XVarSymObject_CAST(sym)->name));
    XObject_Forget(XVarSymObject_CAST(sym)->varobj);

    free(sym);
}

XObject *XVarSymObject_Creat(XStringObject *name, XT_SymType type, XT_Number file_pos, struct __xvarsym_arrspec arrspec) {
    XVarSymObject *sym;

    sym = (XVarSymObject *) malloc(sizeof(XVarSymObject));

    if (sym == NULL)
        return NULL;

    XObject_TYPE(sym) = XType_VarSym;
    XObject_DESTRUCTOR(sym) = XVarSym_Forget;

    sym->name = name;
    sym->type = type;
    sym->file_pos = file_pos;
    sym->varobj = NULL;

    XVarSymObject_ArrDim(sym) = Arrspec_Dim(arrspec);
    XVarSymObject_DimRef(sym)[0] = Arrspec_Ref(arrspec)[0];
    XVarSymObject_DimRef(sym)[1] = Arrspec_Ref(arrspec)[1];

    sym->bytesize = __var_size(sym->type);

    return XObject_CAST(sym);
}

XObject *XSymbolTable_RetrieveObject(XObject *ht, XStringObject *key) {
    XHashTableObject *htobj;
    XT_HTHashKey keyhash;
    XHTEntryObject *htentry;

    htobj = XHashTableObject_CAST(ht);
    keyhash = htobj->hashfn(key, htobj->size);

    for (htentry = htobj->hashbucket[keyhash]; htentry != NULL; htentry = htentry->next)
        if (XString_CompareWith(XObject_CAST(XVarSymObject_CAST(htentry->dataobject)->name), key->buf, key->__last_indx))
            return htentry->dataobject;

    return NULL;
}

void XVarSym_AssignValueFromStream(XObject *sym, XObject *ht, FILE *stream) {
    long seek_store;
    XT_Number _ref_val = 0;
    XT_FPNumber _fpref_val = 0;
    XVarSymObject *symobj;
    XObject *refobj;

    symobj = XVarSymObject_CAST(sym);

    if (XVarSymObject_ArrDim(sym) == 0) {
        seek_store = ftell(stream);
        fseek(stream, symobj->file_pos, SEEK_SET);

        if (symobj->type == XSYT_double || symobj->type == XSYT_float) {
            fread(&_fpref_val, __var_size(symobj->type), 1, stream);
            symobj->varobj = XFPNumber_Creat(&_fpref_val);
        } else {
            fread(&_ref_val, __var_size(symobj->type), 1, stream);
            symobj->varobj = XNumber_Creat(&_ref_val);
        }
        
        fseek(stream, seek_store, SEEK_SET);
    } else {
        if (XVarSymObject_ArrDim(sym) >= 1) {
            if (XObject_TYPE(XVarSymObject_DimRef(sym)[0]) == XType_String) {
                refobj = XSymbolTable_RetrieveObject(ht, XStringObject_CAST(XVarSymObject_DimRef(sym)[0]));
                
                if (refobj == NULL) 
                    return;

                if (XVarSymObject_ArrDim(refobj) != 0) 
                    return;

                _ref_val = XNumberObject_CAST(XVarSymObject_CAST(refobj)->varobj)->val;
            } else
                _ref_val = XNumberObject_CAST(XVarSymObject_DimRef(sym)[0])->val;

            XVarSymObject_DimRef(sym)[0] = XNumber_Creat(&_ref_val);
        }

        if (XVarSymObject_ArrDim(sym) == 2) {
            if (XObject_TYPE(XVarSymObject_DimRef(sym)[1]) == XType_String) {
                refobj = XSymbolTable_RetrieveObject(ht, XStringObject_CAST(XVarSymObject_DimRef(sym)[1]));
                
                if (refobj == NULL)
                    return;

                if (XVarSymObject_ArrDim(refobj) != 0)
                    return;

                _ref_val = XNumberObject_CAST(XVarSymObject_CAST(refobj)->varobj)->val;
            } else
                _ref_val = XNumberObject_CAST(XVarSymObject_DimRef(sym)[1])->val;

            XVarSymObject_DimRef(sym)[1] = XNumber_Creat(&_ref_val);
        }
    }

    if (XVarSymObject_ArrDim(sym) == 1)
        symobj->bytesize = __var_size(symobj->type) * XNumberObject_CAST(XVarSymObject_DimRef(sym)[0])->val;

    if (XVarSymObject_ArrDim(sym) == 2)
        symobj->bytesize = __var_size(symobj->type) * XNumberObject_CAST(XVarSymObject_DimRef(sym)[0])->val * XNumberObject_CAST(XVarSymObject_DimRef(sym)[1])->val;
}

XObject *XSymbolTable_ConstructFromLEX(XObject *lex, FILE *datastream) {
    int lexiterstatcode = 0, tokindx = 0;
    XObject *ht, *symbol, *lexiter, *currtok, *tokarr[8], *symname;
    XT_HTSize tabsize;
    XT_Number filepos = 0;
    XT_SymType symtype;
    struct __xvarsym_arrspec arrspec;

    if (XLEXObject_CAST(lex)->size <= 50)
        tabsize = 29;
    else if (XLEXObject_CAST(lex)->size <= 120)
        tabsize = 97;
    else
        tabsize = 229;

    ht = XHashTable_Creat(tabsize, XHashTable_DefaultHashFunc);
    lexiter = XLEX_GetIter(lex);

    while ((currtok = XLEXIter_IterNext(lexiter, &lexiterstatcode)) != NULL || tokindx > 0) {
        if (currtok != NULL && XLEXTokenObject_CAST(currtok)->type != XLTT_COMMA)
            tokarr[tokindx++] = currtok;
        else {
            if (tokindx == 8) {
                Arrspec_Dim(arrspec) = 2;
                Arrspec_Ref(arrspec)[0] = XLEXTokenObject_CAST(tokarr[3])->dataobject;
                Arrspec_Ref(arrspec)[1] = XLEXTokenObject_CAST(tokarr[6])->dataobject;
            } else if (tokindx == 5) {
                Arrspec_Dim(arrspec) = 1;
                Arrspec_Ref(arrspec)[0] = XLEXTokenObject_CAST(tokarr[3])->dataobject;
                Arrspec_Ref(arrspec)[1] = NULL;
            } else {
                Arrspec_Dim(arrspec) = 0;
                Arrspec_Ref(arrspec)[0] = NULL;
                Arrspec_Ref(arrspec)[1] = NULL;
            }

            switch (XLEXTokenObject_CAST(tokarr[0])->type)
            {
                case XLTT_CHAR:
                    symtype = XSYT_char;
                    break;
                case XLTT_UCHAR:
                    symtype = XSYT_uchar;
                    break;
                case XLTT_INT:
                    symtype = XSYT_int;
                    break;
                case XLTT_UINT:
                    symtype = XSYT_uint;
                    break;
                case XLTT_LONG:
                    symtype = XSYT_long;
                    break;
                case XLTT_FLOAT:
                    symtype = XSYT_float;
                    break;
                case XLTT_DOUBLE:
                    symtype = XSYT_double;
                    break;
                default:
                    symtype = XSYT_int;
            }

            if (XObject_TYPE(XLEXTokenObject_CAST(tokarr[1])->dataobject) != XType_String)
                return NULL;

            symname = XString_Duplicate(XLEXTokenObject_CAST(tokarr[1])->dataobject);
            symbol = XVarSymObject_Creat(XStringObject_CAST(symname), symtype, filepos, arrspec);
            XVarSym_AssignValueFromStream(symbol, ht, datastream);
            XHashTable_InsertObject(ht, symbol, XStringObject_CAST(XLEXTokenObject_CAST(tokarr[1])->dataobject));

            tokarr[0] = NULL;
            tokarr[1] = NULL;
            tokarr[2] = NULL;
            tokarr[3] = NULL;
            tokarr[4] = NULL;
            tokarr[5] = NULL;
            tokarr[6] = NULL;
            tokarr[7] = NULL;
            
            tokindx = 0;

            filepos += XVarSymObject_CAST(symbol)->bytesize;
        }
    }

    XObject_Forget(lexiter);

    if (lexiterstatcode == XLTT__ERR)
        return NULL;

    return ht;
}

char *XVarSym_GetReprWithFileOffset(XObject *sym, long offset, FILE *stream) {
    char *repr, crval;
    unsigned char ucrval;
    int irval, _ctr;
    unsigned int uirval;
    long seek_store, lrval;
    float frval;
    double drval;

    XVarSymObject *symobj;

    symobj = XVarSymObject_CAST(sym);
    repr = (char *) malloc(XSYMBOL_REPR_SIZE * sizeof(char));
    
    for (_ctr = 0; _ctr < XSYMBOL_REPR_SIZE; _ctr++)
        repr[_ctr] = 0;

    seek_store = ftell(stream);
    fseek(stream, offset, SEEK_SET);

    switch (symobj->type) {
        case XSYT_char:
            fread(&crval, sizeof(char), 1, stream);
            snprintf(repr, XSYMBOL_REPR_SIZE, "%d", crval);
            break;
        case XSYT_uchar:
            fread(&ucrval, sizeof(unsigned char), 1, stream);
            snprintf(repr, XSYMBOL_REPR_SIZE, "%u", ucrval);
            break;
        case XSYT_int:
            fread(&irval, sizeof(int), 1, stream);
            snprintf(repr, XSYMBOL_REPR_SIZE, "%d", irval);
            break;
        case XSYT_uint:
            fread(&uirval, sizeof(unsigned int), 1, stream);
            snprintf(repr, XSYMBOL_REPR_SIZE, "%u", uirval);
            break;
        case XSYT_long:
            fread(&lrval, sizeof(long), 1, stream);
            snprintf(repr, XSYMBOL_REPR_SIZE, "%ld", lrval);
            break;
        case XSYT_float:
            fread(&frval, sizeof(float), 1, stream);
            snprintf(repr, XSYMBOL_REPR_SIZE, "%f", frval);
            break;
        case XSYT_double:
            fread(&drval, sizeof(double), 1, stream);
            snprintf(repr, XSYMBOL_REPR_SIZE, "%lf", drval);
            break;
        default:
            return NULL;
    }

    fseek(stream, seek_store, SEEK_SET);

    return repr;
}

int main(int argc, char **argv, char **envp) {
    XObject *str, *lex, *symtab;
    XObject *lexiter, *currtok, *symbol;
    XObject *dimref1, *dimref2;
    XLEXTokenObject *tokarr[6];
    XT_Number tokindx;

    int _statc = 0;
    char *fnam, *symrepr;
    long data_offset;
    FILE *datafile;

    setvbuf(stdout, NULL, 0, 0);

    str = XString_Creat();
    XString_ScanLine(stdin, str);

    fnam = XString_GetString(str);
    datafile = fopen(fnam, "r");

    XObject_Forget(str);
    free(fnam);

    str = XString_Creat();
    XString_ScanLine(stdin, str);

    lex = XLEX_LexString(str);

    XObject_Forget(str);

    symtab = XSymbolTable_ConstructFromLEX(lex, datafile);

    XObject_Forget(lex);

    str = XString_Creat();
    XString_ScanLine(stdin, str);
    
    lex = XLEX_LexString(str);

    XObject_Forget(str);

    lexiter = XLEX_GetIter(lex);
    tokindx = 0;

    while ((currtok = XLEXIter_IterNext(lexiter, &_statc)) != NULL || tokindx > 0) {    
        if (currtok != NULL && XLEXTokenObject_CAST(currtok)->type != XLTT_COMMA)
            tokarr[tokindx++] = XLEXTokenObject_CAST(currtok);
        else {
            symbol = XSymbolTable_RetrieveObject(symtab, XStringObject_CAST(tokarr[0]->dataobject));

            switch (XVarSymObject_ArrDim(symbol)) {
                case 0:
                    data_offset = XVarSymObject_CAST(symbol)->file_pos;
                    break;
                case 1:
                    data_offset = XVarSymObject_CAST(symbol)->file_pos;
                    dimref1 = tokarr[2]->dataobject;

                    if (XObject_TYPE(dimref1) != XType_Number)
                        dimref1 = XVarSymObject_CAST(XSymbolTable_RetrieveObject(symtab, XStringObject_CAST(dimref1)))->varobj;
                    
                    data_offset += __var_size(XVarSymObject_CAST(symbol)->type) * XNumberObject_CAST(dimref1)->val;
                    break;
                case 2:
                    data_offset = XVarSymObject_CAST(symbol)->file_pos;
                    dimref1 = tokarr[2]->dataobject;
                    dimref2 = tokarr[5]->dataobject;

                    if (XObject_TYPE(dimref1) != XType_Number)
                        dimref1 = XVarSymObject_CAST(XSymbolTable_RetrieveObject(symtab, XStringObject_CAST(dimref1)))->varobj;

                    if (XObject_TYPE(dimref2) != XType_Number)
                        dimref2 = XVarSymObject_CAST(XSymbolTable_RetrieveObject(symtab, XStringObject_CAST(dimref2)))->varobj;

                    data_offset += __var_size(XVarSymObject_CAST(symbol)->type) * (XNumberObject_CAST(dimref1)->val * XNumberObject_CAST(XVarSymObject_DimRef(symbol)[1])->val + XNumberObject_CAST(dimref2)->val);
            }

            symrepr = XVarSym_GetReprWithFileOffset(symbol, data_offset, datafile);

            if (symrepr != NULL) {
                printf("%s\n", symrepr);
                free(symrepr);
            } else 
                printf("Symbol got NULL repr!\n");
                
            tokarr[0] = NULL;
            tokarr[1] = NULL;
            tokarr[2] = NULL;
            tokarr[3] = NULL;
            tokarr[4] = NULL;
            tokarr[5] = NULL;

            tokindx = 0;
        }
    }

    XObject_Forget(lex);
    XObject_Forget(symtab);
    XObject_Forget(lexiter);

    return 0;
}
