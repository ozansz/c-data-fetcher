#ifndef _XLEXER_H
#define _XLEXER_H

#include "xobject.h"
#include "xtypes.h"

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

XObject *XLEX_Creat(void);
XObject *XLEXToken_Creat(XT_LEXTokenType ttype, XObject *dataobject);
XT_LEXTokenType XLEXToken_TypeFromString(XObject *str);
XT_LEXTokenListSize XLEX_AppendNode(XObject *lex, XT_LEXTokenType ttype, XObject *dataobject);
XObject *XLEX_GetIter(XObject *lex);
XObject *XLEXIter_IterNext(XObject *iter, int *statcode);
XObject *XLEX_LexString(XObject *str);

void XLEXToken_Forget(void *tok);
void XLEX_Forget(void *lex);
void XLEXIter_Forget(void *iter);

void _XLEXToken_Dump(XLEXTokenObject *tok);
void _XLEX_Dump(XObject *lex);

#endif