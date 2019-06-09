#ifndef _XHASHTABLE_H
#define _XHASHTABLE_H

#include <stdlib.h>

#include "xobject.h"
#include "xtypes.h"
#include "xstring.h"

#define XHT_HASH_MULTIPLIER (XT_HTHashKey)61803

typedef XT_HTHashKey (*XT_HTHashFuncPtr)(XStringObject *, XT_HTSize);

typedef struct _xhtentry {
    XObject_HEAD
    XT_HTHashKey key;
    XObject *dataobject;
    struct _xhtentry *next;
    struct _xhtentry *prev;
} XHTEntryObject;

typedef struct _xhashtable {
    XObject_HEAD
    XT_HTSize size;
    XT_HTHashFuncPtr hashfn;
    XHTEntryObject **hashbucket;
} XHashTableObject;

#define XHTEntryObject_CAST(objptr) ((XHTEntryObject *)(objptr))
#define XHashTableObject_CAST(objptr) ((XHashTableObject *)(objptr))

XObject *XHTEntry_Creat(XObject *dataobject);
XObject *XHashTable_Creat(XT_HTSize size, XT_HTHashFuncPtr hashfn);
XT_HTHashKey XHashTable_DefaultHashFunc(XStringObject *str, XT_HTSize tablesize);
XT_HTHashKey XHashTable_InsertObject(XObject *ht, XObject *dataobject, XStringObject *key);
void _XHashTable_Debug(XObject *ht);

void XHTEntry_Forget(void *ent);
void XHashTable_Forget(void *ht);

#endif
