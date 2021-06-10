#include "system.h"
//#define OUTPUT
//----------------------------------------------------------------------
// Alarm::Alarm
//	Initialize an alarm with a waiting list.
//----------------------------------------------------------------------
Alarm::Alarm()
{
    list = new List();
}

//----------------------------------------------------------------------
// Alarm::Alarm
//	De-allocate an alarm, when no longer needed.
//----------------------------------------------------------------------
Alarm::~Alarm()
{
    delete list;
}

//----------------------------------------------------------------------
// Alarm::CheckList
//	Constantly check the number of threads in waiting list.
//  If there is no threads, quit.
//----------------------------------------------------------------------
void
Alarm::CheckList()
{
    while (num > 0)
    {
        currentThread->Yield();
    }

    // waiting list is empty
    DEBUG('t', "*** No thread in waiting list ***\n");
    currentThread->Finish();
}

//----------------------------------------------------------------------
// Alarm::CheckHandler
//	To invoke CheckList function.
//----------------------------------------------------------------------
static void CheckHandler(int arg)
{
	Alarm* p = (Alarm*)arg;
	p->CheckList();
}

//----------------------------------------------------------------------
// Alarm::Pause
//	Calculate the correct ticks for thread to sleep, add thread into the
//  waiting list and make thread sleep.
//----------------------------------------------------------------------
void
Alarm::Pause(int howLong)
{
    // disable interrupts to atomically complete procedure
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    int waketime = stats->totalTicks + howLong * TimerTicks;
    list->SortedInsert(currentThread, waketime); // insert into sorted waiting list
    num++; // increase num of threads in waiting list

    // if it's first time to create an alarm, create a thread to always check waiting list
    if (num == 1)
    {
        Thread *t = new Thread("CheckList thread");
        t->Fork(CheckHandler, (int)this);
    }
    #ifdef OUTPUT
    printf("*** %s started sleeping for %d at %d (current ticks) ***\n", currentThread->getName(), howLong * TimerTicks, stats -> totalTicks);
    #endif // OUTPUT
    currentThread->Sleep();
    (void)interrupt->SetLevel(oldLevel); // enable interrupts
}

//----------------------------------------------------------------------
// Alarm::Wakeup
//	Remove a thread from list first and check whether it is time to wake
//  up thread. If waketime <= current time, wake up the thread and move
//  to next check. Otherwise, restore the thread into the list and quit.
//----------------------------------------------------------------------
void
Alarm::Wakeup()
{
    // disable interrupts to atomically complete procedure
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    int waketime;
    Thread *thread = (Thread *)list->SortedRemove(&waketime);
    while (thread != NULL)
    {
        // time's up
        if (waketime <= stats->totalTicks)
        {
            #ifdef OUTPUT
            printf("*** %s woke up at %d (current ticks)***\n", thread->getName(), stats->totalTicks);
            #endif
            scheduler->ReadyToRun(thread);                    // make the thread runnable
            num--;                                            // decrease num of threads in waiting list
            thread = (Thread *)list->SortedRemove(&waketime); // pick up another thread that should wake up
        }
        else
        {
            list->SortedInsert((void *)thread, waketime); // restore the thread
            break;
        }
    }

    (void)interrupt->SetLevel(oldLevel); // enable interrupts
}
