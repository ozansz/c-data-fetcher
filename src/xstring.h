#ifndef _XSTRING_H
#define _XSTRING_H

#include <stdio.h>

#include "xobject.h"
#include "xtypes.h"

#define XSTRING_EXTEND_SIZE (XT_StringSize)16
#define XSTRING_INITIAL_SIZE (XT_StringSize)16

#define XSTRING_ENDOFITER   '\0'

typedef struct _xstringobject {
    XObject_HEAD
    XT_StringSize size;
    XT_StringSize __last_indx;
    char *buf;
} XStringObject;

typedef struct _xstringiterobject {
    XObject_HEAD
    XT_StringSize iterindex;
    XStringObject *str;
} XStringIterObject;

#define XStringObject_CAST(objptr) ((XStringObject *)(objptr))
#define XStringIterObject_CAST(objptr) ((XStringIterObject *)(objptr))

XObject *XString_Creat(void);
void _XString_FlushStringBuffer(XStringObject *str);
XT_StringSize _XString_Extend(XObject *str);
XT_StringSize XString_PushChar(XObject *str, char ch);
XT_StringSize XString_ScanUntilChar(FILE *stream, XObject *str, char stopchar);
void XString_Dump(XObject *str);
char *XString_GetString(XObject *str);
int XString_CompareWith(XObject *str, char *cmpstr, XT_StringSize size);
XObject *XString_Duplicate(XObject *str);

#define XString_ScanLine(stream, str) XString_ScanUntilChar(stream, str, '\n')

XObject *XString_GetIter(XObject *str);
char XStringIter_IterNext(XObject *iter);

int _is_number(char ch);
int _is_letter(char ch);
int _is_space(char ch);

XT_Number char_to_num(char ch);

void XString_Forget(void *str);
void XStringIter_Forget(void *iter);

#endif