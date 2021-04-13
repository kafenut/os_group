#include<stdio.h>
#include<stdlib.h>
#include"dllist.h"
#include "system.h"

void GenerateN(int N, DLList *list) {
    while (N--) {
        int key = rand() % 2001 - 1000;   // here we limit the range
                                // of random numbers to [ -1000, 1000 ]
                                // just for the convenience of demonstration
        int item = rand();
        list->SortedInsert(&item, key);
        list->Show();
        printf("Insert an item which key is %d\n", key);
    }
}

void RemoveN(int N, DLList *list) {
    int* key_ptr;
    while (N--) {
        key_ptr = (int *)list->Remove();    
        if (key_ptr) {
            printf("Remove an item which key is %d\n", *key_ptr);
        } else {
            printf("List is empty!\n");
        }
        list->Show();
    }
}

// int main()
// {
// 	int N = 10;
// 	int i;
// 	int s = 0, key;
// 	int* item_ptr;
// 	DLList pt;
// 	DLList* test=&pt;

// 	GenerateN(N,test);
// 	RemoveN(N,test);
// }
