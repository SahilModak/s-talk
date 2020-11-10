#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

static Node node_array[LIST_MAX_NUM_NODES];
static List list_array[LIST_MAX_NUM_HEADS];

static int available_heads = LIST_MAX_NUM_HEADS;
static int available_nodes = LIST_MAX_NUM_NODES;
static bool create_flag = true;
static int arrIndex[LIST_MAX_NUM_NODES];
static int headIndex[LIST_MAX_NUM_HEADS];

// Makes a new, empty list, and returns its reference on success. 
// Returns a NULL pointer on failure.
List* List_create(){
    
    if(create_flag){
        //initializing helper arrays for nodes and lists
        //only executes when the first list is called
        int i = 0;
        create_flag =false;
        while (i < LIST_MAX_NUM_NODES){
            arrIndex[i] = i;
            i++;
        }
        i = 0;
        while (i < LIST_MAX_NUM_HEADS){
            headIndex[i] = i;
            i++;
        }
        
    }
    if(available_heads < 1){
        //Cheking for available list heads
        //returning null if none found
        printf("No available List heads\n");
        return NULL;
    }
    else {
        //initializing list head
        int index = LIST_MAX_NUM_HEADS - available_heads;
        int pos = headIndex[index];
        list_array[pos].List_head = NULL;
        list_array[pos].List_tail = NULL;
        list_array[pos].List_current = NULL;
        list_array[pos].arrayIndex = pos;
        list_array[pos].total_items = 0;
        list_array[pos].beyondEnd = false;
        list_array[pos].beforeStart = false;
        available_heads--;
        headIndex[index] = -1;
        //printf("List created successfully\n");
        return &list_array[pos];
    }
    printf("List creation failed\n");
    return NULL;
}

// Returns the number of items in pList.
int List_count(List* pList){
    assert(pList != NULL);
    return pList->total_items;
}

// Returns a pointer to the first item in pList and makes the first item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_first(List* pList){
    assert(pList != NULL);
    if(pList->total_items == 0){
        //checking if list has items
        pList -> List_current = NULL;
        return NULL;
    }
    pList -> List_current = pList -> List_head;
    pList->beforeStart = false;
    pList->beyondEnd = false;
    return pList -> List_current -> item;
}

// Returns a pointer to the last item in pList and makes the last item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_last(List* pList){
    assert(pList != NULL);
    if(pList->total_items == 0){
        //checking if list has items
        pList -> List_current = NULL;
        return NULL;
    }
    pList -> List_current = pList -> List_tail;
    pList->beyondEnd = false;
    pList->beforeStart = false;
    return pList -> List_current -> item;
}

// Advances pList's current item by one, and returns a pointer to the new current item.
// If this operation advances the current item beyond the end of the pList, a NULL pointer 
// is returned and the current item is set to be beyond end of pList.
void* List_next(List* pList){
    assert(pList != NULL);
    if(pList->total_items == 0 || pList->beyondEnd){
        //checks for empty list and out of bounds
        pList->beyondEnd = true;
        pList->beforeStart = false;
        pList->List_current = NULL;
        return NULL;
    }
    else if(pList->beforeStart){
        //check for out of bounds
        pList->beyondEnd = false;
        return NULL;
    }
    else if(pList->List_current->arrayIndex == pList->List_tail->arrayIndex){
        //checking if current is the tail
        pList->List_current = NULL;
        pList->beyondEnd = true;
        pList->beforeStart = false;
        return NULL;
    }
    else{
        //condition if current is in the middle of the list
        pList->List_current = pList->List_current->next;
        return pList->List_current->item;
    }
    return NULL;
}

// Backs up pList's current item by one, and returns a pointer to the new current item. 
// If this operation backs up the current item beyond the start of the pList, a NULL pointer 
// is returned and the current item is set to be before the start of pList.
void* List_prev(List* pList){
    assert(pList != NULL);
    if(pList->total_items == 0 || pList->beforeStart){
        //checks for empty list and out of bounds
        pList->beyondEnd = false;
        pList->List_current = NULL;
        return NULL;
    }
    else if(pList->beyondEnd){
        //check out of bounds
        pList->beforeStart = false;
        return NULL;
    }
    else if(pList->List_current->arrayIndex == pList->List_head->arrayIndex){
        //checking if current is the head
        pList->List_current = NULL;
        pList->beforeStart = true;
        pList->beyondEnd = false;
        return NULL;
    }
    else{
        //condition if current is in the middle of the list
        pList->List_current = pList->List_current->previous;
        return pList->List_current->item;
    }
    return NULL;
}

// Returns a pointer to the current item in pList.
void* List_curr(List* pList){
    assert(pList != NULL);
    if(pList->beforeStart || pList->beyondEnd || pList->total_items == 0 ||pList->List_current == NULL){
        //checks for oob, empty lists and null current
        return NULL;
    }
    return pList -> List_current -> item;
}

// Adds the new item to pList directly after the current item, and makes item the current item. 
// If the current pointer is before the start of the pList, the item is added at the start. If 
// the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_add(List* pList, void* pItem){
    assert(pList != NULL);
    if(available_nodes < 1){
        //checking for available list nodes
        printf("Add Failed: no available nodes\n");
        return LIST_FAIL;
    }
    if(pList->total_items == 0 || pList->total_items == 1){
        int a = List_append(pList, pItem);
        pList->beyondEnd = false;
        pList->beforeStart = false;
        return a;
    }
    else if(pList->beforeStart){
        //oob check
        int app = List_prepend(pList, pItem);
        pList->beyondEnd = false;
        pList->beforeStart = false;
        return app;
    }
    else if(pList->beyondEnd || pList->List_current->arrayIndex == pList->List_tail->arrayIndex){
        //oob check and curr is tail check
        int pre = List_append(pList, pItem);
        pList->beyondEnd = false;
        pList->beforeStart = false;
        return pre;
    }
    else{
        //case where element is added to the middle of the list
        int index = LIST_MAX_NUM_NODES - available_nodes;
        int pos = arrIndex[index];
        node_array[pos].arrayIndex = pos;
        node_array[pos].item = pItem;
        node_array[pos].next = pList->List_current->next;
        node_array[pos].previous = pList->List_current;
        pList->List_current->next = &node_array[pos];
        pList->List_current = &node_array[pos];
        pList->List_current->next->previous = &node_array[pos];
        pList->total_items = pList->total_items + 1;
        available_nodes--;
        arrIndex[index] = -1;
        pList->beyondEnd = false;
        pList->beforeStart = false;
        printf("An element has been added\n");
        return LIST_SUCCESS;
    }
    return LIST_FAIL;
}

// Adds item to pList directly before the current item, and makes the new item the current one. 
// If the current pointer is before the start of the pList, the item is added at the start. 
// If the current pointer is beyond the end of the pList, the item is added at the end. 
// Returns 0 on success, -1 on failure.
int List_insert(List* pList, void* pItem){
    assert(pList != NULL);
    if(available_nodes < 1){
        //checking for available list nodes
        printf("Insert Failed: no available nodes\n");
        return LIST_FAIL;
    }
    if(pList->total_items == 0 || pList->total_items == 1){
        int x = List_prepend(pList, pItem);
        pList->beyondEnd = false;
        pList->beforeStart = false;
        return x;
    }
    else if(pList->beyondEnd){
        //oob check
        int pre = List_append(pList, pItem);
        pList->beyondEnd = false;
        pList->beforeStart = false;
        return pre;
    }
    else if(pList->beforeStart || pList->List_current->arrayIndex == pList->List_head->arrayIndex){
        //oob check and curr is tail check
        int app = List_prepend(pList, pItem);
        pList->beyondEnd = false;
        pList->beforeStart = false;
        return app;
    }
    else{
        //case where element is added to the middle of the list
        int index = LIST_MAX_NUM_NODES - available_nodes;
        int pos = arrIndex[index];
        node_array[pos].arrayIndex = pos;
        node_array[pos].item = pItem;
        node_array[pos].next = pList->List_current;
        node_array[pos].previous = pList->List_current->previous;
        pList->List_current->previous = &node_array[pos];
        pList->List_current = &node_array[pos];
        pList->List_current->previous->next = &node_array[pos];
        pList->total_items = pList->total_items + 1;
        available_nodes--;
        arrIndex[index] = -1;
        pList->beyondEnd = false;
        pList->beforeStart = false;
        printf("An element has been inserted\n");
        return LIST_SUCCESS;
    }
    return LIST_FAIL;
}

// Adds item to the end of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.
int List_append(List* pList, void* pItem){
    assert(pList != NULL);
    if(available_nodes < 1){
        //checking for available list nodes
        //printf("Append Failed: no available nodes\n");
        return LIST_FAIL;
    }
    else if(pList->total_items == 0){
        //appending to an empty list
        int index = LIST_MAX_NUM_NODES-available_nodes;
        int pos = arrIndex[index];
        node_array[pos].arrayIndex = pos;
        node_array[pos].item = pItem;
        node_array[pos].next = NULL;
        node_array[pos].previous = NULL;
        available_nodes--;
        pList->List_head = &node_array[pos];
        pList->List_tail = &node_array[pos];
        pList->List_current = &node_array[pos];
        pList->total_items = pList -> total_items + 1;
        arrIndex[index] = -1;
        pList->beforeStart = false;
        pList->beyondEnd = false;
        //printf("Node appended successfully\n");
        return LIST_SUCCESS;
    }
    else{
        //APPENDING TO A PREPOPULATED LIST
        int index = LIST_MAX_NUM_NODES-available_nodes;
        int pos = arrIndex[index];
        node_array[pos].arrayIndex = pos;
        node_array[pos].item = pItem;
        node_array[pos].next = NULL;
        node_array[pos].previous = pList->List_tail;
        available_nodes--;
        pList ->List_tail -> next = &node_array[pos];
        pList->List_tail = &node_array[pos];
        pList->List_current = &node_array[pos];
        pList->total_items = pList -> total_items + 1;
        arrIndex[index] = -1;
        //printf("Node appended successfully\n");
        pList->beforeStart = false;
        pList->beyondEnd = false;
        return LIST_SUCCESS;
    }
    return LIST_FAIL;
}

// Adds item to the front of pList, and makes the new item the current one. 
// Returns 0 on success, -1 on failure.

int List_prepend(List* pList, void* pItem){
    assert(pList != NULL);
    if(available_nodes < 1){
        //checking for available list nodes
        printf("Prepend failed: no available nodes\n");
        return LIST_FAIL;
    }
    if(pList -> total_items == 0){
        //prepending to an empty list
        int index = LIST_MAX_NUM_NODES-available_nodes;
        int pos = arrIndex[index];
        node_array[pos].arrayIndex = pos;
        node_array[pos].item = pItem;
        node_array[pos].next = NULL;
        node_array[pos].previous = NULL;
        available_nodes--;
        pList->List_head = &node_array[pos];
        pList->List_tail = &node_array[pos];
        pList->List_current = &node_array[pos];
        pList->total_items = pList -> total_items + 1;
        arrIndex[index] = -1;
        pList->beforeStart = false;
        pList->beyondEnd = false;
        printf("The node has been prepended\n");
        return LIST_SUCCESS;
    }
    else{
        //prepending to prepopulated list
        int index = LIST_MAX_NUM_NODES-available_nodes;
        int pos = arrIndex[index];
        node_array[pos].arrayIndex = pos;
        node_array[pos].item = pItem;
        node_array[pos].next = pList->List_head;
        node_array[pos].previous = NULL;
        available_nodes--;
        pList ->List_head -> previous = &node_array[pos];
        pList->List_head = &node_array[pos];
        pList->List_current = &node_array[pos];
        pList->total_items = pList -> total_items + 1;
        arrIndex[index] = -1;
        pList->beforeStart = false;
        pList->beyondEnd = false;
        printf("The node has been prepended\n");
        return LIST_SUCCESS;
    }
    return LIST_FAIL;
}

// Return current item and take it out of pList. Make the next item the current one.
// If the current pointer is before the start of the pList, or beyond the end of the pList,
// then do not change the pList and return NULL.
void* List_remove(List* pList){
    assert(pList!=NULL);
    if(pList->beyondEnd || pList->beforeStart){
        //OOB CHECK
        printf("Remove failed: List index out of bounds\n");
        return NULL;
    }
    else if(pList->total_items == 0){
        //empty list check
        printf("Remove failed: No items in the list\n");
        return NULL;
    }
    else if(pList->total_items == 1){
        // single element list check
        void* a = List_trim(pList);
        pList->beyondEnd = true;
        pList->beforeStart = false;
        return a;
    }
    else if(pList->List_head->arrayIndex == pList->List_current->arrayIndex){
        //removing from head checks
        int index = LIST_MAX_NUM_NODES - available_nodes;
        arrIndex[index - 1] = pList->List_current->arrayIndex;
        pList->List_head = pList->List_current->next;
        pList->List_current->next = NULL;
        pList->List_current = pList->List_head;
        pList->List_current->previous = NULL;
        pList->total_items = pList->total_items - 1;
        available_nodes++;
        void* item = node_array[arrIndex[index - 1]].item;
        node_array[arrIndex[index - 1]].item = NULL;
        pList->beyondEnd = false;
        pList->beforeStart = false;
        printf("The item has been removed\n");
        return item;
    }
    else if (pList->List_tail->arrayIndex == pList->List_current->arrayIndex){
        //removing from tail
        void* b = List_trim(pList);
        pList->beyondEnd = true;
        pList->beforeStart = false;
        pList->List_current = NULL;
        return b;
    }
    else{
        //removing from the middle
        void* c = pList->List_current->item;
        pList->List_current->previous->next = pList->List_current->next;
        pList->List_current->next->previous = pList->List_current->previous;
        int index = LIST_MAX_NUM_NODES - available_nodes;
        arrIndex[index - 1] = pList->List_current->arrayIndex;
        pList->List_current->item = NULL;
        pList->List_current = pList->List_current->next;
        available_nodes++;
        pList->total_items = pList->total_items - 1;
        node_array[arrIndex[index - 1]].next = NULL;
        node_array[arrIndex[index - 1]].previous = NULL;
        pList->beyondEnd = false;
        pList->beforeStart = false;
        printf("The item has been removed\n");
        return c;
    }
    
    
}

// Adds pList2 to the end of pList1. The current pointer is set to the current pointer of pList1. 
// pList2 no longer exists after the operation; its head is available
// for future operations.
void List_concat(List* pList1, List* pList2){
    assert(pList1 != NULL);
    assert(pList2 != NULL);
    if(pList1->total_items == 0 && pList2->total_items == 0 ){
        //both lists are empty
        int pos = LIST_MAX_NUM_HEADS - available_heads;
        headIndex[pos - 1] = pList2->arrayIndex;
        pList2->List_head = NULL;
        pList2->List_current = NULL;
        pList2->arrayIndex = -1;
        pList2->beforeStart = false;
        pList2->beyondEnd = false;
        pList2->List_tail = NULL;
        pList2->total_items = 0;
        pList2 = NULL;
        available_heads++;
        printf("The lists have been concatenated\n");
        return;
    }
    else if(pList1->total_items == 0){
        //only first list empty
        pList1->List_head = pList2->List_head;
        pList1->List_tail = pList2->List_tail;
        pList1->total_items = pList2->total_items;
        int pos = LIST_MAX_NUM_HEADS - available_heads;
        headIndex[pos - 1] = pList2->arrayIndex;
        pList2->List_head = NULL;
        pList2->List_current = NULL;
        pList2->arrayIndex = -1;
        pList2->beforeStart = false;
        pList2->beyondEnd = false;
        pList2->List_tail = NULL;
        pList2->total_items = 0;
        pList2 = NULL;
        available_heads++;
        printf("The lists have been concatenated\n");
        return;
    }
    else if(pList2->total_items == 0){
        //only second list empty
        int pos = LIST_MAX_NUM_HEADS - available_heads;
        headIndex[pos - 1] = pList2->arrayIndex;
        pList2->List_head = NULL;
        pList2->List_current = NULL;
        pList2->arrayIndex = -1;
        pList2->beforeStart = false;
        pList2->beyondEnd = false;
        pList2->List_tail = NULL;
        pList2->total_items = 0;
        pList2 = NULL;
        available_heads++;
        printf("The lists have been concatenated\n");
        return;
    }
    else{
        //both lists populated
        pList1->List_tail->next = pList2->List_head;
        pList2->List_head->previous = pList1->List_tail;
        pList1->List_tail = pList2->List_tail;
        pList1->total_items = pList1->total_items + pList2->total_items;
        int pos = LIST_MAX_NUM_HEADS - available_heads;
        headIndex[pos - 1] = pList2->arrayIndex;
        pList2->List_head = NULL;
        pList2->List_current = NULL;
        pList2->arrayIndex = -1;
        pList2->beforeStart = false;
        pList2->beyondEnd = false;
        pList2->List_tail = NULL;
        pList2->total_items = 0;
        pList2 = NULL;
        available_heads++;
        printf("The lists have been concatenated\n");
        return;
    }
    printf("List concatenation failed\n");
    return;
}

// Delete pList. pItemFreeFn is a pointer to a routine that frees an item. 
// It should be invoked (within List_free) as: (*pItemFreeFn)(itemToBeFreedFromNode);
// pList and all its nodes no longer exists after the operation; its head and nodes are 
// available for future operations.
typedef void (*FREE_FN)(void* pItem);
void List_free(List* pList, FREE_FN pItemFreeFn){
    assert(pList != NULL);
    if(pList->total_items == 0){
        //check if list is empty
        int index = LIST_MAX_NUM_HEADS - available_heads;
        headIndex[index - 1] = pList->arrayIndex;
        available_heads++;
        pList->arrayIndex = -1;
        pList->beforeStart = false;
        pList->beyondEnd = false;
        pList->List_current = NULL;
        pList->List_head = NULL;
        pList->List_tail = NULL;
        pList->total_items = 0;
        pList = NULL;
        printf("The list has been freed\n");
        return;
    }
    else{
        //freeing populated lists
        List_first(pList);
        int lindex;
        while(pList->total_items != 1){
            (* pItemFreeFn)(List_curr(pList));
            lindex = LIST_MAX_NUM_NODES - available_nodes;
            arrIndex[lindex - 1] = pList->List_current->arrayIndex;
            available_nodes++;
            pList->List_current->previous = NULL;
            pList->List_current->arrayIndex = -1;
            pList->List_current = pList->List_current->next;
            pList->List_current->previous->next = NULL;
            pList->List_current->previous = NULL;
            pList->total_items = pList->total_items - 1;
        }
        lindex = LIST_MAX_NUM_NODES - available_nodes;
        arrIndex[lindex - 1] = pList->List_current->arrayIndex;
        available_nodes++;
        (* pItemFreeFn)(List_curr(pList));
        pList->List_current->next = NULL;
        pList->List_current->previous = NULL;
        pList->List_current->arrayIndex = -1;   
        int index = LIST_MAX_NUM_HEADS - available_heads;
        headIndex[index - 1] = pList->arrayIndex;
        available_heads++;
        pList->arrayIndex = -1;
        pList->beforeStart = false;
        pList->beyondEnd = false;
        pList->List_current = NULL;
        pList->List_head = NULL;
        pList->List_tail = NULL;
        pList->total_items = 0;
        pList = NULL;
        printf("The list has been freed\n");
        return;
    }
    printf("List could not be freed\n");
    return;
}

// Return last item and take it out of pList. Make the new last item the current one.
// Return NULL if pList is initially empty.
void* List_trim(List* pList){
    assert(pList != NULL);
    void* pItem = pList->List_tail->item;
    if(pList->total_items == 0){
        //checking for available list nodes
        //printf("Trim failed: List is empty\n");
        return NULL;
    }
    else if(pList->total_items == 1){
        //checking for lists with only one node
        int pos = LIST_MAX_NUM_NODES - available_nodes;
        arrIndex[pos - 1] = pList->List_current->arrayIndex;
        pList->List_head->arrayIndex = -1;
        pList->List_head->item = NULL;
        pList->List_head->next = NULL;
        pList->List_head->previous = NULL;
        pList->List_current = NULL;
        pList->List_head = NULL;
        pList->List_tail = NULL;
        pList->total_items = 0;
        available_nodes++;
        //printf("The list has been trimmed\n");
        return pItem;
    }
    else{   
        //removing from list with more than one node
        int pos = LIST_MAX_NUM_NODES - available_nodes;
        arrIndex[pos - 1] = pList->List_tail->arrayIndex;
        pList->List_tail = pList->List_tail->previous;
        pList->List_tail->next->previous = NULL;
        pList->List_tail->next->arrayIndex = -1;
        pList->List_tail->next->item = NULL;
        pList->List_tail->next->next = NULL;
        pList->List_tail->next = NULL;
        pList->List_current = pList->List_tail;
        pList->total_items = pList->total_items - 1;
        pList->beforeStart = false;
        pList->beyondEnd = false;
        available_nodes++;
        //printf("The list has been trimmed\n");
        return pItem;
    }
    printf("The list trim failed\n");
}

// Search pList, starting at the current item, until the end is reached or a match is found. 
// In this context, a match is determined by the comparator parameter. This parameter is a
// pointer to a routine that takes as its first argument an item pointer, and as its second 
// argument pComparisonArg. Comparator returns 0 if the item and comparisonArg don't match, 
// or 1 if they do. Exactly what constitutes a match is up to the implementor of comparator. 
// 
// If a match is found, the current pointer is left at the matched item and the pointer to 
// that item is returned. If no match is found, the current pointer is left beyond the end of 
// the list and a NULL pointer is returned.
// 
// If the current pointer is before the start of the pList, then start searching from
// the first node in the list (if any).
typedef bool (*COMPARATOR_FN)(void* pItem, void* pComparisonArg);
void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){
    assert(pList != NULL);
    if (pList->total_items == 0){
        return NULL;
    }
    else if(pList->beyondEnd){
        return NULL;
    }
    else if (pList->beforeStart){
        pList->List_current = pList->List_head;
        pList->beforeStart = false;
        while(pList->List_current->arrayIndex != pList->List_tail->arrayIndex){
            if((* pComparator)(List_curr(pList), pComparisonArg)){
                return List_curr(pList);
            }
            pList->List_current = pList->List_current->next;
        }
        if((* pComparator)(pList->List_tail->item, pComparisonArg)){
            pList->List_current = pList->List_tail;
            return List_curr(pList);
        }
        pList->List_current = NULL;
        pList->beyondEnd = true;
        return NULL;
    }
    else{
        pList->beforeStart = false;
        while(pList->List_current->arrayIndex != pList->List_tail->arrayIndex){
            if((* pComparator)(List_curr(pList), pComparisonArg)){
                return List_curr(pList);
            }
            pList->List_current = pList->List_current->next;
        }
        if((* pComparator)(List_curr(pList), pComparisonArg)){
            pList->List_current = pList->List_tail;
            return List_curr(pList);
        }
        pList->List_current = NULL;
        pList->beyondEnd = true;
        return NULL;
    }
}