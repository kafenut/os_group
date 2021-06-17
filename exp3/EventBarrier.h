#ifndef EVENTBARRIER_H
#define EVENTBARRIER_H

#include "copyright.h"
#include "synch.h"

class EventBarrier
{

private:
    int waitersCnt;                 // number of threads waiting on barrier
                                    // ignores threads that have responded 

    bool signaled;                  // if the barrier is signaled

    int logLevel;                   // 0 for no log
                                    // 1 for basic log

    Lock *signalLock;               // the mutex lock for signalCond
    Condition *signalCond;          // all threads calling Wait() wait on this condition
    Lock *signalMutex;              // mutex lock for signal

    
    Lock *controllerLock;           // the mutex lock for controllerCond
    Condition *controllerCond;      // barrier controller wait on this condition 
                                    // until all waiting threads have responded
    
    Lock *completeLock;         // the mutex lock for completeCondWait
    Condition *completeCond;    // all threads calling Complete wait on this condition
    

public:
    EventBarrier(char *debugName, int lv = 1);  
                                    // initialize EventBarrier with a default log level 1
    ~EventBarrier();                // deallocate the EventBarrier

    void Wait();                    // wait until the barrier is signaled. 
                                    // return immediately if already signaled 

    void Signal();                  // wake up all threads waiting on signalCond
                                    // then barrier controller(caller) blocks until all wakened threads have responed
                                    // then wake up all threads waiting on completeCond

    void Complete();                // wait until all threads waiting on barrier are ready to run 
                                    // must be called after Wait()

    int Waiters();                  // returns number of threads waiting on barrier
                                    // ignores threads that have responded 
};

#endif
