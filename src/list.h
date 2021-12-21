#ifndef EMU8086_LIST_H
#define EMU8086_LIST_H

#include <Emu8086/types.h>

typedef struct _List List;

typedef void (*_List_PushBack)(List* self, void* item);
typedef void (*_List_Shrink)(List* self);
typedef void* (*_List_Index)(List* self, uintptr_t index);
typedef uintptr_t (*_List_GetSize)(List* self);
typedef void (*_List_Finish)(List* self);

struct _List {
    _List_PushBack PushBack;
    _List_Shrink Shrink;
    _List_Index Index;
    _List_GetSize GetSize;
    _List_Finish Finish;

    uintptr_t count;

    uintptr_t _size;
    uintptr_t _mem_size;
    uintptr_t _reserved;
    void* _data;
};

void List_Init(List* list, uintptr_t item_size);

#endif