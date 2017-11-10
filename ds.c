/*H**********************************************************************
* FILENAME    :       ds.c 
* DESCRIPTION :       Consists all dynamic scheduler related operations
* NOTES       :       *Caution* Data inside the original dsP
*                     might change based on functions.
*
* AUTHOR      :       Utkarsh Mathur           START DATE :    10 Nov 17
*
* CHANGES :
*
*H***********************************************************************/

#include "ds.h"

// Since this is a small proj, add all utils in this
// file instead of a separate file
//-------------- UTILITY BEGIN -----------------

#define LOG2(A)      log(A) / log(2)
// This is a ceil LOG2
#define CLOG2(A)     ceil( LOG2(A) )

// Creates a mask with lower nBits set to 1 and left shifts it by offset
int  utilCreateMask( int nBits, int offset )
{
   int mask = 0, index;
   for( index=0; index<nBits; index++ )
      mask |= 1 << index;

   return mask << offset;
}

int utilShiftAndMask( int input, int shiftValue, int mask )
{
   return (input >> shiftValue) & mask;
}

//-------------- UTILITY END   -----------------

// Allocates and inits all internal variables
dsPT  dynamicSchedulerInit(   
      char*              name, 
      int                s,
      int                n
      )
{
   // Calloc the mem to reset all vars to 0
   dsPT dsP                          = (dsPT) calloc( 1, sizeof(dsT) );

   sprintf( dsP->name, "%s", name );
   dsP->s                            = s;
   dsP->n                            = n;

   return dsP;
}

void dsProcess( dsPT dsP, int pc, int operation, int dst, int src1, int src2 )
{
}
