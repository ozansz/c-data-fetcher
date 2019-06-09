#ifndef _XOBJ_H
#define _XOBJ_H

#include "xtypes.h"

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

XObject *XObject_Creat(void);
void XObject_Forget(XObject *obj);

#endif