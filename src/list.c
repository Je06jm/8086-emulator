#include "list.h"

#include "error.h"

#include <stdlib.h>
#include <memory.h>

#ifdef EMU8086_DEBUG
#include <assert.h>
#endif

#define LIST_RESERVE_ADD 16

#ifdef EMU8086_DEBUG
#define ListTest(self)\
    assert(self != NULL);\
    assert(self->PushBack != NULL);\
    assert(self->Shrink != NULL);\
    assert(self->Index != NULL);\
    assert(self->GetSize != NULL);\
    assert(self->Finish != NULL);\
    assert((self->data == NULL) && (self->_reserved == 0));\
    assert(self->count <= self->_reserved);\
    assert(self->_size != 0);
#else
#define ListTest(self)
#endif

void ListPushBack(List* self, void* item) {
    ListTest(self);
#ifdef EMU8086_DEBUG
    assert(item != NULL);
#endif

    // Check for room first
    if (self->count >= self->_reserved) {
        // No room; Add N more elements
        uintptr_t new_size = self->_mem_size + LIST_RESERVE_ADD * self->_size;
        void* data = malloc(new_size);
        if (data == NULL) {
            print_error("Could not allocate memory for list push back");
        }

        if (self->_data != NULL) {
            memcpy(data, self->_data, self->_mem_size);

            free(self->_data);
        }
        
        self->_data = data;
        self->_mem_size = new_size;
        self->_reserved += LIST_RESERVE_ADD;
    }

    // Add element to back
    uintptr_t addr = (uintptr_t)self->_data;
    addr += self->_size * self->count;

    memcpy((void*)addr, item, self->_size);

    self->count++;
}

void ListShrink(List* self) {
    ListTest(self);

    void* data = NULL;
    uintptr_t size = self->_size * self->count;

    if (self->count != 0) {
        void* data = malloc(size);

        memcpy(data, self->_data, size);
    }
    
    if (self->_data != NULL) {
        free(self->_data);
    }
    
    self->_data = data;
    self->_mem_size = size;
    self->_reserved = self->count;
}

void* ListIndex(List* self, uintptr_t index) {
    ListTest(self);

    if (index > self->count) {
        print_error("List bounds error. Attempted to index %llu",
            (unsigned long long)index);
    }

    uintptr_t addr = (uintptr_t)self->_data;
    addr += self->_size * index;

    return (void*)addr;
}

uintptr_t ListGetSize(List* self) {
    ListTest(self);

    return self->_size * self->count;
}

void ListFinish(List* self) {
    ListTest(self);

    if (self->_data != NULL) {
        free(self->_data);
    }
}


void List_Init(List* list, uintptr_t item_size) {
    list->PushBack = ListPushBack;
    list->Shrink = ListShrink;
    list->Index = ListIndex;
    list->GetSize = ListGetSize;
    list->Finish = ListFinish;
    
    list->count = 0;

    list->_size = item_size;
    list->_mem_size = 0;
    list->_reserved = 0;
    list->_data = NULL;
}
