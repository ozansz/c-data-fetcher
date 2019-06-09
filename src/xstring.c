#include <stdlib.h>
#include <stdio.h>

#include "xobject.h"
#include "xtypes.h"
#include "xstring.h"

void _XString_FlushStringBuffer(XStringObject *str) {
    XT_StringSize indx;
    XStringObject *strobj;

    strobj = XStringObject_CAST(str);

    for (indx = 0; indx < strobj->size; indx++)
        strobj->buf[indx] = 0;
}

XObject *XString_Creat(void) {
    XStringObject *str;

    str = (XStringObject *) malloc(sizeof(XStringObject));

    if (str == NULL)
        return NULL;

    str->buf = (char *) malloc(XSTRING_INITIAL_SIZE * sizeof(char));

    if (str->buf == NULL)
        return NULL;

    str->ob_head.type = XType_String;
    str->ob_head.destructor = XString_Forget;

    str->size = XSTRING_INITIAL_SIZE;
    str->__last_indx = 0;

    _XString_FlushStringBuffer(str);

    return XObject_CAST(str);
}

XT_StringSize _XString_Extend(XObject *str) {
    XStringObject *strobj;

    strobj = XStringObject_CAST(str);
    
    strobj->size += XSTRING_EXTEND_SIZE;
    strobj->buf = (char *) realloc(strobj->buf, sizeof(char) * strobj->size);

    return strobj->size;
}

XT_StringSize XString_PushChar(XObject *str, char ch) {
    XStringObject *strobj;

    strobj = XStringObject_CAST(str);

    if (strobj->size <= strobj->__last_indx)
            _XString_Extend(str);

    strobj->buf[strobj->__last_indx++] = ch;

    return strobj->size;
}

XT_StringSize XString_ScanUntilChar(FILE *stream, XObject *str, char stopchar) {
    char ch;
    XStringObject *strobj;

    strobj = XStringObject_CAST(str);
    strobj->__last_indx = 0;

    while ((ch = fgetc(stream)) != stopchar)
        XString_PushChar(str, ch);

    return strobj->__last_indx;
}

XObject *XString_GetIter(XObject *str) {
    XStringIterObject *iter;

    iter = (XStringIterObject *) malloc(sizeof(XStringIterObject));

    if (iter == NULL)
        return NULL;

    iter->ob_head.type = XType_StringIter;
    iter->ob_head.destructor = XStringIter_Forget;

    iter->iterindex = 0;
    iter->str = XStringObject_CAST(str);

    return XObject_CAST(iter);
}

char XStringIter_IterNext(XObject *iter) {
    char ret;
    XStringIterObject *iterobj;

    iterobj = XStringIterObject_CAST(iter);

    if (iterobj->iterindex >= iterobj->str->__last_indx)
        return XSTRING_ENDOFITER;
    
    ret = iterobj->str->buf[iterobj->iterindex++];

    return ret;
}

int _is_number(char ch) {
    if (ch >= '0' && ch <= '9')
        return 1;

    return 0;
}

int _is_letter(char ch) {
    if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        return 1;

    return 0;
}

int _is_space(char ch) {
    if (ch == ' ' || ch == '\n' || ch == '\t')
        return 1;

    return 0;
}

void XString_Dump(XObject *str) {
    XT_StringSize indx;
    XStringObject *strobj;

    strobj = XStringObject_CAST(str);

    for (indx = 0; indx < strobj->__last_indx; indx++) {
        if (!_is_letter(strobj->buf[indx])) 
            printf("\n%ld    (%d)", indx, (int)(strobj->buf[indx]));
        else
            printf("\n%ld    %c", indx, strobj->buf[indx]);
    }
}

XT_Number char_to_num(char ch) {
    return ch - '0';
}

int XString_CompareWith(XObject *str, char *cmpstr, XT_StringSize size) {
    XT_StringSize indx;
    XStringObject *strobj;

    strobj = XStringObject_CAST(str);

    if (strobj->__last_indx != size)
        return 0;

    for (indx = 0; indx < strobj->__last_indx && indx < size; indx++)
        if (strobj->buf[indx] != cmpstr[indx])
            return 0;

    return 1;
}

void XString_Forget(void *str) {
    XStringObject *strobj;

    if (str == NULL)
        return;

    strobj = XStringObject_CAST(str);

    free(strobj->buf);
    free(str);
}

char *XString_GetString(XObject *str) {
    XT_StringSize indx;
    char *buf;

    buf = (char *) malloc(XStringObject_CAST(str)->__last_indx * sizeof(char) + 1);

    if (buf == NULL)
        return NULL;

    for (indx = 0; indx < XStringObject_CAST(str)->__last_indx; indx++)
        buf[indx] = XStringObject_CAST(str)->buf[indx];

    buf[indx] = '\0';

    return buf;
}

void XStringIter_Forget(void *iter) {
    if (iter == NULL)
        return;
        
    free(iter);
}

XObject *XString_Duplicate(XObject *str) {
    char ch;
    XObject *newstr, *striter;

    newstr = XString_Creat();
    striter = XString_GetIter(str);

    while ((ch = XStringIter_IterNext(striter)) != XSTRING_ENDOFITER)
        XString_PushChar(newstr, ch);

    XObject_Forget(striter);

    return newstr;
}