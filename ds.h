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
#include "fifo.h"

// Mask for 32 bit address
#define   MSB_ONE_32_BIT    0x80000000

// Pointer translations
typedef  struct  _dsT                 *dsPT;
typedef  struct  _dsInstInfoT         *dsInstInfoPT;

// Emums
typedef enum{
   PROC_PIPE_STAGE_IF   = 0,
   PROC_PIPE_STAGE_ID   = 1,
   PROC_PIPE_STAGE_IS   = 2,
   PROC_PIPE_STAGE_EX   = 3,
   PROC_PIPE_STAGE_WB   = 4
}procPipeStageT;

// Dynamic Instruction Scheduler structure.
typedef struct _dsT{
   /*
    * Configutration params
    */
   // Placeholder for name
   char                  name[128];
   int                   s;
   int                   n;

   // Circular FIFO
   fifoPT                fakeRobP;

   // 3 Lists
   // Dispatch list a.k.a dispatch queue: size:= 2n
   int*                  dispatchList;
   // Issue list a.k.a scheduling queue : size:= s
   int*                  issueList;
   // Exectute list a.k.a FU            : size:= n
   int*                  executeList;
}dsT;

// Container for "Fake ROB" for storing per instruction info
typedef struct _dsInstInfoT{
   procPipeStageT      stage;
   int                 type;
   int                 state;
   int                 sequenceNum;
}dsInstInfoT;

// Function prototypes. This part can be automated
// lets keep hardcoded as of now
dsPT  dynamicSchedulerInit(   
         char*              name, 
         int                s,
         int                n
      );

void dsProcess( dsPT dsP, int pc, int operation, int dst, int src1, int src2 );
boolean dsInstNotWB( void* data );
void fakeRetire( dsPT dsP );
void execute( dsPT dsP );
void issue( dsPT dsP );
void dispatch( dsPT dsP );
void fetch( dsPT dsP );

#endif
