#include <stdlib.h>
#include <stdio.h>

#include "xobject.h"
#include "xtypes.h"

XObject *XObject_Creat(void) {
    XObject *obj;

    obj = (XObject *) malloc(sizeof(XObject));

    if (obj == NULL)
        return NULL;

    obj->ob_head.type = XType_None;

    return obj;
}

void XObject_Forget(XObject *obj) {
    printf("\n[CALL] XObject_Forget");

    if (obj == NULL) {
        printf("(NULL)\n");
        return;
    }
        
    printf("(%p) (type: %d)\n", (void *)obj, obj->ob_head.type);

    if (XObject_TYPE(obj) != XType_None && XObject_DESTRUCTOR(obj) != NULL)
        XObject_DESTRUCTOR(obj)(obj);
}