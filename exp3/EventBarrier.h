// EventBarrier.h 
//
// Copyright (c) 2020 MarxYoung.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef EVENTBARRIER_H
#define EVENTBARRIER_H

#include "copyright.h"
#include "synch.h"

class EventBarrier
{
private:
    /* data */
    int waitersCnt;     // count of threads that are waiting for
                // the event or that have not yet responded to it.
    bool state;         // true: signaled state
                        // false: unsignaled state
    Condition *signalCond;  // on which previous threads  
                        // block to wait for Signal
    Lock *signalLock;    // the mutex that signalCond is used with
    Lock *signalMutex;    // ensure mutual exclusion on Signal
    //Condition *completeCond;    // on which previous threads block until all  
                        // threads that wait for this event have responded
    //Lock *completeLock;     // the mutex that completeCond is used with
    Condition *completeCondSignal;    // on which previous threads block until all  
                        // threads that wait for this event have responded
    Lock *completeLockSignal;     // the mutex that completeCond is used with
    Condition *completeCondWait;    // on which previous threads block until all  
                        // threads that wait for this event have responded
    Lock *completeLockWait;     // the mutex that completeCond is used with
public:
    EventBarrier(char *debugName);   // initialize EventBarrier to "no one waiting"
    ~EventBarrier();            // deallocate the EventBarrier
    void Wait();        // Wait until the event is signaled. 
                // Return immediately if already in the signaled state.
    void Signal();      // Signal the event and block until all threads that 
                // wait for this event have responded. The EventBarrier  
                // reverts to the unsignaled state when Signal() returns.
    void Complete();    // Indicate that the calling thread has finished 
                // responding to a signaled event, and block until all 
                // other threads that wait for this event have also responded.
    int Waiters();      // Return a count of threads that are waiting  
                // for the event or that have not yet responded to it.
};

#endif // EVENTBARRIER_H
