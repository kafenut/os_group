#include <cstddef>
#include"dllist.h"
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include "system.h"
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

//----------------------------------------------------------------------
// DLLElement::DLLElement
//	    Initialize an element.
//----------------------------------------------------------------------

DLLElement::DLLElement(void* itemPtr, int sortKey)
{
	next = prev = NULL;
	item = itemPtr;
	key = sortKey;
}

DLList::DLList()
{
	first = NULL;
	last = NULL;
	err_type = -1;
}
DLList::DLList(int err_type)
{
    first = last = NULL;
    this->err_type = err_type;
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
void* DLList::Remove()				//�޸ĵ�3������ط� ԭ�� int* keyPtr ����û�á�
{
	DLLElement* temp;
	temp = first;
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
		first = first->next;
	}
	return &(temp->key);
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
			if (ele->key < p->key)			//�������޸ĵ�1
				break;
			else
				p = p->next;
		}
		if (first == p)		// if insert to head			//�޸ĵ�2������������һ���ڵ�ʱ��ҲҪ�Աȴ�С
		{
            ele->prev = NULL;
            ele->next = p;
            p->prev = ele;

            if(err_type == 2){
                currentThread->Yield();}

            first = ele;
		}

		else if (p == NULL)	// if insert to tail
		{
            p = last;
			p->next = ele;
			ele->next = NULL;
			ele->prev = p;

			if(err_type == 2){
                currentThread->Yield();}

            last = ele;
		}
		else 				//if insert to center
		{
			ele->prev = p->prev;
            ele->next = p;
            p->prev->next = ele;
            p->prev = ele;

            if(err_type == 2){
                currentThread->Yield();}
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
    else
		printf("Empty List!");
	printf("\n");
}
