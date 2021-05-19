#include "synch.h"

#include "BoundedBuffer.h"
#include "stdio.h"
#include "list.h"
using namespace std;
char name1[5] = "lock",name2[6]="empty",name3[5]="full";
BoundedBuffer::BoundedBuffer(int maxsize)
{
	sizeofbuffer = maxsize;
	wptr = rptr = new char;
	lockofbuffer = new Semaphore(name1,1);
	empty = new Semaphore(name2,maxsize);
	full = new Semaphore(name3,0);
}
void BoundedBuffer::Read(void* data, int size)
{
	int i;
	if (size > (wptr - rptr))			//check if there are enough item or not
	{
		printf("No Enough Items");
		return;
	}
	for (i = 0; i < size; i++)
	{
		full->P();
										//might be inerrupted
		lockofbuffer->P();
		((char*)data)[i] = *rptr;
		rptr++;
	
		empty->V();
		lockofbuffer->V();
	}
}
void BoundedBuffer::Write(void* data, int size)
{
	int i;

	if (((wptr - rptr) + size) > sizeofbuffer)
	{
		printf("No Enough Space");
		return;
	}
	for (i = 0; i < size; i++)
	{
		empty->P();

		lockofbuffer->P();
		*wptr = ((char*)data)[i];
		wptr+=1;

		full->V();
		lockofbuffer->V();
	}
}

void BoundedBuffer::Showbuffer()
{
	char* p;
	p = rptr;
	while (p < wptr)
	{
		printf("%c", *p);
		p++;
	}
}