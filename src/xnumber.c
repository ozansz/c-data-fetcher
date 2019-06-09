#include <stdlib.h>

#include "xobject.h"
#include "xtypes.h"
#include "xstring.h"
#include "xnumber.h"

XObject *XNumber_Creat(XT_Number *valptr) {
    XNumberObject *num;

    num = (XNumberObject *) malloc(sizeof(XNumberObject));

    if (num == NULL)
        return NULL;

    num->ob_head.type = XType_Number;
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

    return XNumber_Creat(&nval);
}

XObject *XFPNumber_Creat(XT_FPNumber *valptr) {
    XFPNumberObject *fpnum;

    fpnum = (XFPNumberObject *) malloc(sizeof(XFPNumberObject));

    if (fpnum == NULL)
        return NULL;

    fpnum->ob_head.type = XType_FPNumber;
    fpnum->val = *valptr;

    return XObject_CAST(fpnum);
}

void XNumber_Forget(void *num) {
    free(num);
}

void XFPNumber_Forget(void *fpnum) {
    free(fpnum);
}