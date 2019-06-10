#include <stdlib.h>
#include <stdio.h>

#include "xlexer.h"
#include "xstring.h"
#include "xsymboltable.h"
#include "xhashtable.h"

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

            switch (XVarSymObject_CAST(symbol)->arrspec.arr_dim)
            {
                case 0:
                    data_offset = XVarSymObject_CAST(symbol)->file_pos;
                    break;
                case 1:
                    data_offset = XVarSymObject_CAST(symbol)->file_pos;
                    dimref1 = tokarr[2]->dataobject;

                    if (dimref1->ob_head.type != XType_Number)
                        dimref1 = XVarSymObject_CAST(XSymbolTable_RetrieveObject(symtab, XStringObject_CAST(dimref1)))->varobj;
                    
                    data_offset += __var_size(XVarSymObject_CAST(symbol)->type) * XNumberObject_CAST(dimref1)->val;
                    break;
                case 2:
                    data_offset = XVarSymObject_CAST(symbol)->file_pos;
                    dimref1 = tokarr[2]->dataobject;
                    dimref2 = tokarr[5]->dataobject;

                    if (dimref1->ob_head.type != XType_Number)
                        dimref1 = XVarSymObject_CAST(XSymbolTable_RetrieveObject(symtab, XStringObject_CAST(dimref1)))->varobj;

                    if (dimref2->ob_head.type != XType_Number)
                        dimref2 = XVarSymObject_CAST(XSymbolTable_RetrieveObject(symtab, XStringObject_CAST(dimref2)))->varobj;

                    data_offset += __var_size(XVarSymObject_CAST(symbol)->type) * (XNumberObject_CAST(dimref1)->val * XNumberObject_CAST(XVarSymObject_CAST(symbol)->arrspec.dim_ref[1])->val + XNumberObject_CAST(dimref2)->val);
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
