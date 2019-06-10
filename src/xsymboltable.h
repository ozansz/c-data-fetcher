#ifndef _XSYMBOLTABLE_H
#define _XSYMBOLTABLE_H

#include <stdio.h>

#include "xobject.h"
#include "xtypes.h"
#include "xstring.h"
#include "xlexer.h"
#include "xnumber.h"
#include "xhashtable.h"

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

#define XVarSymObject_CAST(objptr) ((XVarSymObject *)(objptr))

XObject *XVarSymObject_Creat(XStringObject *name, XT_SymType type, XT_Number file_pos, struct __xvarsym_arrspec arrspec);
void XVarSym_AssignValueFromStream(XObject *sym, XObject *ht, FILE *stream);
void XVarSym_Dump(XObject *sym);
XObject *XSymbolTable_ConstructFromLEX(XObject *lex, FILE *datastream);
XObject *XSymbolTable_RetrieveObject(XObject *ht, XStringObject *key);

XT_Number __var_size(XT_SymType type);

void XVarSym_Forget(void *sym);

void XSymbolTable_LinearDump(XObject *tab);

char *XVarSym_GetReprWithFileOffset(XObject *sym, long offset, FILE *stream);

#endif