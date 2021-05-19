#include "synch.h"

class BoundedBuffer {
   public:
     // create a bounded buffer with a limit of 'maxsize' bytes
     BoundedBuffer(int maxsize);
     
     // read 'size' bytes from the bounded buffer, storing into 'data'.
     // ('size' may be greater than 'maxsize')
     void Read(void *data, int size);
     
     // write 'size' bytes from 'data' into the bounded buffer.
     // ('size' may be greater than 'maxsize')
     void Write(void *data, int size);
     
     void Showbuffer();

   private:
       int sizeofbuffer;
       char* wptr,*rptr;
       Semaphore* lockofbuffer, *empty, *full;
};

