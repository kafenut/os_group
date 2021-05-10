#include "synch-sleep.h"
#include "system.h"

Lock::Lock(char* debugName)
{
    name = debugName;
    isBusy = false; //lock is free
    queue = new List;
    owner = NULL;
}

bool Lock::isHeldByCurrentThread()
{
    if (owner == currentThread && isBusy)
        return true;
    else
        return false;
}

void Lock::Acquire()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff); // disable interrupts
    while(isBusy) //if Lock is busy
    {
        queue->Append((void *) currentThread);  //so go to sleep
        currentThread->Sleep();
    }
    isBusy = true;  //set the Lock to busy
    owner = currentThread;

    (void) interrupt->SetLevel(oldLevel);   // re-enable interrupts
}

void Lock::Release()
{
    Thread* thread;
    ASSERT(isHeldByCurrentThread()); //ensure current thread
    //is the owner of the Lock
    IntStatus oldLevel = interrupt->SetLevel(IntOff); // disable interrupts
    thread = (Thread *)queue->Remove();
    if(thread != NULL)  //wake up a thread
    {
        scheduler->ReadyToRun(thread);
    }
    isBusy = false;
    owner = NULL;

    (void) interrupt->SetLevel(oldLevel);   // re-enable interrupts
}

Condition::Condition(char* debugName)
{
    name = debugName;
    queue = new List;
}

Condition::~Condition()
{
    delete queue;
}

void Condition::Wait(Lock* conditionLock)
{
    ASSERT(conditionLock->isHeldByCurrentThread());
    IntStatus oldLevel = interrupt->SetLevel(IntOff); // disable interrupts
    conditionLock->Release();
    queue->Append((void *)currentThread);
    currentThread->Sleep();
    conditionLock->Acquire();

    (void) interrupt->SetLevel(oldLevel);   // re-enable interrupts
}

void Condition::Signal(Lock* conditionLock)
{
    Thread* thread;
    ASSERT(conditionLock->isHeldByCurrentThread());
    IntStatus oldLevel = interrupt->SetLevel(IntOff); // disable interrupts
    thread = (Thread *)queue->Remove();
    if(thread != NULL)  //wake up the next thread
        scheduler->ReadyToRun(thread);

    (void) interrupt->SetLevel(oldLevel);   // re-enable interrupts
}

void Condition::Broadcast(Lock* conditionLock)
{
    Thread* thread;
    ASSERT(conditionLock->isHeldByCurrentThread());
    IntStatus oldLevel = interrupt->SetLevel(IntOff); // disable interrupts
    thread = (Thread *)queue->Remove();
    while(thread != NULL)   //wake up all threads
    {
        scheduler->ReadyToRun(thread);
        thread = (Thread *)queue->Remove();
    }

    (void) interrupt->SetLevel(oldLevel);   // re-enable interrupts
}
