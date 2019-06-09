#include <stdlib.h>

#include "xobject.h"
#include "xtypes.h"
#include "xlexer.h"
#include "xstring.h"
#include "xnumber.h"

XObject *XLEX_Creat(void) {
    XLEXObject *obj;

    obj = (XLEXObject *) malloc(sizeof(XLEXObject));

    if (obj == NULL)
        return NULL;

    obj->ob_head.type = XType_LEX;
    obj->ob_head.destructor = XLEX_Forget;

    obj->head = NULL;
    obj->size = 0;
    obj->__last = NULL;

    return XObject_CAST(obj);
}

XObject *XLEXToken_Creat(XT_LEXTokenType ttype, XObject *dataobject) {
    XLEXTokenObject *tobj;

    tobj = (XLEXTokenObject *) malloc(sizeof(XLEXTokenObject));

    if (tobj == NULL)
        return NULL;

    tobj->ob_head.type = XType_LEXToken;
    tobj->ob_head.destructor = XLEXToken_Forget;

    tobj->type = ttype;
    tobj->dataobject = dataobject;

    tobj->next = NULL;
    tobj->prev = NULL;    

    return XObject_CAST(tobj);
}

XT_LEXTokenType XLEXToken_TypeFromString(XObject *str) {
    int flag_num = 0, flag_char = 0;
    char ch;
    XObject *striter;

    if (XStringObject_CAST(str)->__last_indx == 0)
        return XLTT__ERR;

    striter = XString_GetIter(str);

    while ((ch = XStringIter_IterNext(striter)) != XSTRING_ENDOFITER) {
        if (_is_letter(ch))
            flag_char = 1;
        else if (_is_number(ch))
            flag_num = 1;

        if (flag_num && flag_char)
            break;
    }

    XObject_Forget(striter);

    if (flag_num && flag_char)
        return XLTT_IDENTIFIER;
    else if (flag_num && !flag_char)
        return XLTT_NNUM;
    else {
        if (XString_CompareWith(str, "char", 4))
            return XLTT_CHAR;
        else if (XString_CompareWith(str, "uchar", 5))
            return XLTT_UCHAR;
        else if (XString_CompareWith(str, "int", 3))
            return XLTT_INT;
        else if (XString_CompareWith(str, "uint", 4))
            return XLTT_UINT;
        else if (XString_CompareWith(str, "long", 4))
            return XLTT_LONG;
        else if (XString_CompareWith(str, "float", 5))
            return XLTT_FLOAT;
        else if (XString_CompareWith(str, "double", 6))
            return XLTT_DOUBLE;
        else 
            return XLTT_IDENTIFIER;
    }

    return XLTT__ERR;
}

XT_LEXTokenListSize XLEX_AppendNode(XObject *lex, XT_LEXTokenType ttype, XObject *dataobject) {
    XLEXObject *lexobj;
    XObject *tobj, *_tmpobj;

    lexobj = XLEXObject_CAST(lex);

    if (ttype == XLTT__ERR)
        return -1;

    if (ttype == XLTT_NNUM) {
        _tmpobj = dataobject;
        dataobject = XNumber_FromString(dataobject);
        XObject_Forget(_tmpobj);
    }

    tobj = XLEXToken_Creat(ttype, dataobject);

    if (tobj == NULL)
        return -1;

    if (lexobj->size == 0)
        lexobj->head = XLEXTokenObject_CAST(tobj);
    else {
        XLEXTokenObject_CAST(tobj)->prev = lexobj->__last;
        lexobj->__last->next = XLEXTokenObject_CAST(tobj);
    }

    lexobj->__last = XLEXTokenObject_CAST(tobj);
    lexobj->size++;

    return lexobj->size;
}

XObject *XLEX_GetIter(XObject *lex) {
    XLEXObject *lexobj;
    XLEXIterObject *iobj;

    lexobj = XLEXObject_CAST(lex);

    iobj = (XLEXIterObject *) malloc(sizeof(XLEXIterObject));

    if (iobj == NULL)
        return NULL;

    iobj->ob_head.type = XType_LEXIter;
    iobj->ob_head.destructor = XLEXIter_Forget;

    iobj->currtok = lexobj->head;
    iobj->iterindex = 0;

    return XObject_CAST(iobj);
}

XObject *XLEXIter_IterNext(XObject *iter, int *statcode) {
    XLEXIterObject *iterobj;
    XLEXTokenObject *ret;

    iterobj = XLEXIterObject_CAST(iter);

    if (iterobj == NULL) {
        *statcode = XLTT__ERR;
        return NULL;
    }

    if (iterobj->currtok == NULL) {
        *statcode = XLTT__ENDOFITER;
        return NULL;
    }

    ret = iterobj->currtok;
    iterobj->currtok = ret->next;
    iterobj->iterindex++;

    return XObject_CAST(ret);
}

XObject *XLEX_LexString(XObject *str) {
    char ch;
    XObject *striter, *lex, *buffer;
    XT_LEXTokenType ttype;

    striter = XString_GetIter(str);
    lex = XLEX_Creat();

    buffer = XString_Creat();

    while ((ch = XStringIter_IterNext(striter)) != XSTRING_ENDOFITER) {
        if (_is_space(ch)) {
            if (XStringObject_CAST(buffer)->__last_indx > 0) {
                ttype = XLEXToken_TypeFromString(buffer);
                XLEX_AppendNode(lex, ttype, buffer);
                buffer = XString_Creat();
            } else
                continue;
        } else if (ch == ',') {
            if (XStringObject_CAST(buffer)->__last_indx > 0) {
                ttype = XLEXToken_TypeFromString(buffer);
                XLEX_AppendNode(lex, ttype, buffer);
                buffer = XString_Creat();
            }
            XLEX_AppendNode(lex, XLTT_COMMA, NULL);
        } else if (ch == '[') {
            if (XStringObject_CAST(buffer)->__last_indx > 0) {
                ttype = XLEXToken_TypeFromString(buffer);
                XLEX_AppendNode(lex, ttype, buffer);
                buffer = XString_Creat();
            }
            XLEX_AppendNode(lex, XLTT_LBRACKET, NULL);
        } else if (ch == ']') {
            if (XStringObject_CAST(buffer)->__last_indx > 0) {
                ttype = XLEXToken_TypeFromString(buffer);
                XLEX_AppendNode(lex, ttype, buffer);
                buffer = XString_Creat();
            }
            XLEX_AppendNode(lex, XLTT_RBRACKET, NULL);
        } else {
            XString_PushChar(buffer, ch);
        }
    }

    if (XStringObject_CAST(buffer)->size > 0) {
        ttype = XLEXToken_TypeFromString(buffer);
        XLEX_AppendNode(lex, ttype, buffer);
    }

    XObject_Forget(striter);
    XObject_Forget(buffer);

    return XObject_CAST(lex);
}

void _XLEXToken_Dump(XLEXTokenObject *tok) {
    switch (tok->type) {
        case XLTT__ERR:
            printf("ERR");
            break;
        case XLTT__ENDOFITER:
            printf("END OF ITER");
            break;
        case XLTT_NNUM:
            printf("NNUM(%ld)", XNumberObject_CAST(tok->dataobject)->val);
            break;
        case XLTT_IDENTIFIER:
            printf("IDENTIFIER(%s)[%ld]", XString_GetString(tok->dataobject), XStringObject_CAST(tok->dataobject)->__last_indx);
            break;
        case XLTT_CHAR:
            printf("CHAR");
            break;
        case XLTT_UCHAR:
            printf("UCHAR");
            break;
        case XLTT_INT:
            printf("INT");
            break;
        case XLTT_UINT:
            printf("UINT");
            break;
        case XLTT_LONG:
            printf("LONG");
            break;
        case XLTT_FLOAT:
            printf("FLOAT");
            break;
        case XLTT_DOUBLE:
            printf("DOUBLE");
            break;
        case XLTT_COMMA:
            printf("COMMA(,)");
            break;
        case XLTT_LBRACKET:
            printf("LBRACKET([)");
            break;
        case XLTT_RBRACKET:
            printf("RBRACKET(])");
            break;    
        default:
            break;
    }

    printf(" <self: %p>", (void *)tok);

    if (tok->dataobject != NULL)
        printf(" <data: %p>", (void *)(tok->dataobject));
    else
        printf(" <data: NULL>");
}

void _XLEX_Dump(XObject *lex) {
    XT_LEXTokenListSize indx = 0;
    XLEXTokenObject *tok;

    for (tok = XLEXObject_CAST(lex)->head; tok != NULL; tok = tok->next, indx++) {
        printf("\n#%ld    ", indx);
        _XLEXToken_Dump(tok);
    }
}

void XLEXToken_Forget(void *tok) {
    if (tok == NULL)
        return;
        
    if (XLEXTokenObject_CAST(tok)->dataobject != NULL)
        XObject_Forget(XLEXTokenObject_CAST(tok)->dataobject);

    free(tok);
}

void XLEX_Forget(void *lex) {
    XLEXTokenObject *tok, *nexttok;

    if (lex == NULL)
        return;

    for (tok = XLEXObject_CAST(lex)->head; tok != NULL; tok = nexttok) {
        if (tok->next != NULL)
            nexttok = tok->next;
        else
            nexttok = NULL;

        XObject_Forget(XObject_CAST(tok));
    }
        

    free(lex);
}

void XLEXIter_Forget(void *iter) {
    if (iter == NULL)
        return;
        
    free(iter);
}