#include <stdio.h>
#include <stdio.h>

#include "xobject.h"
#include "xtypes.h"
#include "xstring.h"
#include "xlexer.h"
#include "xnumber.h"
#include "xhashtable.h"
#include "xsymboltable.h"

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

XObject *XVarSymObject_Creat(XStringObject *name, XT_SymType type, XT_Number file_pos, struct __xvarsym_arrspec arrspec) {
    XVarSymObject *sym;

    sym = (XVarSymObject *) malloc(sizeof(XVarSymObject));

    if (sym == NULL)
        return NULL;

    sym->ob_head.type = XType_VarSym;
    sym->ob_head.destructor = XVarSym_Forget;

    sym->name = name;
    sym->type = type;
    sym->file_pos = file_pos;
    sym->varobj = NULL;

    sym->arrspec.arr_dim = arrspec.arr_dim;
    sym->arrspec.dim_ref[0] = arrspec.dim_ref[0];
    sym->arrspec.dim_ref[1] = arrspec.dim_ref[1];

    sym->bytesize = __var_size(sym->type);

    /* DO THE BYTESIZE CALC HERE */

    return XObject_CAST(sym);
}

void XVarSym_AssignValueFromStream(XObject *sym, XObject *ht, FILE *stream) {
    long seek_store;
    XT_Number _ref_val;
    XVarSymObject *symobj;
    XObject *refobj;

    symobj = XVarSymObject_CAST(sym);

    if (symobj->arrspec.arr_dim == 0) {
        seek_store = ftell(stream);
        fseek(stream, symobj->file_pos, SEEK_SET);

        fread(&_ref_val, __var_size(symobj->type), 1, stream);
        symobj->varobj = XNumber_Creat(&_ref_val);
        
        fseek(stream, seek_store, SEEK_SET);
    } else {
        if (symobj->arrspec.arr_dim >= 1) {
            if (symobj->arrspec.dim_ref[0]->ob_head.type == XType_String) {
                refobj = XSymbolTable_RetrieveObject(ht, XStringObject_CAST(symobj->arrspec.dim_ref[0]));
                
                if (refobj == NULL) {
                    printf("[!] XVarSym_AssignValueFromStream: Ref object <%p> of symbol <%p> is not in table <%p> !\n", (void *)(symobj->arrspec.dim_ref[0]), (void *)sym, (void *)ht);
                    return;
                }

                if (XVarSymObject_CAST(refobj)->arrspec.arr_dim != 0) {
                    printf("[!] XVarSym_AssignValueFromStream: Ref object <%p> of symbol <%p> is not in type scalar ! (arrspec.arr_dim: %d)\n", (void *)(symobj->arrspec.dim_ref[0]), (void *)sym, XVarSymObject_CAST(refobj)->arrspec.arr_dim);
                    return;
                }

                _ref_val = XNumberObject_CAST(XVarSymObject_CAST(refobj)->varobj)->val;
                symobj->arrspec.dim_ref[0] = XNumber_Creat(&_ref_val);
            }
        }

        if (symobj->arrspec.arr_dim == 2) {
            if (symobj->arrspec.dim_ref[1]->ob_head.type == XType_String) {
                refobj = XSymbolTable_RetrieveObject(ht, XStringObject_CAST(symobj->arrspec.dim_ref[1]));
                
                if (refobj == NULL) {
                    printf("[!] XVarSym_AssignValueFromStream: Ref object <%p> of symbol <%p> is not in table <%p> !\n", (void *)(symobj->arrspec.dim_ref[1]), (void *)sym, (void *)ht);
                    return;
                }

                if (XVarSymObject_CAST(refobj)->arrspec.arr_dim != 0) {
                    printf("[!] XVarSym_AssignValueFromStream: Ref object <%p> of symbol <%p> is not in type scalar ! (arrspec.arr_dim: %d)\n", (void *)(symobj->arrspec.dim_ref[1]), (void *)sym, XVarSymObject_CAST(refobj)->arrspec.arr_dim);
                    return;
                }

                _ref_val = XNumberObject_CAST(XVarSymObject_CAST(refobj)->varobj)->val;
                symobj->arrspec.dim_ref[1] = XNumber_Creat(&_ref_val);
            }
        }
    }

    if (symobj->arrspec.arr_dim == 1)
        symobj->bytesize = __var_size(symobj->type) * XNumberObject_CAST(symobj->arrspec.dim_ref[0])->val;

    if (symobj->arrspec.arr_dim == 2)
        symobj->bytesize = __var_size(symobj->type) * XNumberObject_CAST(symobj->arrspec.dim_ref[0])->val * XNumberObject_CAST(symobj->arrspec.dim_ref[1])->val;
}

void XVarSym_Dump(XObject *sym) {
    printf("\nSymbol at <%p>", (void *)sym);
    printf("\n  name      : %s", XString_GetString(XObject_CAST(XVarSymObject_CAST(sym)->name)));
    printf("\n  type      : %d", XVarSymObject_CAST(sym)->type);
    printf("\n  file pos  : %ld", XVarSymObject_CAST(sym)->file_pos);
    printf("\n  arr dim   : %d", XVarSymObject_CAST(sym)->arrspec.arr_dim);
    printf("\n  dim_ref[0]: %p", (void *)(XVarSymObject_CAST(sym)->arrspec.dim_ref[0]));

    if (XVarSymObject_CAST(sym)->arrspec.dim_ref[0] != NULL && XVarSymObject_CAST(sym)->arrspec.dim_ref[0]->ob_head.type == XType_Number)
        printf(" (%ld)", XNumberObject_CAST(XVarSymObject_CAST(sym)->arrspec.dim_ref[0])->val);
        
    printf("\n  dim_ref[1]: %p", (void *)(XVarSymObject_CAST(sym)->arrspec.dim_ref[1]));

    if (XVarSymObject_CAST(sym)->arrspec.dim_ref[1] != NULL && XVarSymObject_CAST(sym)->arrspec.dim_ref[1]->ob_head.type == XType_Number)
        printf(" (%ld)", XNumberObject_CAST(XVarSymObject_CAST(sym)->arrspec.dim_ref[1])->val);

    printf("\n  bytesize  : %ld", XVarSymObject_CAST(sym)->bytesize);

    if (XVarSymObject_CAST(sym)->varobj != NULL) {
        printf("\n  varobj    : %p", (void *)(XVarSymObject_CAST(sym)->varobj));

        if (XVarSymObject_CAST(sym)->varobj->ob_head.type == XType_Number)
            printf(" (%ld)", XNumberObject_CAST(XVarSymObject_CAST(sym)->varobj)->val);

        printf("\n  varobj_typ: %d", XVarSymObject_CAST(sym)->varobj->ob_head.type);
    }
}

XObject *XSymbolTable_ConstructFromLEX(XObject *lex, FILE *datastream) {
    int lexiterstatcode = 0, tokindx = 0;
    XObject *ht, *symbol, *lexiter, *currtok, *tokarr[8];
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
                arrspec.arr_dim = 2;
                arrspec.dim_ref[0] = XLEXTokenObject_CAST(tokarr[3])->dataobject;
                arrspec.dim_ref[1] = XLEXTokenObject_CAST(tokarr[6])->dataobject;
            } else if (tokindx == 5) {
                arrspec.arr_dim = 1;
                arrspec.dim_ref[0] = XLEXTokenObject_CAST(tokarr[3])->dataobject;
                arrspec.dim_ref[1] = NULL;
            } else {
                arrspec.arr_dim = 0;
                arrspec.dim_ref[0] = NULL;
                arrspec.dim_ref[1] = NULL;
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

            if (XLEXTokenObject_CAST(tokarr[1])->dataobject->ob_head.type != XType_String) {
                printf("[!] ERR ident is not str! (is %d)", XLEXTokenObject_CAST(tokarr[1])->dataobject->ob_head.type);
                return NULL;
            }

            symbol = XVarSymObject_Creat(XStringObject_CAST(XLEXTokenObject_CAST(tokarr[1])->dataobject), symtype, filepos, arrspec);
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

XObject *XSymbolTable_RetrieveObject(XObject *ht, XStringObject *key) {
    XHashTableObject *htobj;
    XT_HTHashKey keyhash;
    XHTEntryObject *htentry;

    htobj = XHashTableObject_CAST(ht);
    keyhash = htobj->hashfn(key, htobj->size);

    for (htentry = htobj->hashbucket[keyhash]; htentry != NULL; htentry = htentry->next)
        if (XString_CompareWith(XObject_CAST(XVarSymObject_CAST(htentry->dataobject)->name), key->buf, key->__last_indx)) {
            printf("ret dataobj\n");
            return htentry->dataobject;
        }

    return NULL;
}

void XVarSym_Forget(void *sym) {
    XObject_Forget(XObject_CAST(XVarSymObject_CAST(sym)->name));
    XObject_Forget(XVarSymObject_CAST(sym)->varobj);

    free(sym);
}