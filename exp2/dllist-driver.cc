// dllist-dirver.cc
//  Provide 2 functions to help operate doubly-linked list.
//
//  You can specify the list and the number of items.
//
// Copyright (c) 2020 Marx Young. All rights reserved.

#include "dllist.h"
#include "system.h"

//----------------------------------------------------------------------
// GenerateN
// 	Generates N items with random keys and inserts them into a
//  doubly-linked list.
//----------------------------------------------------------------------

void
GenerateN(int N, DLList *list) {
    while (N--) {
        int key = Random() % 2001 - 1000;   // here we limit the range
                                // of random numbers to [ -1000, 1000 ]
                                // just for the convenience of demonstration
        int item = Random();
        list->SortedInsert(&item, key);
        list->Show();
        printf("Insert an item which key is %d\n", key);
    }
}

//----------------------------------------------------------------------
// RemoveN
// 	Remove N items starting from the head of the list
//  and prints out the removed items to the console.
//----------------------------------------------------------------------

void
RemoveN(int N, DLList *list) {
    int key;
    int *item_ptr;
    while (N--) {
        item_ptr = (int *)list->Remove();
        key = *item_ptr;
        list->Show();
        if (item_ptr) {
            printf("Remove an item which key is %d\n", key);
        } else {
            printf("List is empty!\n");
        }
    }
}

