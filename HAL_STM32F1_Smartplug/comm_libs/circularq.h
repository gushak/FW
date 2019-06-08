#pragma once

#include <stdint.h>
#include <stdlib.h>

/* Defines -------------------------------------------------------------------*/
typedef uint8_t CirQ_Elem_t;

/* Type declarations ---------------------------------------------------------*/
typedef struct
{
  int front, rear;
  size_t elem_size;
  CirQ_Elem_t* elem;
}CirQ_t;

typedef enum {CIRQ_SUCCESS=0, CIRQ_ERROR} CirlQ_Status_t;

/* Function prototypes -------------------------------------------------------*/
void cirq_init(CirQ_t* q, CirQ_Elem_t* elem, size_t elem_size);
void cirq_clear(CirQ_t* q);
int cirq_is_empty(const CirQ_t* q);
int cirq_is_full(const CirQ_t* q);
size_t cirq_elem_count(const CirQ_t* q);
CirlQ_Status_t cirq_enque(CirQ_t* q, uint8_t item);
CirlQ_Status_t cirq_dequeue(CirQ_t* q, uint8_t *item);
uint8_t cirq_direct_dequeue(CirQ_t* q);
size_t cirq_peek(CirQ_t* q, CirQ_Elem_t *item, size_t copy_count);