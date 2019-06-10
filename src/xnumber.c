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