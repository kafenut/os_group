// EventBarrier.cc
//
// Copyright (c) 2020 MarxYoung.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "EventBarrier.h"
#include "system.h"

//#define OUTPUT_INFO

//----------------------------------------------------------------------
// EventBarrier::EventBarrier
//	Initialize an EventBarrier with no waiters and unsignaled
//  "debugName" is an arbitrary name, useful for debugging.
//----------------------------------------------------------------------

EventBarrier::EventBarrier(char *debugName)
{
    waitersCnt = 0;
    state = false;
    signalLock = new Lock(debugName);
    signalCond = new Condition(debugName);
    signalMutex = new Lock(debugName);
    completeLockSignal = new Lock(debugName);
    completeCondSignal = new Condition(debugName);
    completeLockWait = new Lock(debugName);
    completeCondWait = new Condition(debugName);
}

//----------------------------------------------------------------------
// EventBarrier::~EventBarrier
//  De-allocate an EventBarrier, when no longer needed.
//  Assume there are no waiters or signaler waiting!
//----------------------------------------------------------------------

EventBarrier::~EventBarrier()
{
    delete signalLock;
    delete signalCond;
    delete completeLockSignal;
    delete completeCondSignal;
    delete completeLockWait;
    delete completeCondWait;
}

//----------------------------------------------------------------------
// EventBarrier::Wait
//  Wait until the event is signaled. Return immediately
//  if already in the signaled state.
//----------------------------------------------------------------------

void
EventBarrier::Wait()
{
    signalLock->Acquire();
    #ifdef OUTPUT_INFO
        printf("**%s Wait\n", currentThread->getName());
    #endif
    waitersCnt++;
    if (state)
    {
        signalLock->Release();
        return;
    }

    signalCond->Wait(signalLock);
    signalLock->Release();
}

//----------------------------------------------------------------------
// EventBarrier::Signal
//  Signal the event and block until all threads that wait for this
//  event have responded. The EventBarrier reverts to the unsignaled
//  state when Signal() returns.
//----------------------------------------------------------------------

void
EventBarrier::Signal()
{
    signalMutex->Acquire();
    signalLock->Acquire();
    state = true;       // set EventBarrier to signaled state
    signalCond->Broadcast(signalLock);
    signalLock->Release();

    #ifdef OUTPUT_INFO
        printf("**%s Signal\n", currentThread->getName());
    #endif

    completeLockSignal->Acquire();
    completeCondSignal->Wait(completeLockSignal);
    completeLockSignal->Release();

    completeLockWait->Acquire();
    completeCondWait->Broadcast(completeLockWait);

    state = false;      // EventBarrier reverts to unsignaled state

    #ifdef OUTPUT_INFO
        printf("**%s passes the EventBarrier\n", currentThread->getName());
    #endif

    completeLockWait->Release();
    signalMutex->Release();
}

//----------------------------------------------------------------------
// EventBarrier::Complete
//  Indicate that the calling thread has finished responding to a
//  signaled event, and block until all other threads that wait for
//  this event have also responded.
//----------------------------------------------------------------------

void
EventBarrier::Complete()
{
    completeLockWait->Acquire();

    if (--waitersCnt == 0)
    {
        completeLockSignal->Acquire();
        completeCondSignal->Broadcast(completeLockSignal);
        completeLockSignal->Release();
    }

    #ifdef OUTPUT_INFO
        printf("**%s Respond\n", currentThread->getName());
    #endif

    completeCondWait->Wait(completeLockWait);

    #ifdef OUTPUT_INFO
        printf("**%s passes the EventBarrier\n", currentThread->getName());
    #endif

    completeLockWait->Release();
}

//----------------------------------------------------------------------
// EventBarrier::Waiters
//  Return a count of threads that are waiting for the event or that
//  have not yet responded to it.
//----------------------------------------------------------------------

int
EventBarrier::Waiters()
{
    return waitersCnt;
}
