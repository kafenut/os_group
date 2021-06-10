// threadtest.cc
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield,
//	to illustrate the inner workings of the thread system.
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
#include "EventBarrier.h"
#include "Elevator.h"
<<<<<<< HEAD
#define STOP_TIME 1000000
=======
#define STOP_TIME 800000
>>>>>>> 7a95b12f52005db4f4e1ab4f6c13ba450bd42216

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
EventBarrier *barrier;
int cnt = 0;
Building *building;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
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
// 	insert N items -- switch threads -- remove N items -- switch threads
// Error phenomenon
//  thread may take out items that do not belong to itself
//----------------------------------------------------------------------

void
ConcurrentError1(int which)
{
    lock->Acquire();    // Here we consider a situation that we want to
    cond->Wait(lock);   // remove the items just inserted, so we nedd to
    lock->Release();    // enforce mutual exclusive in addtion to inside's
    printf("*** thread %d\n", which);
    GenerateN(N, list);
    currentThread->Yield();
    printf("*** thread %d\n", which);
    RemoveN(N, list);
    lock->Acquire();
    cond->Signal(lock);
    lock->Release();
    currentThread->Yield();
}

//----------------------------------------------------------------------
// ConcurrentError2
// 	switch threads before setting list->first when inserting an item
//  into an empty list
// Error phenomenon
//  list will lose some items
//----------------------------------------------------------------------

void
ConcurrentError2(int which)
{
    printf("*** thread %d\n", which);  // Suppose that the thread cooperates with others
    GenerateN(N, list);    // No need to keep mutual exclusion until removing
    currentThread->Yield();
    printf("*** thread %d\n", which);
    RemoveN(N, list);
}

//----------------------------------------------------------------------
// ConcurrentError3
// 	switch threads after setting list->first when inserting an item
//  into an empty list
// Error phenomenon
//  segment fault
//----------------------------------------------------------------------

void
ConcurrentError3(int which)
{
    int key[] = {1,2};  // 2rd thread's item's key > 1st thread's item's key
                        // so that a segment fault will occur in
                        // "else if (sortKey >= last->key)"(DLList::SortedInsert)
    int item[] = {11,22};
    printf("*** thread %d is going to insert an item with key: %d\n", which, key[which % 2]);
    list->SortedInsert(&item[which % 2], key[which % 2]);
    printf("*** thread %d\n", which);
    RemoveN(1, list);
}

//----------------------------------------------------------------------
// ConcurrentError4
//  switch threads after setting element to first
// Error phenomenon
//  memory access error
//----------------------------------------------------------------------

void
ConcurrentError4(int which)
{
    ConcurrentError2(which);
}

//----------------------------------------------------------------------
// ConcurrentError5
//  switch threads before setting first/last to element(in SortedInsert)
// Error phenomenon
//  list will lose some items
//----------------------------------------------------------------------
void
ConcurrentError5(int which)
{
    int key[] = {5,4,3,8,9,10};
    int item[] = {11,22,33,44,55,66};
    int i = 0;
    while (++i < 4) {
        printf("*** thread %d is going to insert an item with key: %d\n",
                                            which, key[(i - 1) * 2 + which]);
        list->SortedInsert(&item[(i - 1) * 2 + which], key[(i - 1) * 2 + which]);
        list->PrintList();
        currentThread->Yield();
    }
    //printf("*** thread %d\n", which);
    //currentThread->Yield();
    printf("*** thread %d\n", which);
    RemoveN(3, list);
}

//----------------------------------------------------------------------
// ConcurrentError6
//  switch threads after finding the insertion position(in SortedInsert)
// Error phenomenon
//  some items are out of order in the list
//----------------------------------------------------------------------
void
ConcurrentError6(int which)
{
    int key[] = {1,100,70,60,50,40};
    int item[] = {11,22,33,44,55,66};
    int i = 0;
    while (++i < 4) {
        printf("*** thread %d is going to insert an item with key: %d\n",
                                            which, key[(i - 1) * 2 + which]);
        list->SortedInsert(&item[(i - 1) * 2 + which], key[(i - 1) * 2 + which]);
        list->PrintList();
        currentThread->Yield();
    }
    //currentThread->Yield();
    printf("*** thread %d\n", which);
    RemoveN(3, list);
}

const int error_num = 6;    // total number of concurrent errors
typedef void (*func) (int);
func ConcurrentErrors[error_num] = {ConcurrentError1, ConcurrentError2, ConcurrentError3,
                                    ConcurrentError4, ConcurrentError5, ConcurrentError6};

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
// 	Set up a ping-pong between two threads, by forking a thread
//	to call SimpleThread, and then calling SimpleThread ourselves.
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
// ThreadTest2
//  Demonstrate concurrency errors.
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
        void *obj = (void *)(Random() % 100);
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
    printf("\nCurrent thread is write thread :%s\n", currentThread -> getName());
    int data[num];
    int i;
    for(i = 0; i<num; i++)
    {
	    *(data + i) = Random() % 100;
    }
    printf("%s will write these data to buffer:", currentThread -> getName());
    for(i = 0;i < num-1;i++)
        printf("%d ",*(data + i));
    printf("%d\n",*(data + i));
    buffer->Write((void *)data,num);
    printf("%s finished,",currentThread -> getName());
    buffer->PrintBuffer();
}

//----------------------------------------------------------------------
//ReadBuffer
//	Read 'num' bytes of data from buffer to the area at the
//beginning of '* data'
//----------------------------------------------------------------------

void
ReadBuffer(int num)
{
    printf("\nCurrent thread is read thread :%s\n",currentThread -> getName());
    int data[num + 1];
    buffer -> Read((void *)data,num);
    printf("%s finished,read these data from buffer:",currentThread -> getName());
    int i;
    for (i = 0;i<num-1;i++)
	printf("%d ",*(data + i));
    printf("%d\n",*(data + i));
    buffer -> PrintBuffer();
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
// EventBarrierSignalThread
//  Signal for specified EventBarrier.
//----------------------------------------------------------------------

void
EventBarrierSignalThread(int which)
{
    barrier->Signal();
}

//----------------------------------------------------------------------
// EventBarrierWaitThread1
//  Wait on specified EventBarrier and wait for other waiters
//  before Complete.
//----------------------------------------------------------------------

void
EventBarrierWaitThread1(int which)
{
    barrier->Wait();
    lock->Acquire();
    cnt++;
    cond->Wait(lock);   // wait for all waiters arrive and then response
    lock->Release();
    barrier->Complete();
}

//----------------------------------------------------------------------
// EventBarrierWaitThread2
//  Wait on specified EventBarrier and call Wait again immediately
//  after returning from Complete.
//----------------------------------------------------------------------

void
EventBarrierWaitThread2(int which)
{
    barrier->Wait();
    barrier->Complete();

    barrier->Wait();
    barrier->Complete();
}

//----------------------------------------------------------------------
// EventBarrierTest
//  A test routine for EventBarrier.
//----------------------------------------------------------------------
void
EventBarrierTest(int part)
{
    barrier = new EventBarrier("EventBarrierTest");
    lock = new Lock("EventBarrierTest");
    cond = new Condition("EventBarrierTest");
    Thread *t;
    char *threadName[3];

    switch (part)
    {
    case 1:
        // Part Ⅰ
        printf("PART I: At first, a thread will Signal. And then three thread "
            "will Wait. Previous threads won't Complete until all 3 threads arrive."
            "(This is the setting in this test, not EventBarrier's feature.)"
            "This part is to show that a signal without waiter could be recorded "
            "and the EventBarrier keeps in signaled state before all waiters response.\n");

        t = new Thread("thread 0 (signal thread)");
        t->Fork(EventBarrierSignalThread, 0);
        currentThread->Yield();

        for (int i = 1; i < 4; i++)
        {
            threadName[i - 1] = new char[30];
            sprintf(threadName[i - 1], "thread %d (wait   thread)", i);
            t = new Thread(threadName[i - 1]);
            t->Fork(EventBarrierWaitThread1, i);
            currentThread->Yield();
        }

        while (cnt < 3)
        {
            currentThread->Yield();
        }

        lock->Acquire();
        cond->Broadcast(lock);
        lock->Release();
        break;
    case 2:
        // Part Ⅱ
        printf("\n\nPART II: At first, a thread will Wait. And then another "
            "two threads will Signal.(At the same time, only one thread can "
            "signal and others will block.) The wait thread call Wait again "
            "immediately after returning from Complete. It'll pass the "
            "EventBarrier twice with different threads. This part is to show"
            "the case where threads call Wait again immediately after returning"
            " from Complete and the mutual exclusion on Signal.\n");
        t = new Thread("thread 0 (wait   thread)");
        t->Fork(EventBarrierWaitThread2, 0);
        currentThread->Yield();

        t = new Thread("thread 1 (signal thread)");
        t->Fork(EventBarrierSignalThread, 1);

        t = new Thread("thread 2 (signal thread)");
        t->Fork(EventBarrierSignalThread, 2);
        break;
    default:
        break;
    }
}

//----------------------------------------------------------------------
// AlarmActions
//  Set a random time for a thread to sleep.
//----------------------------------------------------------------------
void
AlarmActions(int which)
{
    int howLong =  Random() % 100 * 10 * (which + 1);
    alarms -> Pause(howLong);
    printf("*** thread%d finished AlarmTest ***\n", which);
    // printf("*** thread%d woke up at %d (current ticks) ***\n", which, stats->totalTicks);
}

//----------------------------------------------------------------------
// AlarmTest
//  A test routine for Alarm. t stands for number of threads.
//----------------------------------------------------------------------
void
AlarmTest(int t)
{
    DEBUG('t', "Entering AlarmTest");
    printf("Attention: time of thread going to sleep + duration set for sleep may not equal time of thread woke up due to multiple reasons.\n");
    for(int i = 0; i < t; i ++) {
        char *threadName = new char[10];
		sprintf(threadName, "thread%d", i);
		Thread *t = new Thread(threadName);
		t->Fork(AlarmActions, i);
    }
}




//----------------------------------------------------------------------
// ElevatorThread
//   Using this thread to control the operation of the elevator and
// change the state of the elevator.
//   When elevatorState is UP or DOWN, the elevator moves in only one
// direction, finding the furthest request in the current direction each
// time it gets the request, and satisfying all outstanding requests
// on the way.
//----------------------------------------------------------------------
void ElevatorThread(int which)
{
    Elevator *e = building->elevator;
    printf("Elevator start.\n");
    while(1)
    {
        int dstfloor = e->getRequest();
        if(e->elevatorState == STAY)
        {
            e->ElevatorLock->Acquire();
            printf("** %2d Floor! [STOP]**\n",e->currentfloor);
            if(dstfloor == -1)//If no request,thread blocks itself until it is awakened
            {
                e->HaveRequest->Wait(e->ElevatorLock);
            }
            e->ElevatorLock->Release();
            printf("** %2d Floor! %2d Riders**\n",e->currentfloor, e->occupancy);
            dstfloor = e->getRequest();//Decide whether to change the status of the elevator by dst's value
            if(dstfloor > e->currentfloor)
                e->elevatorState = UP;
            else
                e->elevatorState = DOWN;
        }
        else if(e->elevatorState == UP)
        {
            while(e->currentfloor != dstfloor)
            {
                if(e->isOut[e->currentfloor] || e->isUp[e->currentfloor])
                {
                    e->OpenDoors();
                    e->CloseDoors();
                }

                e->VisitFloor(e->goUp());
                printf("** %2d Floor! %2d Riders**\n",e->currentfloor, e->occupancy);
                dstfloor = e->getRequest();
            }
            if(!e->isUp[e->currentfloor])//if the request on dstFloor is not up,elevator turn around
            {
                e->elevatorState = DOWN;
                if(e->isDown[e->currentfloor] || e->isOut[e->currentfloor])
                {
                    e->OpenDoors();
                    e->CloseDoors();
                }
                dstfloor = e->getRequest();
                if(dstfloor == -1)
                    e->elevatorState = STAY;
                else
                    e->VisitFloor(e->goDown());
            }
            else//if the request on dstFloor is up
            {
                e->OpenDoors();
                e->CloseDoors();
                e->VisitFloor(e->goUp());
            }
            printf("** %2d Floor! %2d Riders**\n",e->currentfloor, e->occupancy);
        }
        else//When elevatorstate is DOWN, it's the opposite of when it's UP.
        {
             while(e->currentfloor != dstfloor)
            {
                if(e->isOut[e->currentfloor] || e->isDown[e->currentfloor])
                {
                    e->OpenDoors();
                    e->CloseDoors();
                }

                e->VisitFloor(e->goDown());
                printf("** %2d Floor! %2d Riders**\n",e->currentfloor, e->occupancy);
                dstfloor = e->getRequest();
            }
            if(!e->isDown[e->currentfloor])
            {
                e->elevatorState = UP;
                if(e->isUp[e->currentfloor] || e->isOut[e->currentfloor])
                {
                    e->OpenDoors();
                    e->CloseDoors();
                }
                dstfloor = e->getRequest();
                if(dstfloor == -1)
                    e->elevatorState = STAY;
                else
                    e->VisitFloor(e->goUp());
            }
            else
            {
                e->OpenDoors();
                e->CloseDoors();
                e->VisitFloor(e->goDown());
            }
            printf("** %2d Floor! %2d Riders**\n",e->currentfloor, e->occupancy);
        }
    }
}



//----------------------------------------------------------------------
// riderTest
//   Create a passenger thread and randomly generate the start floor and
// destination floor to simulate the passenger request.
//----------------------------------------------------------------------
void riderTest(int id)//It's almost the same as the given example rider thread
{
    while(true)
    {
    int srcFloor = (Random() % (building->elevator->topFloor)) + 1;
    int dstFloor = (Random() % (building->elevator->topFloor)) + 1;
    Elevator *e;
    if(srcFloor == dstFloor)
        return;
    printf("---REQUEST!---Rider %2d : from %d floor to %2d floor\n", id , srcFloor , dstFloor);
    do{
        if(srcFloor < dstFloor)
        {
            building->CallUp(srcFloor);
            e = building->AwaitUp(srcFloor);
        }
        else
        {
            building->CallDown(srcFloor);
            e = building->AwaitDown(srcFloor);
        }
    }while(!e->Enter());
    printf("---ENTER!---Rider %2d request to go to %2d floor\n",id, dstFloor);
    e->RequestFloor(dstFloor);
    printf("---LEAVE!---Rider %2d leave from %2d floor\n",id, dstFloor);
    e->Exit();
    alarms->Pause(STOP_TIME);
    }
}



//----------------------------------------------------------------------
// ElevatorTest
//   This function creates an instance of Building and generates an
// elevator control thread and several passenger threads as required.
//----------------------------------------------------------------------
void ElevatorTest(int floornum, int ridernum,int capacity)
{
    printf("---------Elevator Test--------\n");
    char **threadName;
    threadName = new char*[ridernum];
    Thread *t;
    building = new Building("building", floornum, 1);
    if(capacity > 0)
        building->elevator->capacity = capacity;
    else
    {
        printf("-----------------------------------------------\n");
        printf("The capacity of elevator cannot be less than 0!\n");
        printf("-----------------------------------------------\n");
        return;
    }
    if(floornum > 1)
        building->elevator->topFloor = floornum;
    else
    {
        printf("-----------------------------------------------\n");
        printf("The floornum of elevator cannot be less than 1!\n");
        printf("-----------------------------------------------\n");
        return;
    }
    printf("-------------------- ---\n");
    printf("Elevator's Capacity: %2d\n",building->elevator->capacity);
    printf("Elevator's Floornum: %2d\n",building->elevator->topFloor);
    printf("---------------------- -\n");
    t = new Thread("thread 0 (Elevator thread)");
    t->Fork(ElevatorThread, 0);
    for (int i = 1; i <= ridernum; i++)
    {
        threadName[i - 1] = new char[30];
        sprintf(threadName[i - 1], "thread %d (rider   thread)", i);
        t = new Thread(threadName[i - 1]);
        t->Fork(riderTest, i);
        //currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
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
    // test Lock and Condition
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
    case 7:
        EventBarrierTest(t);
        break;
    case 8:
        AlarmTest(t);
        break;
    case 9:
        ElevatorTest(t,n,e);
        break;
    default:
        printf("No test specified.\n");
        break;
    }
}
