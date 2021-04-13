#ifndef DLLIST_H
#define DLLIST_H

class DLLElement;

class DLList {
public:
	DLList();	//initialize a list
	DLList(int err_type);
	~DLList();	//de-allocate the list

	void Prepend(void* item);	//add to head of list (set key = min_key-1)
	void Append(void* item);	//add to tail of list (set key = max_key+1)
	void* Remove();	//remove from head of list
								//set *keyPtr to key of the removed item
	bool IsEmpty();				//return false if list has elements
	void Show();
	void SortedInsert(void* item, int sortKey);
	void* SortedRemove(int sortKey);	//remove first item with key==sortKey

private:
	DLLElement* first;	//head of the list
	DLLElement* last;	//last of the list
	int err_type;   // type of concurrent errors
};

#endif
