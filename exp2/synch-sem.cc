#include "synch-sem.h"
#include "system.h"

//ASSERT define in utility.h
Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts

    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    }
    value--; 					// semaphore available,
						// consume its value

    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

void Lock::Acquire()
{
    ASSERT(!isHeldByCurrentThread()); // prevent lock holder from
                                    // acquiring the lock a second time
    s->P();
    isBusy = true;
    owner = currentThread;
}

void Lock::Release()
{
    ASSERT(isHeldByCurrentThread()); //ensure current thread
                                    //is the owner of the Lock
    isBusy = false;
    owner = NULL;
    s->V();
}

void Condition::Wait(Lock* conditionLock)
{
    if (mutex == NULL)
        mutex = conditionLock;
    else
        ASSERT(mutex == conditionLock);
    Semaphore *waiter;

    ASSERT(conditionLock->isHeldByCurrentThread());

    waiter = new Semaphore("condition", 0);
    waitQueue->Append((void *)waiter);
    conditionLock->Release();
    waiter->P();
    conditionLock->Acquire();
    delete waiter;
}

void Condition::Signal(Lock* conditionLock)
{
    if (mutex == NULL)
        mutex = conditionLock;
    else
        ASSERT(mutex == conditionLock);
    Semaphore *waiter;

    ASSERT(conditionLock->isHeldByCurrentThread());

    if (!waitQueue->IsEmpty())
    {
        waiter = (Semaphore *)waitQueue->Remove();
        waiter->V();
    }
}

void Condition::Broadcast(Lock* conditionLock)
{
    if (mutex == NULL)
        mutex = conditionLock;
    else
        ASSERT(mutex == conditionLock);
    ASSERT(conditionLock->isHeldByCurrentThread());

    while (!waitQueue->IsEmpty())
    {
        Signal(conditionLock);
    }
}
