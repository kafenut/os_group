#include "copyright.h"
#include "EventBarrier.h"
#include "system.h"

EventBarrier::EventBarrier(char *debugName, int lv)
{
    waitersCnt = 0;
    signaled = false;
    logLevel = lv;
    signalLock = new Lock(debugName);
    signalCond = new Condition(debugName);
    signalMutex = new Lock(debugName);
    controllerLock = new Lock(debugName);
    controllerCond = new Condition(debugName);
    completeLock = new Lock(debugName);
    completeCond = new Condition(debugName);
}

EventBarrier::~EventBarrier()
{
    delete signalLock;
    delete signalCond;
    delete controllerLock;
    delete controllerCond;
    delete completeLock;
    delete completeCond;
}

void
EventBarrier::Wait()
{
    signalLock->Acquire();

    if (logLevel == 1)
        printf("**%s Wait\n", currentThread->getName());

    waitersCnt++;
    if (signaled)
    {
        signalLock->Release();
        return;
    }

    signalCond->Wait(signalLock);
    signalLock->Release();
}

void
EventBarrier::Signal()
{
    signalMutex->Acquire();

    signalLock->Acquire();
    signaled = true;       // set EventBarrier to signaled state
    signalCond->Broadcast(signalLock);
    signalLock->Release();

    if (logLevel == 1)
        printf("**%s Signal\n", currentThread->getName());

    //controller go to sleep
    controllerLock->Acquire();
    controllerCond->Wait(controllerLock);
    controllerLock->Release();
    
    //controller signal completeCond
    completeLock->Acquire();
    completeCond->Broadcast(completeLock);

    signaled = false;      // EventBarrier reverts to unsignaled state

    if (logLevel == 1)
        printf("**%s passes the EventBarrier\n", currentThread->getName());

    completeLock->Release();

    signalMutex->Release();
}

void
EventBarrier::Complete()
{
    completeLock->Acquire();

    if (--waitersCnt == 0)
    {
        controllerLock->Acquire();
        controllerCond->Broadcast(controllerLock);
        controllerLock->Release();
    }

    if (logLevel == 1)
        printf("**%s Respond\n", currentThread->getName());

    completeCond->Wait(completeLock);

    if (logLevel == 1)
        printf("**%s passes the EventBarrier\n", currentThread->getName());

    completeLock->Release();
}

int
EventBarrier::Waiters()
{
    return waitersCnt;
}
