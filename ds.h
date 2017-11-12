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
typedef  struct  _dsCapsuleT          *dsCapsulePT;

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
   FILE*                 fp;
   int                   s;
   int                   n;
   boolean               (*fetchFP)( dsPT, int*, int*, int*, int*, int*, int* ); 
   int                   seqNum;
   int                   ready[128];
   int                   mapTable[128];
   int                   cycle;

   // Circular FIFO
   fifoPT                fakeRobP;

   // 3 Lists
   // Dispatch list a.k.a dispatch queue: size:= 2n
   fifoPT                dispatchList;
   // Issue list a.k.a scheduling queue : size:= s
   fifoPT                issueList;
   // Exectute list a.k.a FU            : size:= n
   fifoPT                executeList;

   // TempQ
   fifoPT                tempQ;
}dsT;

// Container for "Fake ROB" for storing per instruction info
typedef struct _dsInstInfoT{
   procPipeStageT      stage;       // State like WB, EX
   int                 type;        // Type of instruction

   // Operands that wud be renamed
   int                 src1;
   int                 src2;
   // Original operands
   int                 origSrc1;
   int                 origSrc2;
   int                 dst;

   int                 src1Ready;    // Src1 ready state
   int                 src2Ready;    // Src2 ready state

   int                 sequenceNum; // Tag or sequence number

   // Timing related info
   int                 ifStart;
   int                 ifDuration;

   int                 idStart;
   int                 idDuration;

   int                 isStart;
   int                 isDuration;

   int                 exStart;
   int                 exDuration;

   int                 wbStart;
   int                 wbDuration;
}dsInstInfoT;

typedef struct _dsCapsuleT{
   dsPT                dsP;
   void*               data;
}dsCapsuleT;

// Function prototypes. This part can be automated
// lets keep hardcoded as of now
dsPT  dynamicSchedulerInit(   
         char*              name, 
         FILE*              fp,
         int                s,
         int                n,
         boolean            (*fetchFP)( dsPT, int*, int*, int*, int*, int*, int* ) 
      );

boolean dsProcess( dsPT dsP );
boolean dsInstInEx( dsPT dsP, dsInstInfoPT  instP );
boolean dsInstInWB( dsInstInfoPT  instP );
boolean fakeRetire( dsPT dsP );
boolean execute( dsPT dsP );
void dsIssuer( dsPT dsP,  dsInstInfoPT instP );
boolean issue( dsPT dsP );
void dsDispatcher( dsPT dsP,  dsInstInfoPT instP );
boolean dsInstSeqNum( int* seqNum, dsInstInfoPT instP );
boolean dispatch( dsPT dsP );
boolean fetch( dsPT dsP );

#endif
