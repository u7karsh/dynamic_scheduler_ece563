/*H**********************************************************************
* FILENAME    :       fifo.h
* DESCRIPTION :       Contains structures and prototypes for generic FIFO
*
* NOTES       :       -NA-
*
* AUTHOR      :       Utkarsh Mathur           START DATE :    10 Nov 17
*
* CHANGES :
*
*H***********************************************************************/


#ifndef _FIFO_H
#define _FIFO_H

#include "all.h"

// Mask for 32 bit address
#define   MSB_ONE_32_BIT    0x80000000

// Pointer translations
typedef  struct  _fifoT                 *fifoPT;
typedef  struct  _fifoCellT             *fifoCellPT;

// Generic FIFO structure.
typedef struct _fifoT{
   fifoCellPT          head;
   int                 numElems;
}fifoT;

typedef struct _fifoCellT{
   fifoCellPT          next;
   void*               payload;
}fifoCellT;

fifoPT  fifoInit();
fifoCellPT fifoCreateCell( void* payload );
void fifoPush( fifoPT fifoP, void* payload );
void* fifoPop( fifoPT fifoP );
void* fifoPeekNth( fifoPT fifoP, int n, boolean* success );
int fifoNumElems( fifoPT fifoP );
void* fifoPopConditional( fifoPT fifoP, boolean* success, boolean (*funcP)() );
#endif
