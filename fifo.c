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
   fifoP->tail                       = NULL;
   fifoP->numElems                   = 0;

   return fifoP;
}

// Interface to create new cells
fifoCellPT fifoCreateCell( void* payload )
{
   fifoCellPT cellP                  = (fifoCellPT) calloc( 1, sizeof(fifoCellT) );
   ASSERT( !cellP, "Unable to create FIFO cells" );

   cellP->next                       = NULL;
   cellP->prev                       = NULL;
   cellP->payload                    = payload;
   return cellP;
}

void fifoPush( fifoPT fifoP, void* payload )
{
   // Create a new cell
   fifoCellPT cellP                  = fifoCreateCell( payload );

   // Add newly created node to head
   cellP->next                       = fifoP->head;
   if( fifoP->head != NULL )
      fifoP->head->prev              = cellP;
   fifoP->head                       = cellP;
   if( fifoP->tail == NULL )
      fifoP->tail                    = fifoP->head;
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
   if( fifoP->head != NULL )
      fifoP->head->prev              = NULL;
   else
      fifoP->tail                    = NULL;

   // Free the deleted link
   free( cellP );
   fifoP->numElems--;

   return payload;
}

void* fifoPopTail( fifoPT fifoP )
{
   // Check underflow
   if( fifoP->numElems == 0 ) return NULL;

   // Get payload
   void* payload                     = fifoP->tail->payload;
   
   // Store the pointer to tail
   fifoCellPT cellP                  = fifoP->tail;

   // Update tail
   fifoP->tail                       = fifoP->tail->prev;
   if( fifoP->tail != NULL )
      fifoP->tail->next              = NULL;
   else
      fifoP->head                    = NULL;

   // Free the deleted link
   free( cellP );
   fifoP->numElems--;

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

void* fifoPopTailConditional( fifoPT fifoP, boolean* success, boolean (*funcP)() )
{
   *success    = FALSE;
   if( !fifoP || !fifoP->tail ) return NULL;

   if( funcP( fifoP->tail->payload ) ){
      *success = TRUE;
      return fifoPopTail( fifoP );
   }
   return NULL;
}

// For each
int fifoForeach( fifoPT fifoP, void (*funcOpP)(), void* userData )
{
   if( !fifoP || !fifoP->head ) return 0;

   fifoCellPT temp      = fifoP->head;
   int cnt = 0;
   while( temp != NULL ){
      funcOpP( userData, temp->payload );
      temp              = temp->next;
      cnt++;
   }
   return cnt;
}

int fifoForeachInv( fifoPT fifoP, void (*funcOpP)(), void* userData )
{
   if( !fifoP || !fifoP->tail ) return 0;

   fifoCellPT temp      = fifoP->tail;
   int cnt = 0;
   while( temp != NULL ){
      funcOpP( userData, temp->payload );
      temp              = temp->prev;
      cnt++;
   }
   return cnt;
}

// Search, operate (dealloc payload) and remove
int fifoSearchOpRemove( fifoPT fifoP, boolean (*funcCondP)(), void (*funcOpP)(), void* userData, boolean breakOnFind )
{
   if( !fifoP || !fifoP->head ) return 0;
   int count            = 0;

   fifoCellPT temp      = fifoP->head;
   while( temp != NULL ){
      // Search
      if( funcCondP( userData, temp->payload ) ){
         if( funcOpP != NULL )
            funcOpP( userData, temp->payload );

         // Remove
         // Unlink the node
         fifoCellPT delCell     = temp;

         if( delCell->prev != NULL ){
            delCell->prev->next = delCell->next;
         } else {
            // Node to be deleted is the head node
            fifoP->head         = delCell->next;
            if( fifoP->head != NULL )
               fifoP->head->prev= NULL;
         }

         if( delCell->next != NULL ){
            delCell->next->prev = delCell->prev;
         } else{
            // Node to be deleted is the tail node
            fifoP->tail         = delCell->prev;
            if( fifoP->tail != NULL )
               fifoP->tail->next   = NULL;
         }

         // Update temp to continue iteration
         temp                   = delCell->next;
         free( delCell );
         count++;
         fifoP->numElems--;
         if( breakOnFind ) break;
      } else{
         temp              = temp->next;
      }
   }
   return count;
}

// Search, operate (dealloc payload) and remove
int fifoSearchOpRemoveInv( fifoPT fifoP, boolean (*funcCondP)(), void (*funcOpP)(), void* userData, boolean breakOnFind )
{
   if( !fifoP || !fifoP->head ) return 0;
   int count            = 0;

   fifoCellPT temp      = fifoP->tail;
   while( temp != NULL ){
      // Search
      if( funcCondP( userData, temp->payload ) ){
         if( funcOpP != NULL )
            funcOpP( userData, temp->payload );

         // Remove
         // Unlink the node
         fifoCellPT delCell     = temp;

         if( delCell->prev != NULL ){
            delCell->prev->next = delCell->next;
         } else {
            // Node to be deleted is the head node
            fifoP->head         = delCell->next;
            if( fifoP->head != NULL )
               fifoP->head->prev= NULL;
         }

         if( delCell->next != NULL ){
            delCell->next->prev = delCell->prev;
         } else{
            // Node to be deleted is the tail node
            fifoP->tail         = delCell->prev;
            if( fifoP->tail != NULL )
               fifoP->tail->next   = NULL;
         }

         // Update temp to continue iteration
         temp                   = delCell->prev;
         free( delCell );
         count++;
         fifoP->numElems--;
         if( breakOnFind ) break;
      } else{
         temp              = temp->prev;
      }
   }
   return count;
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
