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

   // Init FIFO
   dsP->fakeRobP                     = fifoInit();

   // Init 3 lists
   dsP->dispatchList                 = (int*) calloc( 2*n, sizeof(int) );
   dsP->issueList                    = (int*) calloc( s  , sizeof(int) );
   dsP->executeList                  = (int*) calloc( n  , sizeof(int) );

   return dsP;
}

void dsProcess( dsPT dsP, int pc, int operation, int dst, int src1, int src2 )
{
   fakeRetire( dsP );
   execute( dsP );
   issue( dsP );
   dispatch( dsP );
   fetch( dsP );
}

boolean dsInstNotWB( void* data )
{
   dsInstInfoPT payload = data;
   if( !payload ) return FALSE;
   if( payload->stage == PROC_PIPE_STAGE_WB ) return TRUE;
   return FALSE;
}

void fakeRetire( dsPT dsP )
{
   // Remove instructions from the head of fake ROB until an instruction 
   // is reached that is not in the WB state
   boolean success = TRUE;
   while( success ){
      fifoPopConditional( dsP->fakeRobP, &success, dsInstNotWB );
   }
}

void execute( dsPT dsP )
{
   // From the execute_list, check for instructions that are finishing
   // execution this cycle, and:
   // 1) Remove the instruction from the execute_list.
   // 2) Transition from EX state to WB state.
   // 3) Update the register file state (e.g., ready flag) and wakeup
   //    dependent instructions (set their operand ready flags)
}

void issue( dsPT dsP )
{
   // From the issue_list, construct a temp list of instructions whose
   // operands are ready – these are the READY instructions. Scan the READY
   // instructions in ascending order of tags and issue up to N of them.
   // To issue an instruction:
   // 1) Remove the instruction from the issue_list and add it to the
   //    execute_list.
   // 2) Transition from the IS state to the EX state.
   // 3) Free up the scheduling queue entry (e.g., decrement a count
   //    of the number of instructions in the scheduling queue)
   // 4) Set a timer in the instruction’s data structure that will allow
   //    you to model the execution latency
}

void dispatch( dsPT dsP )
{
   // From the dispatch_list, construct a temp list of instructions in the ID
   // state (don’t include those in the IF state – you must model the
   // 1 cycle fetch latency). Scan the temp list in ascending order of
   // tags and, if the scheduling queue is not full, then:
   // 1) Remove the instruction from the dispatch_list and add it to the
   //    issue_list. Reserve a schedule queue entry (e.g. increment a
   //    count of the number of instructions in the scheduling
   //    queue) and free a dispatch queue entry (e.g. decrement a count of
   //    the number of instructions in the dispatch queue).
   // 2) Transition from the ID state to the IS state.
   // 3) Rename source operands by looking up state in the register
   //    file; rename destination operands by updating state in
   //    the register file.
   //
   // For instructions in the dispatch_list that are in the IF
   // state, unconditionally transition to the ID state (models the 1 cycle
   // latency for instruction fetch)
}

void fetch( dsPT dsP )
{
   // Read new instructions from the trace as long as 1) you have not
   // reached the end-of-file, 2) the fetch bandwidth is not exceeded,
   // and 3) the dispatch queue is not full.  Then, for each incoming
   // instruction:
   // 1) Push the new instruction onto the fake-ROB. Initialize the
   //    instruction’s data structure, including setting its state to IF.
   // 2) Add the instruction to the dispatch_list and reserve a
   //    dispatch queue entry (e.g., increment a count of the number
   //    of instructions in the dispatch queue)
}
