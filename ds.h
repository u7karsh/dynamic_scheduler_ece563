/*H**********************************************************************
* FILENAME    :       ds.h
* DESCRIPTION :       Contains structures and prototypes for dynamic
*                     instruction scheduler simulator
* NOTES       :       -NA-
*
* AUTHOR      :       Utkarsh Mathur           START DATE :    10 Nov 17
*
* CHANGES :
*
*H***********************************************************************/


#ifndef _DS_H
#define _DS_H

#include "all.h"

// Mask for 32 bit address
#define   MSB_ONE_32_BIT    0x80000000

// Pointer translations
typedef  struct  _dsT                 *dsPT;

// Dynamic Instruction Scheduler structure.
typedef struct _dsT{
   /*
    * Configutration params
    */
   // Placeholder for name
   char                  name[128];
   int                   s;
   int                   n;
}dsT;

// Function prototypes. This part can be automated
// lets keep hardcoded as of now
dsPT  dynamicSchedulerInit(   
      char*              name, 
      int                s,
      int                n
      );

void dsProcess( dsPT dsP, int pc, int operation, int dst, int src1, int src2 );

#endif
