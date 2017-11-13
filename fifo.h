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

// Pointer translations
typedef  struct  _fifoT                 *fifoPT;
typedef  struct  _fifoCellT             *fifoCellPT;

// Generic FIFO structure.
typedef struct _fifoT{
   fifoCellPT          head;
   fifoCellPT          tail;
   int                 numElems;
}fifoT;

typedef struct _fifoCellT{
   fifoCellPT          next;
   fifoCellPT          prev;
   void*               payload;
}fifoCellT;

fifoPT     fifoInit();
fifoCellPT fifoCreateCell( void* payload );
void       fifoPush( fifoPT fifoP, void* payload );
void*      fifoPop( fifoPT fifoP );
void*      fifoPopTail( fifoPT fifoP );
void*      fifoPeekNth( fifoPT fifoP, int n, boolean* success );
int        fifoNumElems( fifoPT fifoP );
void*      fifoPopConditional( fifoPT fifoP, boolean* success, boolean (*funcP)() );
void*      fifoPopTailConditional( fifoPT fifoP, boolean* success, boolean (*funcP)() );
int        fifoSearchOpRemove( fifoPT fifoP, boolean (*funcCondP)(), void (*funcOpP)(), void* userData, boolean breakOnFind );
int        fifoSearchOpRemoveInv( fifoPT fifoP, boolean (*funcCondP)(), void (*funcOpP)(), void* userData, boolean breakOnFind );
int        fifoForeach( fifoPT fifoP, void (*funcOpP)(), void* userData );
int        fifoForeachInv( fifoPT fifoP, void (*funcOpP)(), void* userData );
#endif
