#ifndef _XNUMBER_H
#define _XNUMBER_H

#include "xobject.h"
#include "xtypes.h"
#include "xstring.h"

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

XObject *XNumber_Creat(XT_Number *valptr);
XObject *XNumber_FromString(XObject *str);
XObject *XFPNumber_Creat(XT_FPNumber *valptr);

void XNumber_Forget(void *num);
void XFPNumber_Forget(void *fpnum);

#endif