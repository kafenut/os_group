# include "Table.h"

//----------------------------------------------------------------------
// Table::Table
// 	Create a table to hold at most 'size' entries.
//----------------------------------------------------------------------
Table::Table(int size)
{
	this->size = size;
    elem = new void *[this->size + 1]();
	lock = new Lock("TableLock");
	
}

//----------------------------------------------------------------------
// Table::~Table
// 	De-allocate Table when no longer needed.
//----------------------------------------------------------------------
Table::~Table()
{
	delete elem;
    delete lock;
}

//----------------------------------------------------------------------
// Table::Alloc
//  Allocate a table slot for 'object'.
//  Return the table index for the slot or -1 on error.
//----------------------------------------------------------------------
int Table::Alloc(void* object)
{
    int index = -1;

    if(object == NULL)
        return index;
	lock->Acquire();
	for(int i = 0; i < size; i++)
	{
		if(elem[i] == NULL)
		{
			elem[i] = object;
			index= i;
			break;
		}
	}
	lock->Release();
	return index;
}

//----------------------------------------------------------------------
// Table::Get
//  Return the object from table index 'index' or NULL on error.
//  (assert index is in range).  Leave the table entry allocated
//  and the pointer in place.
//----------------------------------------------------------------------
void* Table::Get(int index)
{
	ASSERT(index >= 0 && index < size);
	return elem[index];
}

//----------------------------------------------------------------------
// Table::Release
// 	Free a table slot.
//----------------------------------------------------------------------
void Table::Release(int index)
{
	ASSERT(index >= 0 && index < size);
	elem[index] = NULL;
}
