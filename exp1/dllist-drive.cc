#include<stdio.h>
#include<stdlib.h>

void GenerateN(int N, DLList *list) {
    while (N--) {
        int key = rand() % 2001 - 1000; // here we limit the range
                                        // of random numbers to [ -1000, 1000 ]
                                        // just for the convenience of demonstration
        int item = rand();
        list->SortedInsert(&item, key);
        list->Show();
        printf("Insert an item which key is %d\n", key);
    }
}

void RemoveN(int N, DLList *list) {
    int key;
    int *item_ptr;
    while (N--) {
        item_ptr = (int *)list->Remove();
        list->Show();
        if (item_ptr) {
            printf("Remove an item which key is %d\n", key);
        } else {
            printf("List is empty!\n");
        }
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