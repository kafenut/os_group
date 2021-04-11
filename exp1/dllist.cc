#include<stdio.h>
#include<iostream>
using namespace std;

class DLLElement
{
public:
	DLLElement(void* itemPtr, int sortKey); // initialize a list element

	DLLElement* next; // next element on list
					  // NULL if this is the last
	DLLElement* prev; // previous element on list
					  // NULL if this is the first
	int key;          // priority, for a sorted list
	void* item;       // pointer to item on the list
};

DLLElement::DLLElement(void* itemPtr, int sortKey)
{
	next = prev = NULL;
	item = itemPtr;
	key = sortKey;
}


class DLList{
public:
	DLList();	//initialize a list
	~DLList();	//de-allocate the list

	void Prepend(void *item);	//add to head of list (set key = min_key-1)
	void Append(void *item);	//add to tail of list (set key = max_key+1)
	void *Remove();				//remove from head of list
								//return *key of the removed item
	bool IsEmpty();				//return false if list has elements

	void SortedInsert(void *item, int sortKey);
	void *SortedRemove(int sortKey);	//remove first item with key==sortKey

private:
	DLLElement *first;	//head of the list
	DLLElement *last;	//last of the list
}

DLList::DLList()
{
	first = NULL;
	last = NULL;
}
DLList::~DLList()
{
	return;
}

void DLList::Prepend(void* item)
{
	DLLElement* ele = new DLLElement(item, 0);
	if (IsEmpty())	// if list is empty
	{
		ele->key = 1;
		ele->prev = NULL;
		ele->next = NULL;
		first = ele;
		last = ele;
	}
	else 	// if list is not empty
	{
		DLLElement* temp;	// add element to head
		temp = first;
		ele->key = temp->key - 1;
		ele->prev = NULL;
		ele->next = temp;
		first = ele;
		temp->prev = ele;
	}
	return;
}
void DLList::Append(void* item)
{
	DLLElement* ele = new DLLElement(item, 0);
	if (IsEmpty())	// if list is empty
	{
		ele->key = 1;
		ele->prev = NULL;
		ele->next = NULL;
		first = ele;
		last = ele;
	}
	else 	// if list is not empty
	{
		DLLElement* temp;	// add element to tail
		temp = last;
		ele->key = temp->key + 1;
		ele->prev = temp;
		ele->next = NULL;
		last = ele;
		temp->next = ele;
	}
	return;
}
void* DLList::Remove()				//
{
	DLLElement* temp;
	if (IsEmpty())	// if list is empty
	{
		return NULL;
	}
	else if (first == last)	// if there is only one element
	{
		first = NULL;
		last = NULL;
	}
	else
	{
		temp = first->next;
		temp->prev = NULL;
		first = temp;
	}
	return &(first->key);
}
bool DLList::IsEmpty()
{
	if (first == NULL)
		return true;
	return false;
}
void DLList::SortedInsert(void* item, int sortKey)
{
	DLLElement* ele = new DLLElement(item, sortKey);
	if (IsEmpty())	// if list is empty
	{
		first = ele;
		last = ele;
		ele->prev = NULL;
		ele->next = NULL;
	}
	else
	{
		DLLElement* p = first;
		while (p)
		{
			if (ele->key < p->key || p ->next== NULL)			//王贝伦修改点1
				break;
			else
				p = p->next;
		}
		if (first == p)		// if insert to head			//修改点2：链表中已有一个节点时，也要对比大小
		{
			if (p->key <= ele->key)
			{
				ele->prev = p;
				ele->next = NULL;
				p->next = ele;
				last = ele;
			}
			else
			{
				first = ele;
				ele->prev = NULL;
				ele->next = p;
				p->prev = ele;
			}

		}

		else if (p == NULL)	// if insert to tail
		{
			p = last;
			last = ele;
			p->next = ele;
			ele->next = NULL;
			ele->prev = p;
		}
		else 				//if insert to center
		{
			if (p->key <= ele->key)
			{
				p->next = ele;
				ele->next = NULL;
				ele->prev = p;
				last = ele;
			}
			else
			{
				ele->prev = p->prev;
				ele->next = p;
				p->prev->next = ele;
				p->prev = ele;
			}
		}
	}
	return;
}
void* DLList::SortedRemove(int sortKey)
{
	DLLElement* p = first;
	while (p)
	{

		if (p->key == sortKey)
			break;
		p = p->next;
	}
	if (p == NULL)	// no such element
	{
		return NULL;
	}
	else
	{
		if (first == p)	// if remove from head
		{
			first = p->next;
			p->next->prev = NULL;
		}
		else if (last == p)	// if remove from tail
		{
			last = p->prev;
			p->prev->next = NULL;
		}
		else
		{
			p->prev->next = p->next;
			p->next->prev = p->prev;
		}
	}
	return p;
}
void DLList::Show()
{
	int flag = 1;
	DLLElement* p = first;
	while (p)
	{
		printf(" %d(%d) ", flag, p->key);
		flag++;
		p = p->next;
	}
	printf("\n");
	if(first&&last)
		printf("f:%d l:%d", first->key, last->key);
	printf("\n");
}

// int main()
// {
// 	int N = 10;
// 	int i;
// 	int s = 0, key;
// 	int* item_ptr;
// 	DLList pt;
// 	DLList* test=&pt;

// 	for (i = 0; i < N; i++) {
// 		test->SortedInsert(&s, (int)(rand() % 100));
// 	}
// 	test->Show();
// 	//for (i = 0; i < N; i++) {
// 	//	test->Remove();
// 	//}
// 	test->Show();
// 	test->SortedRemove(78);
// 	test->Show();
// 	test->SortedRemove(62);
// 	test->Show();
// 	test->SortedRemove(41);
// 	test->Show();
// 	test->SortedRemove(0);
// 	test->Show();
// 	test->Append(&s);
// 	test->Show();
// 	test->Prepend(&s);
// 	test->Show();
// 	test->Remove();
// 	test->Show();
// }
