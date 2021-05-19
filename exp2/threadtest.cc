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
#include "synch.h"
#include "Table.h"
#include "BoundedBuffer.h"

extern void GenerateN(int N, DLList *list);
extern void RemoveN(int N, DLList *list);

// testnum is set in main.cc
int testnum = 1;
int T, N, E;
DLList *list;
Lock *lock;
Condition *cond;
BoundedBuffer *buffer;
Table *table;

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


void
SynThread(int which)
{
    printf("*** thread %d is to Wait\n", which);
    lock->Acquire();
    cond->Wait(lock);
    lock->Release();
    printf("*** thread %d is awakened\n", which);
}


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

    lock = new Lock("ThreadTest2");
    cond = new Condition("ThreadTest2");

    // problem with parameter E
    if (E > error_num || E < 1) {
        printf("No concurrent error specified.\n");
        return;
    } else if (E == 5 || E == 6) {
        printf("To better demonstrate the concurrency error, we use the "
            "set key value here, and do not support customizing the "
            "number of insert elements and threads.\n");
        T = 2;  // to better demonstrate the concurrency error
    }

    list = new DLList(E);
    int i;

    for (i = 0; i < T; i++) {
        Thread *t = new Thread("forked thread");
        t->Fork(ConcurrentErrors[E - 1], i);
        currentThread->Yield();
    }
    lock->Acquire();
    cond->Signal(lock);
    lock->Release();
}

void ThreadTest3()
{
    DEBUG('t', "Entering ThreadTest2");

    lock = new Lock("ThreadTest3");
    cond = new Condition("ThreadTest3");
    // verify that a Signal cannot affect a subsequent Wait
    lock->Acquire();
    cond->Signal(lock);
    lock->Release();
    printf("*** thread 0 Signal\n");

    // these threads will wait for cond
    for (int i = 1; i < 5; i++) {
        Thread *t = new Thread("forked thread");
        t->Fork(SynThread, i);
        currentThread->Yield();
    }

    printf("*** thread 0 Signal\n");
    lock->Acquire();
    cond->Signal(lock);
    lock->Release();

    currentThread->Yield();

    printf("*** thread 0 BroadCast\n");
    lock->Acquire();
    cond->Broadcast(lock);
    lock->Release();
}

//----------------------------------------------------------------------
//TableActions
//	Create a table according to parameter N.
//  Store, get and release elements while switching threads.
//----------------------------------------------------------------------
void
TableActions(int which)
{
    int indexArr[N];
    //
    for(int i =0; i < N; i++) {
        void *obj = (void *)(Random() % 1000);
        indexArr[i] = table->Alloc(obj);
        printf("*** thread %d stores %d at [%d] ***\n", which, (int)obj, indexArr[i]);
        currentThread->Yield();
    }
    //
    for(int i =0; i < N; i++) {
        printf("*** thread %d gets %d from [%d] ***\n", which, (int)table->Get(indexArr[i]), indexArr[i]);
        currentThread->Yield();
    }
    //
    for(int i =0; i < N; i++) {
        table->Release(indexArr[i]);
        printf("*** thread %d released [%d] ***\n", which, indexArr[i]);
        currentThread->Yield();
    }
    printf("*** thread %d finished ***\n", which);
}

//----------------------------------------------------------------------
//TableTest
//	T = number of threads, N = number of obj for allocation each threads.
//  Create a shared table with the size of T * N.
//  Fork threads to invoke TableActions.
//----------------------------------------------------------------------
void
TableTest()
{
    DEBUG('t', "Entering TableTest");

    int maxTableSize = T * N;
    table = new Table(maxTableSize);
    for(int i = 0; i < T; i ++) {
        Thread *t = new Thread("forked thread");
        t->Fork(TableActions, i);
        currentThread->Yield();
    }
}
//----------------------------------------------------------------------
//WriteBuffer
//	Create an pointer named 'data' that points to an area with
//'num' pieces of data and write these data to the buffer.
//----------------------------------------------------------------------

void
WriteBuffer(int num)
{
    printf("\nCurrent thread is %s\n", currentThread->getName());
    char* data = new char;
    int i;
    for (i = 0; i < num; i++)
    {
        *(data + i) = (Random() % 26) + 65;
    }
    printf("%s will write these data to buffer:\n", currentThread->getName());
    for (i = 0; i < num - 1; i++)
        printf("%c ", *(data + i));
    printf("%c\n", *(data + i));

    buffer->Write((void*)data, num);

    printf("%s finished writing.\n", currentThread->getName());
    buffer->Showbuffer();
}

//----------------------------------------------------------------------
//ReadBuffer
//	Read 'num' bytes of data from buffer to the area at the
//beginning of '* data'
//----------------------------------------------------------------------

void
ReadBuffer(int num)
{
    printf("\nCurrent thread is read thread :%s\n", currentThread->getName());
    char* data = new char;
    buffer->Read((void*)data, num);
    printf("%s finished,read these data from buffer:", currentThread->getName());
    int i;
    for (i = 0; i < num - 1; i++)
        printf("%c ", *(data + i));
    printf("%c\n", *(data + i));
    buffer->Showbuffer();
}


//----------------------------------------------------------------------
//BufferTest
// 	Invoke a buffer test routine.In this test routine,we create
//       some read thread and wtire thread to test whether current
//       buffer is thread-safe.
//       Some Params:
//	T:maxsize of boundedbuffer
//  N:num of read threads(read from buffer)
//	E:num of write threads(write to buffer)
//	num1:num of read bytes
//  num2:num of write bytes
//----------------------------------------------------------------------
void
BufferTest()
{
     int num1, num2, i;
     DEBUG('t', "Entering BufferTest");
     buffer = new BoundedBuffer(T);
     printf("\nEnter read bytes:");
     scanf("%d", &num1);
     printf("\nEnter write bytes:");
     scanf("%d", &num2);
     printf("\n");
    int k, count1 = 0, count2 = 0;
    //Use random number to decide create
    //a read thread or a write thread
    for(i = 0; i < N + E; i++)
    {
        char *str = new char[20];
        k = Random() % 2;
        if (k == 1)
        {
            if (count1 < N)
            {
                sprintf(str, "ReadThread %d", count1);
                Thread * t = new Thread(str);
                t -> Fork(ReadBuffer, num1);
                count1++;
            }
            else
            {
                sprintf(str, "WriteThread %d", count2);
                Thread * t = new Thread(str);
                t -> Fork(WriteBuffer, num2);
                count2++;
            }
        }
        else
        {
            if (count2 < E)
            {
                sprintf(str, "WriteThread %d", count2);
                Thread * t = new Thread(str);
                t -> Fork(WriteBuffer, num2);
                count2++;
            }
            else
            {
                sprintf(str, "ReadThread %d", count1);
                Thread * t = new Thread(str);
                t -> Fork(ReadBuffer, num1);
                count1++;
            }
        }
    }
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
    case 3:
        ThreadTest3();
        break;
    case 4:
        T = t;
        N = n;
        TableTest();
        break;
    case 6:
        T = t;
        N = n;
        E = e;
        BufferTest();
        break;
    default:
    printf("No test specified.\n");
    break;
    }
}

