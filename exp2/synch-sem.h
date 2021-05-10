#ifndef SYNCH-SEM_H_INCLUDED
#define SYNCH-SEM_H_INCLUDED

#include "thread.h"
#include "list.h"

class Semaphore {
  public:
    Semaphore(char* debugName, int initialValue);	// set initial value
    ~Semaphore();   					// de-allocate semaphore
    char* getName() { return name;}			// debugging assist

    void P();	 // these are the only operations on a semaphore
    void V();	 // they are both *atomic*

  private:
    char* name;        // useful for debugging
    int value;         // semaphore value, always >= 0
    List *queue;       // threads waiting in P() for the value to be > 0
};

#endif // SYNCH-SEM_H_INCLUDED
