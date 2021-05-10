#ifndef SYNCH-SLEEP_H_INCLUDED
#define SYNCH-SLEEP_H_INCLUDED

#include "thread.h"
#include "list.h"

class Lock {
  public:
    Lock(char* debugName);  		// initialize lock to be FREE
    ~Lock();				// deallocate lock
    char* getName() { return name; }	// debugging assist

    void Acquire(); // these are the only operations on a lock
    void Release(); // they are both *atomic*

    bool isHeldByCurrentThread();	// true if the current thread
					// holds this lock.  Useful for
					// checking in Release, and in
					// Condition variable ops below.

  private:
    char* name;				// for debugging
    Thread* owner;
    List queue;
    bool isBusy;  // for judge the lock is free or not
    // plus some other stuff you'll need to define
};

class Condition {
  public:
    Condition(char* debugName);		// initialize condition to
					// "no one waiting"
    ~Condition();			// deallocate the condition
    char* getName() { return (name); }

    void Wait(Lock *conditionLock); 	// these are the 3 operations on
					// condition variables; releasing the
					// lock and going to sleep are
					// *atomic* in Wait()
    void Signal(Lock *conditionLock);   // conditionLock must be held by
    void Broadcast(Lock *conditionLock);// the currentThread for all of
					// these operations

  private:
    char* name;
    List queue;
    // plus some other stuff you'll need to define
};
#endif // SYNCH-SLEEP_H_INCLUDED
