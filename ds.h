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
#include "cache.h"

// Execution latencies
#define PIPE_EX_LATENCY_TYPE0 0
#define PIPE_EX_LATENCY_TYPE1 2
#define PIPE_EX_LATENCY_TYPE2 5

// Memory latencies
#define PIPE_EX_LATENCY_L1HIT  5
#define PIPE_EX_LATENCY_L1MISS 10
#define PIPE_EX_LATENCY_L2MISS 20

// Pointer translations
typedef  struct  _dsT                 *dsPT;
typedef  struct  _dsInstInfoT         *dsInstInfoPT;
typedef  struct  _dsCapsuleT          *dsCapsulePT;

// Emums for pipeline stages
typedef enum{
   PROC_PIPE_STAGE_IF   = 0,
   PROC_PIPE_STAGE_ID   = 1,
   PROC_PIPE_STAGE_IS   = 2,
   PROC_PIPE_STAGE_EX   = 3,
   PROC_PIPE_STAGE_WB   = 4
}procPipeStageT;

typedef enum{
   PROC_INST_TYPE0      = 0,
   PROC_INST_TYPE1      = 1,
   PROC_INST_TYPE2      = 2
}procInstructionT;

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
   cachePT               l1P;
   cachePT               l2P;

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
   int                 latency;
   int                 mem;

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
         boolean            (*fetchFP)( dsPT, int*, int*, int*, int*, int*, int* ), 
         int                blockSize,
         int                l1Size,
         int                l1Assoc,
         int                l2Size,
         int                l2Assoc
      );

boolean    dsProcess( dsPT dsP );
boolean    dsInstInEx( dsPT dsP, dsInstInfoPT  instP );
boolean    dsInstInWB( dsInstInfoPT  instP );
boolean    fakeRetire( dsPT dsP );
void       dsWakeup( int *reg,  dsInstInfoPT instP );
void       dsSearchDst( int *dstFlag,  dsInstInfoPT instP );
void       dsExFinish( dsPT dsP, dsInstInfoPT instP );
boolean    execute( dsPT dsP );
void       dsIssuer( dsPT dsP,  dsInstInfoPT instP );
boolean    issue( dsPT dsP );
void       dsDispatcher( dsPT dsP,  dsInstInfoPT instP );
boolean    dsInstSeqNum( int* seqNum, dsInstInfoPT instP );
boolean    dispatch( dsPT dsP );
boolean    fetch( dsPT dsP );

#endif
