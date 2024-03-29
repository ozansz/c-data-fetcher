#include <stdlib.h>
#include <stdio.h>

#include "xobject.h"
#include "xtypes.h"
#include "xstring.h"
#include "xhashtable.h"

XObject *XHTEntry_Creat(XObject *dataobject) {
    XHTEntryObject *ent;

    ent = (XHTEntryObject *) malloc(sizeof(XHTEntryObject));

    if (ent == NULL)
        return NULL;

    XObject_TYPE(ent) = XType_HashTableEntry;
    XObject_DESTRUCTOR(ent) = XHTEntry_Forget;

    ent->dataobject = dataobject;
    ent->key = -1;
    ent->next = NULL;
    ent->prev = NULL;

    return XObject_CAST(ent);
}

XObject *XHashTable_Creat(XT_HTSize size, XT_HTHashFuncPtr hashfn) {
    XT_HTSize indx;
    XHashTableObject *ht;
    XHTEntryObject **bucket;

    ht = (XHashTableObject *) malloc(sizeof(XHashTableObject));
    bucket = (XHTEntryObject**) malloc(size * sizeof(XHTEntryObject*));

    if (ht == NULL || bucket == NULL)
        return NULL;

    XObject_TYPE(ht) = XType_HashTable;
    XObject_DESTRUCTOR(ht) = XHashTable_Forget;

    ht->hashbucket = bucket;
    ht->hashfn = hashfn;
    ht->size = size;

    for (indx = 0; indx < ht->size; indx++)
        ht->hashbucket[indx] = NULL;

    return XObject_CAST(ht);
}

XT_HTHashKey XHashTable_DefaultHashFunc(XStringObject *str, XT_HTSize tablesize) {
    char ch;
    XT_StringSize indx;
    unsigned long charsum;
    XObject *striter;

    striter = XString_GetIter(XObject_CAST(str));

    for (indx = 0, charsum = 0; (ch = XStringIter_IterNext(striter)) != XSTRING_ENDOFITER && indx <= 5; indx++)
        charsum += indx * indx * char_to_num(ch);

    XObject_Forget(striter);

    return (XT_HTHashKey)(XHT_HASH_MULTIPLIER * charsum % tablesize);
}

XT_HTHashKey XHashTable_InsertObject(XObject *ht, XObject *dataobject, XStringObject *key) {
    XHashTableObject *htobj;
    XT_HTHashKey keyhash;
    XHTEntryObject *last_entry;
    XObject *new_entry;

    htobj = XHashTableObject_CAST(ht);
    keyhash = htobj->hashfn(key, htobj->size);

    new_entry = XHTEntry_Creat(dataobject);

    if (new_entry == NULL) 
        return -1;

    XHTEntryObject_CAST(new_entry)->key = keyhash;

    if (htobj->hashbucket[keyhash] == NULL)
        htobj->hashbucket[keyhash] = XHTEntryObject_CAST(new_entry);
    else {
        for (last_entry = htobj->hashbucket[keyhash]; last_entry->next != NULL; last_entry = last_entry->next);
        last_entry->next = XHTEntryObject_CAST(new_entry);
        XHTEntryObject_CAST(new_entry)->prev = last_entry;
    }

    return keyhash;
}

void _XHashTable_Debug(XObject *ht) {
    XT_HTSize indx;
    XHashTableObject *htobj;
    XHTEntryObject *ent;

    htobj = XHashTableObject_CAST(ht);

    printf("\n\n");

    for (indx = 0; indx < htobj->size; indx++) {
        printf("#%ld\t| ", indx);

        ent = htobj->hashbucket[indx];

        if (ent == NULL)
            printf("NULL |\n");
        else {
            for (; ent != NULL; ent = ent->next)
                printf("%p (type: %d) | ", (void *)(ent->dataobject), XObject_TYPE(ent->dataobject));
            
            printf("\n");
        }
    }
}

void XHTEntry_Forget(void *ent) {
    if (ent == NULL)
        return;

    XObject_Forget(XHTEntryObject_CAST(ent)->dataobject);
    free(ent);
}

void XHashTable_Forget(void *ht) {
    XT_HTSize indx;
    XHTEntryObject *ent, *nextent;

    if (ht == NULL)
        return;

    for (indx = 0; indx < XHashTableObject_CAST(ht)->size; indx++) {
        for (ent = XHashTableObject_CAST(ht)->hashbucket[indx]; ent != NULL; ) {
            nextent = ent->next;
            XObject_Forget(XObject_CAST(ent));
            ent = nextent;
        }
    }

    free(XHashTableObject_CAST(ht)->hashbucket);
    free(ht);
}