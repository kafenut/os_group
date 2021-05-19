// threadtest.cc
//  Simple test case for the threads assignment.
//
//  Create two threads, and have them context switch
//  back and forth between themselves by calling Thread::Yield,
//  to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "dllist.h"

extern void GenerateN(int N, DLList *list);
extern void RemoveN(int N, DLList *list);

// testnum is set in main.cc
int testnum = 1;
int T, N, E;
DLList *list;


//----------------------------------------------------------------------
// SimpleThread
//  Loop 5 times, yielding the CPU to another ready thread
//  each iteration.
//
//  "which" is simply a number identifying the thread, for debugging
//  purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;

    for (num = 0; num < 5; num++) {
    printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ConcurrentError1
//  insert N items -- switch threads -- remove N items -- switch threads
// Error phenomenon
//  thread may take out items that do not belong to itself
//----------------------------------------------------------------------

void
ConcurrentError1(int which)
{
    printf("*** thread %d\n", which);
    GenerateN(N, list);
    currentThread->Yield();
    printf("*** thread %d\n", which);
    RemoveN(N, list);
    currentThread->Yield();
}

void
ConcurrentError2(int which)
{
    int key[] = { 3,2,1,4,5,6};
    int item[] = {1,2,3,4,5,6};
    int i = 0;
    while (++i < 4) {
        printf("*** thread %d is going to insert an item with key: %d\n",
                                            which, key[(i - 1) * 2 + which]);
        list->SortedInsert(&item[(i - 1) * 2 + which], key[(i - 1) * 2 + which]);
        list->Show();
        currentThread->Yield();
    }
    printf("*** thread %d\n", which);
    RemoveN(3, list);
}

void
ConcurrentError3(int which)
{
    int key[] = {4,5,3,8,9,10};
    int item[] = {11,22,33,44,55,66};
    int i = 0;
    while (++i < 4) {
        printf("*** thread %d is going to insert an item with key: %d\n",
                                            which, key[(i - 1) * 2 + which]);
        list->SortedInsert(&item[(i - 1) * 2 + which], key[(i - 1) * 2 + which]);
        list->Show();
        currentThread->Yield();
    }
    //printf("*** thread %d\n", which);
    //currentThread->Yield();
    printf("*** thread %d\n", which);
    RemoveN(3, list);
}
void
ConcurrentError4(int which)
{
    int item[] = {0,0,0,0,0,0};
    int i = 0;
	if(which == 0)
	{
		printf("*** thread %d is going to insert the first element key = 10 \n",which);
		list->SortedInsert(&item[i], 10);
		list->Show();
		currentThread->Yield();
	    while (i++ < 3)
		{
			printf("***1 thread %d\n",which);
			list->Prepend(&item[i]);
			list->Show();
		}
		currentThread->Yield();
		while (i++ <=6)
		{
			printf("***1 thread %d\n",which);
			list->Prepend(&item[i]);
			list->Show();
		}
	}
	else
	{
		printf("*** thread %d remove\n",which);
		list->Remove();
		list->Show();
		currentThread->Yield();
	}

    //printf("*** thread %d\n", which);
    //currentThread->Yield();
    //printf("*** thread %d\n", which);
}
const int error_num = 4;    // total number of concurrent errors
typedef void (*func) (int);
func ConcurrentErrors[error_num] = {ConcurrentError1, ConcurrentError2, ConcurrentError3,ConcurrentError4};
                                    //ConcurrentError4, ConcurrentError5, ConcurrentError6};

//----------------------------------------------------------------------
// ThreadTest1
//  Set up a ping-pong between two threads, by forking a thread
//  to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
//ThreadTest2
//
//----------------------------------------------------------------------

void
ThreadTest2()
{
    DEBUG('t', "Entering ThreadTest2");

    // problem with parameter E
    if (E > error_num || E < 1) {
        printf("No concurrent error specified.\n");
        return;
    }

    list = new DLList(E);
    int i;

    for (i = 1; i < T; i++) {
        Thread *t = new Thread("forked thread");
        t->Fork(ConcurrentErrors[E - 1], i);
    }
    ConcurrentErrors[E - 1](0);
}

//----------------------------------------------------------------------
// ThreadTest
//  Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest(int t, int n, int e)
{
    switch (testnum) {
    case 1:
    ThreadTest1();
    break;
    case 2:
    T = t;
    N = n;
    E = e;
    ThreadTest2();
    break;
    default:
    printf("No test specified.\n");
    break;
    }
}

