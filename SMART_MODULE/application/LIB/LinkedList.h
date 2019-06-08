/**
 * @file    LinkedList.h
 * @brief   Core Utility - Templated Linked List class
 * @author  sam grove
 * @version 1.0
 * @see     
 *
 * Copyright (c) 2013
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdint.h>
#include "mbed.h"

/**
 *  @struct node
 *  @brief The Linked List structure
 */ 
template<typename T>
struct node
{
	T *data;         /*!< pointer to list member data */
	struct node *next;  /*!< pointer to the next list member */
};

///** Returns true if data1 shall be inserted before data 2.
//*/
//typedef bool insertAtFunc(void *data1, void *data2);

/** Example using the LinkedList Class
 * @code
 *  #include "mbed.h"
 *  #include "LinkedList.h"
 *  
 *  LinkedList<node>list;
 *  
 *  int main()
 *  {
 *      node *tmp;
 *      
 *      list.push((char *)"Two\n");
 *      list.append((char *)"Three\n");
 *      list.append((char *)"Four\n");
 *      list.push((char*)"One\n");
 *      list.append((char*)"Five\n");
 *      
 *      for(int i=1; i<=list.length(); i++)
 *      {
 *          tmp = list.pop(i);
 *          printf("%s", (char *)tmp->data);
 *      }
 *      
 *      error("done\n");
 *  }
 * @endcode
 */
 
 /** Example using new insertOrdered function:
 * @code
 
#include "mbed.h"
#include "LinkedList.h"

void printList(LinkedList<node> &list, const char* msg) 
{
    printf("%s: ", msg);
    for(int i=1; i<=list.length(); i++)
    {
        node *tmp = list.pop(i);
        printf("%d ", *(int*)tmp->data);
    }
    printf("\n");
}

bool ascending(int *n1, int *n2)
{
    int *d1 = (int*)n1;
    int *d2 = (int*)n2;
    bool rv = *d1 <= *d2;
    printf("(%d %d:%d)", *d1, *d2, rv);
    return rv;
}

void testAscending()
{
    node<int> *tmp;
    
    int n0 = 0;
    int n1 = 1;
    int n1B = 1;
    int n2 = 2;
    int n3 = 3;
    int n4 = 4;
    
    LinkedList<int>list;
    
    tmp = list.insertOrdered(&n2, ascending);
    if (NULL == tmp) {
        error("insertOrdered did not insert a node");
    }
    printList(list, "exp 2");
    
    tmp = list.insertOrdered(&n1, ascending);
    if (NULL == tmp) {
        error("insertOrdered did not insert a node");
    }
    printList(list, "exp 1,2");

    tmp = list.insertOrdered(&n4, ascending);
    if (NULL == tmp) {
        error("insertOrdered did not insert a node");
    }
    printList(list, "exp 1,2,4");

    tmp = list.insertOrdered(&n3, ascending);
    if (NULL == tmp) {
        error("insertOrdered did not insert a node");
    }
    printList(list, "exp 1,2,3,4");

    tmp = list.insertOrdered(&n0, ascending);
    if (NULL == tmp) {
        error("insertOrdered did not insert a node");
    }
    printList(list, "exp 0,1,2,3,4");

    tmp = list.insertOrdered(&n1B, ascending);
    if (NULL == tmp) {
        error("insertOrdered did not insert a node");
    }
    printList(list, "exp 0,1,1,2,3,4");
    
    tmp = list.pop(2);
    if (tmp->data != &n1) {
        error("pos 2 is not n1\n");
    }
    printf("n1 is good\n");
    
    tmp = list.pop(3);
    if (tmp->data != &n1B) {
        error("pos3 is not n1B");
    }
    printf("n1B is good\n");
}

bool descending(int *n1, int *n2)
{
    int *d1 = (int*)n1;
    int *d2 = (int*)n2;
    bool rv = *d1 <= *d2;
    printf("(%d %d:%d)", *d1, *d2, rv);
    return rv;
}

void testDescending()
{
    node<int> *tmp;
    
    int n0 = 0;
    int n1 = 1;
    int n1B = 1;
    int n2 = 2;
    int n3 = 3;
    int n4 = 4;
    
    LinkedList<int>l;
    
    tmp = l.insertOrdered(&n2, descending);
    if (NULL == tmp) {
        error("insertOrdered did not insert a node");
    }
    printList(l, "exp 2");
    
    tmp = l.insertOrdered(&n1, descending);
    if (NULL == tmp) {
        error("insertOrdered did not insert a node");
    }
    printList(l, "exp 2,1");

    tmp = l.insertOrdered(&n4, descending);
    if (NULL == tmp) {
        error("insertOrdered did not insert a node");
    }
    printList(l, "exp 4,2,1");

    tmp = l.insertOrdered(&n3, descending);
    if (NULL == tmp) {
        error("insertOrdered did not insert a node");
    }
    printList(l, "exp 4,3,2,1");

    tmp = l.insertOrdered(&n0, descending);
    if (NULL == tmp) {
        error("insertOrdered did not insert a node");
    }
    printList(l, "exp 4,3,2,1,0");

    tmp = l.insertOrdered(&n1B, descending);
    if (NULL == tmp) {
        error("insertOrdered did not insert a node");
    }
    printList(l, "exp 4,3,2,1,1,0");
    
    tmp = l.pop(4);
    if (tmp->data != &n1) {
        error("pos 2 is not n1\n");
    }
    printf("n1 is good\n");
    
    tmp = l.pop(5);
    if (tmp->data != &n1B) {
        error("pos3 is not n1B");
    }
    printf("n1B is good\n");
}


int main()
{
    printf("\nJob Scheduler Demo\n");

    testDescending();
    
    error("done\n");
 
    exit(0);
}
 * @endcode
 */

/**
 *  @class LinkedList
 *  @brief API abstraction for a Linked List
 */ 
template<typename T>
class LinkedList
{
protected:
	node<T> *_head;

public:
    /** Create the LinkedList object
     */   
	LinkedList() {
	// clear the member
		_head = 0;

		static_index = 0;
		static_size = 0;
		dynamic_start = 0;
		dynamic_size = 0;
		return;
	}
    
    /** Deconstructor for the LinkedList object
     *  Removes any members
     */ 
	~LinkedList() {
		flush();
		return;
	}
	/** Removes all members
	 */
	void flush() {
		// free any memory that is on the heap
		while (remove(1) != NULL);
	}
	
    /** >> Should be used for static data <<
     *  Add a member to the begining of the list
     *  @param data - Some data type that is added to the list
     *  @return The member that was just inserted (NULL if empty)
     */
	node<T> *push(T *data) {
		node<T> *new_node = new node<T>[1];
		// make sure the new object was allocated
		if (0 == new_node)
		{
			error("Memory allocation failed\n");
		}
		// update the next item in the list to the current head
		new_node->next = _head;
		// store the address to the linked datatype
		new_node->data = data;
		_head = new_node;
		static_index = 0;
		static_size++;
		dynamic_start++;
		return _head;
	}

    
    /** Add a member to some position based on sort condition.
    * The list will be iterated from _head to end and the moment inserted
    * data is NOT smaller than current node's data, then
    * data will be inserted in front of current node.
    * This will preserve ascending order of data.
    * @param data - some data type that is added to the list
    * @param isBigger - comparator function returns true if data1 is bigger
    * than data2 and false otherwise.  
    * If data1 equals data2, then ">" comparision
    * will result in LIFO order.  Using ">=" operator in the function
    * results in "FIFO" order and thus it preserves order in which items
    * were added to the list.
    *
    * If isBigger function is implemented with data1 < data2, then
    * result insert will be in descending order.  If data1 equals data2, then
    * LIFO order is established.
    * If data1 <= data2, then FIFO order is preserved when data1 equals data2.  
    *
    * @return The member that was just inserted (NULL if not inserted).
    */
/*	node<T> *insertOrdered(T *data, bool(isBigger)(T* data1, T *data2))
	{
		node<T>  *new_node = new  node<T>  [1];
	   // make sure the new object was allocated
		if (0 == new_node)
		{
			error("Memory allocation failed\n");
		}
		// store the address to the linked datatype
		new_node->data = data;
		// clear the next pointer
		new_node->next = 0;
		// check for an empty list
		if (0 == _head)
		{
			_head = new_node;
			return new_node;
		}
    
		node<T>  *current = _head;
		node<T>  *prev = 0;
	   // move to the item we want to insert
		while (isBigger(data, current->data))
		{
		    // while(true) execute the following
		    // data being inserted is smaller than current->data
			if (0 == current->next) {
			    // there is no more data
				current->next = new_node;
				return new_node;
			}
			prev = current;
			current = current->next;
		}
		if (0 == prev) {
			new_node->next = _head;
			_head = new_node;    
			return new_node;
		}
		new_node->next = current;
		prev->next = new_node;
		return new_node;
	}
 */    
    /** >> Should be used for dynamic data <<
     * Add a member to the end of the list
     *  @param data - Some data type that is added to the list
     *  @return The member that was just inserted (NULL if empty)
     */
	node<T> *append(T *data) {
		node<T> *current = _head;
		node<T> *new_node = new node<T>[1];
		// make sure the new object was allocated
		if (0 == new_node)
		{
			error("Memory allocation failed\n");
		}
		// store the address to the linked datatype
		new_node->data = data;
		// clear the next pointer
		new_node->next = 0;
		// check for an empty list
		if (0 == current)
		{
			_head = new_node;
			dynamic_start=1;
			dynamic_size=1;
			return _head;
		}
		else
		{
		    // look for the end of the list
			while (current->next != 0)
			{
				current = current->next;
			}
			// and then add the new item to the list
			current->next = new_node;
			dynamic_size++;
		}

		return current->next;
	}

    
    /** Remove a member from the list
     *  @param loc - The location of the member to remove
     *  @return The head of the list
     */
	node<T> *remove(uint32_t loc) {
		node<T> *current = _head;
		node<T> *prev = 0;
		// make sure we have an item to remove
		if ((loc <= length()) && (loc > 0))
		{
		    // move to the item we want to delete
			if (1 == loc)
			{
				_head = current->next;
			}
			else
			{
				for (uint32_t i = 2; i <= loc; ++i)
				{
					prev = current;
					current = current->next;
				}
				// store the item + 1 to replace what we are deleting
				prev->next = current->next;
			}
			delete current->data;
			delete[] current;
			if (loc <= static_size)
			{
				static_size--;
				static_index %= static_size;
				if (dynamic_start)
					dynamic_start--;
			}
			else
			{
				if (dynamic_size)
					dynamic_size--;
			}
		}

		return _head;
	}

    /** Get access to a member from the list
     *  @param loc - The location of the member to access
     *  @return The member that was just requested (NULL if empty or out of bounds)
     */
	node<T> *pop(uint32_t loc) {
		node<T> *current = _head;
		// make sure we have something in the location
		if ((loc > length()) || (loc == 0))
		{
			return 0;
		}
		// and if so jump down the list
		for (uint32_t i = 2; i <= loc; ++i)
		{
			current = current->next;
		}

		return current;
	}
	node<T> *pop(T* data, bool(isEqual)(T* data1, T* data2))
	{
		if (_head == NULL) {
			return NULL;
		}
		node<T> *current = _head;
		// make sure we have something in the location
		// and if so jump down the list
		while (!isEqual(current->data, data)) {
			if (current->next == NULL) {
				return NULL;
			}
			current = current->next;
		}
		return current;
	}

    /** Get the length of the list
     *  @return The number of members in the list
     */
	uint32_t length(void) {
		int32_t count = 0;
		node<T> *current = _head;
		//loop until the end of the list is found
		while (current != 0)
		{
			++count;
			current = current->next;
		}

		return count;
	}

	void flush_static() {
		if (!static_size)
			return;
		
		node<T> *data;
		// free any memory that is on the heap
		do {
			data = remove(1);
			static_size--;
		} while (data && static_size);
		static_size = 0;
	}
	void flush_dynamic() {
		if (!dynamic_size)
			return;
		uint32_t last_index = dynamic_start + dynamic_size;	// "- 1" is not applied because the remove index of head is 1.
		node<T> *data;
		// free any memory that is on the heap
		do {
			data = remove(last_index--);
		} while (data && last_index >= dynamic_start);
		dynamic_size = 0;
	}
	int16_t find(T* pData, uint8_t comp_bytes)
	{
		int16_t loop;
		int16_t size = length();
		if (!size)
			return 0;
		for (loop = size; loop > 0; loop--)
		{
			node<T> *pNode = pop(loop);
			if (!memcmp(pNode->data, pData, comp_bytes))
				break;
		}
		if (loop > 0)
			return loop;
		return 0;
	}
	node<T> *remove_matched(T* pData, uint8_t comp_bytes)
	{
		int16_t index = find(pData, comp_bytes);
		if (index >= 0)
			return remove(index);
		return (node<T> *)NULL;
	}
	T *getStatic() {
		if (!static_size)
			return NULL;

		static_index %= static_size;
		return pop((uint32_t)++static_index)->data;
	}
	T *getDynamic() {
		if (!dynamic_size)
			return NULL;
			
		return pop(length())->data;
	}
	uint16_t static_readable() {
		return static_size;
	}
	uint16_t dynamic_readable() {
		return dynamic_size;
	}
private:
	uint32_t static_index;
	uint32_t static_size;
	uint32_t dynamic_start;
	uint32_t dynamic_size;
};

#endif /* LINKEDLIST_H_ */
