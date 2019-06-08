
#include "circularq.h"

void cirq_init(CirQ_t *q, CirQ_Elem_t* elem, size_t elem_size)
{
        q->front = q->rear = 0;
        q->elem_size = elem_size;
        q->elem = elem;
}

void cirq_clear(CirQ_t* q)
{
        q->front = q->rear = 0;
}

int cirq_is_empty(const CirQ_t* q)
{
        return (q->front == q->rear);
}

int cirq_is_full(const CirQ_t* q)
{
        return ((q->rear+1) % (int)q->elem_size == q->front);
}

size_t cirq_elem_count(const CirQ_t* q)
{
        return (size_t)(q->rear - q->front + (int)q->elem_size) % q->elem_size;
}

CirlQ_Status_t cirq_enque(CirQ_t* q, uint8_t item)
{
        if(cirq_is_full(q))
            return CIRQ_ERROR;
        q->elem[q->rear] = item;
        q->rear = (q->rear+1) % q->elem_size;
        
        return CIRQ_SUCCESS;
}

CirlQ_Status_t cirq_dequeue(CirQ_t* q, uint8_t *item)
{
        if(cirq_is_full(q))
            return CIRQ_ERROR;
        *item = q->elem[q->front];
        q->front = (q->front + 1) % q->elem_size;
        
        return CIRQ_SUCCESS;
}

uint8_t cirq_direct_dequeue(CirQ_t* q)
{
        uint8_t item;
        
        if(cirq_is_empty(q))
                return 0;
         item = q->elem[q->front];
         q->front = (q->front + 1) % q->elem_size;
         
         return item;
}

size_t cirq_peek(CirQ_t* q, CirQ_Elem_t *item, size_t copy_count)
{
        int front;
        size_t count = cirq_elem_count(q);
        
        if(copy_count > count)
                copy_count = count;
        front = q->front;
        for(count = 0; count < copy_count; ++count)
        {
                *item++ = q->elem[front];
                front = (front + 1) % q->elem_size;
        }
   
 	return count;    
}


