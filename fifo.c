/*H**********************************************************************
* FILENAME    :       fifo.c 
* DESCRIPTION :       Consists generic FIFO related operations
*
* AUTHOR      :       Utkarsh Mathur           START DATE :    10 Nov 17
*
* CHANGES :
*
*H***********************************************************************/

#include "fifo.h"

// Allocates and inits all internal variables
fifoPT  fifoInit()
{
   // Calloc the mem to reset all vars to 0
   fifoPT fifoP                      = (fifoPT) calloc( 1, sizeof(fifoT) );
   ASSERT( !fifoP, "Unable to create FIFO" );

   fifoP->head                       = NULL;
   fifoP->numElems                   = 0;

   return fifoP;
}

// Interface to create new cells
fifoCellPT fifoCreateCell( void* payload )
{
   fifoCellPT cellP                  = (fifoCellPT) calloc( 1, sizeof(fifoCellT) );
   ASSERT( !cellP, "Unable to create FIFO cells" );

   cellP->next                       = NULL;
   cellP->payload                    = payload;
   return cellP;
}

void fifoPush( fifoPT fifoP, void* payload )
{
   // Create a new cell
   fifoCellPT cellP                  = fifoCreateCell( payload );

   // Add newly created node to head
   cellP->next                       = fifoP->head;
   fifoP->head                       = cellP;
   fifoP->numElems++;
}

void* fifoPop( fifoPT fifoP )
{
   // Check underflow
   if( fifoP->numElems == 0 ) return NULL;

   // Get payload
   void* payload                     = fifoP->head->payload;
   
   // Store the pointer to head
   fifoCellPT cellP                  = fifoP->head;

   // Update head
   fifoP->head                       = fifoP->head->next;

   // Free the deleted link
   free( cellP );
   fifoP->numElems--;
   if( fifoP->numElems == 0 )
      fifoP->head                    = NULL;

   return payload;
}

void* fifoPopConditional( fifoPT fifoP, boolean* success, boolean (*funcP)() )
{
   *success    = FALSE;
   if( !fifoP || !fifoP->head ) return NULL;

   if( funcP( fifoP->head->payload ) ){
      *success = TRUE;
      return fifoPop( fifoP );
   }
   return NULL;
}

void* fifoPeekNth( fifoPT fifoP, int n, boolean* success )
{
   if( n >= fifoP->numElems ) { *success = FALSE; return NULL; }

   fifoCellPT temp      = fifoP->head;
   for( int i = 0; i < n; i++ ){
      temp              = temp->next;
   }
   ASSERT( !temp, "Illegal (NULL) value found at the nth(=%d) link of FIFO", n );
   *success             = TRUE;
   return temp->payload;
}

inline int fifoNumElems( fifoPT fifoP )
{
   return fifoP->numElems;
}
