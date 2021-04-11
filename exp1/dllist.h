#ifndef DLLIST_H
#define DLLIST_H


class DLLElement{
public:
	DLLElement(void *itemPtr, int sortKey);	//initialize a list element

	DLLElement *next;	//next element
	DLLElement *prev;	//previous element

	int key;	//priority for a sorted list
	void *item;	//pointer to item on the list
}

class DLList{
public:	
	int err_type;   // type of concurrent errors

	DLList();	//initialize a list
	DLList(int err_type);
	~DLList();	//de-allocate the list

	void Prepend(void *item);	//add to head of list (set key = min_key-1)
	void Append(void *item);	//add to tail of list (set key = max_key+1)
	void *Remove();				//remove from head of list
								//return *key of the removed item
	bool IsEmpty();				//return false if list has elements
	void show();
	void SortedInsert(void *item, int sortKey);
	void *SortedRemove(int sortKey);	//remove first item with key==sortKey

	

private:
	DLLElement *first;	//head of the list
	DLLElement *last;	//last of the list
}

#endif