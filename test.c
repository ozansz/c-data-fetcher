#include <stdlib.h>
#include <stdio.h>

#include "xlexer.h"
#include "xstring.h"
#include "xsymboltable.h"
#include "xhashtable.h"

int main(int argc, char **argv, char **envp) {
    XObject *str, *lex, *symtab;
    XObject *lexiter, *currtok, *symbol;
    XLEXTokenObject *tokarr[8];
    XT_Number tokindx;

    int _statc = 0;
    char *fnam;
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
    _XHashTable_Debug(symtab);

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
            XVarSym_Dump(symbol);

            tokarr[0] = NULL;
            tokarr[1] = NULL;
            tokarr[2] = NULL;
            tokarr[3] = NULL;
            tokarr[4] = NULL;
            tokarr[5] = NULL;
            tokarr[6] = NULL;
            tokarr[7] = NULL;

            tokindx = 0;
        }
    }

    XObject_Forget(lex);
    XObject_Forget(symtab);
    XObject_Forget(lexiter);

    return 0;
}
