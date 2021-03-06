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

// Allocates and inits all internal variables
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
      )
{
   // Calloc the mem to reset all vars to 0
   dsPT dsP                          = (dsPT) calloc( 1, sizeof(dsT) );

   sprintf( dsP->name, "%s", name );
   dsP->fp                           = fp;
   dsP->s                            = s;
   dsP->n                            = n;
   dsP->fetchFP                      = fetchFP;

   // Init FIFO
   dsP->fakeRobP                     = fifoInit();

   // Init 3 lists
   dsP->dispatchList                 = fifoInit();
   dsP->issueList                    = fifoInit();
   dsP->executeList                  = fifoInit();

   dsP->tempQ                        = fifoInit();

   for( int i = 0; i < 128; i++ )
      dsP->ready[i]                  = 1;

   // Init cache
   cachePT  l1P          = cacheInit( "L1", l1Size, l1Assoc, blockSize, 0, POLICY_REP_LRU, POLICY_WRITE_BACK_WRITE_ALLOCATE, NULL);
   cachePT  l2P          = cacheInit( "L2", l2Size, l2Assoc, blockSize, 0, POLICY_REP_LRU, POLICY_WRITE_BACK_WRITE_ALLOCATE, NULL);
   dsP->l1P              = l1P;
   dsP->l2P              = l2P;

   return dsP;
}

boolean dsProcess( dsPT dsP )
{
   boolean result;
   result     = fakeRetire( dsP );
   result    &= execute( dsP );
   result    &= issue( dsP );
   result    &= dispatch( dsP );
   result    &= fetch( dsP );
   dsP->cycle++;
   return result;
}

// If instruction is in execute, check if it has executed
boolean dsInstInEx( dsPT dsP, dsInstInfoPT  instP )
{
   if( instP->stage == PROC_PIPE_STAGE_EX ){
      return ((dsP->cycle - instP->exStart) >= instP->latency) ? TRUE : FALSE;
   }
   return FALSE;
}

boolean dsInstInWB( dsInstInfoPT  instP )
{
   if( instP->stage == PROC_PIPE_STAGE_WB ) return TRUE;
   return FALSE;
}


boolean fakeRetire( dsPT dsP )
{
   // Remove instructions from the head of fake ROB until an instruction 
   // is reached that is not in the WB state
   boolean success = TRUE;
   while( success ){
      dsInstInfoPT infoP = fifoPopTailConditional( dsP->fakeRobP, &success, dsInstInWB );
      if( success ){
         printf("%d fu{%d} src{%d,%d} dst{%d} IF{%d,%d} ID{%d,%d} IS{%d,%d} EX{%d,%d} WB{%d,%d}\n",
               infoP->sequenceNum, infoP->type, infoP->origSrc1, infoP->origSrc2, infoP->dst,
               infoP->ifStart, infoP->ifDuration,
               infoP->idStart, infoP->idDuration,
               infoP->isStart, infoP->isDuration,
               infoP->exStart, infoP->exDuration,
               infoP->wbStart, infoP->wbDuration);
         free( infoP );
      }
   }
   return ( fifoNumElems( dsP->fakeRobP ) == 0 ) ? TRUE : FALSE;
}

void dsWakeup( int *reg,  dsInstInfoPT instP )
{

   if( instP->src1 == *reg ) instP->src1Ready = 1;
   if( instP->src2 == *reg ) instP->src2Ready = 1;
}


void dsSearchDst( int *dstFlag,  dsInstInfoPT instP )
{
   if( instP->dst == dstFlag[0] ){
      if( instP->sequenceNum > dstFlag[1] ){
         dstFlag[1] = instP->sequenceNum;
      }
   }
}

void dsExFinish( dsPT dsP, dsInstInfoPT instP )
{
   instP->exDuration              = dsP->cycle - instP->exStart;
   instP->wbStart                 = dsP->cycle;
   instP->wbDuration              = 1;
   instP->stage                   = PROC_PIPE_STAGE_WB;
   if( instP->dst != -1 ){
      int dstFlag[4]              = { instP->dst, 0, 0, 0 };

      fifoForeach( dsP->executeList, dsSearchDst, dstFlag );
      fifoForeach( dsP->issueList  , dsSearchDst, dstFlag );

      if( !dsP->ready   [ instP->dst ] ){
         dsP->ready   [ instP->dst ] = (dstFlag[1] == instP->sequenceNum) ? 1 : 0;
         dsP->mapTable[ instP->dst ] = dstFlag[1];
      }
      fifoForeach( dsP->issueList, dsWakeup, &( instP->sequenceNum ) );
   }
}

boolean execute( dsPT dsP )
{
   // From the executeList, check for instructions that are finishing
   // execution this cycle, and:
   // 1) Remove the instruction from the executeList.
   // 2) Transition from EX state to WB state.
   // 3) Update the register file state (e.g., ready flag) and wakeup
   //    dependent instructions (set their operand ready flags)
   
   // Remove all EX and transition in program order
   fifoSearchOpRemoveInv( dsP->executeList, dsInstInEx, dsExFinish, dsP, FALSE );

   return ( fifoNumElems( dsP->executeList ) == 0 ) ? TRUE : FALSE;
}

void dsIssuer( dsPT dsP,  dsInstInfoPT instP )
{
   // Check if instruction is ready to be scheduled
   boolean ready = ( instP->src1 == -1 || instP->src1Ready == 1 ) && 
                   ( instP->src2 == -1 || instP->src2Ready == 1 );
   
   if( ready ){
      // Push to head in temp ready queue
      fifoPush( dsP->tempQ, instP );
   }
}

boolean issue( dsPT dsP )
{
   // From the issueList, construct a temp list of instructions whose
   // operands are ready – these are the READY instructions. Scan the READY
   // instructions in ascending order of tags and issue up to N of them.
   // To issue an instruction:
   // 1) Remove the instruction from the issueList and add it to the
   //    executeList.
   // 2) Transition from the IS state to the EX state.
   // 3) Free up the scheduling queue entry (e.g., decrement a count
   //    of the number of instructions in the scheduling queue)
   // 4) Set a timer in the instruction’s data structure that will allow
   //    you to model the execution latency
   // Clear the old list
   while( fifoNumElems( dsP->tempQ ) > 0 ){
      fifoPop( dsP->tempQ );
   }
   fifoForeach( dsP->issueList, dsIssuer, dsP );

   // FUs are pipelined and can take upto N instructions every cycle
   // NOTE: Do not limit executions based on size of executeList as FUs are pipelined
   int iss              = 0;
   while( iss < dsP->n && fifoNumElems( dsP->tempQ ) > 0 ){
      dsInstInfoPT instP= fifoPop( dsP->tempQ );
      iss++;
      instP->isDuration = dsP->cycle - instP->isStart;
      instP->exStart    = dsP->cycle;
      instP->stage      = PROC_PIPE_STAGE_EX;

      // Memory operation on cache
      // NOTE: cacheCommunicate is smart enough to return miss if no cache is present
      // ----------------- CACHE PLUGIN BEGIN -------------------
      if( instP->type == PROC_INST_TYPE2 && dsP->l1P != NULL ){
         cacheCommT comm          = cacheCommunicate( dsP->l1P, instP->mem, CMD_DIR_READ );
         if( !comm.hit ){
            // L1 Miss
            instP->latency        = PIPE_EX_LATENCY_L1MISS;
            comm                  = cacheCommunicate( dsP->l2P, instP->mem, CMD_DIR_READ );
            if( !comm.hit ){
               // L2 Miss
               instP->latency     = PIPE_EX_LATENCY_L2MISS;
            }
         } else{
            // L1 Hit
            instP->latency        = PIPE_EX_LATENCY_L1HIT;
         }
      }
      // ----------------- CACHE PLUGIN END ---------------------

      fifoPush( dsP->executeList, instP );
      // Remove from dispatch list
      fifoSearchOpRemove( dsP->issueList, dsInstSeqNum, NULL, &(instP->sequenceNum), TRUE );
   }

   return ( fifoNumElems( dsP->issueList ) == 0 ) ? TRUE : FALSE;
}

void dsDispatcher( dsPT dsP,  dsInstInfoPT instP )
{
   // Construct a list of instructions in ID state
   if( instP->stage == PROC_PIPE_STAGE_ID ){
      fifoPush( dsP->tempQ, instP );
   } else{
      // Unconditionally transition to ID
      instP->ifDuration = dsP->cycle - instP->ifStart;
      instP->idStart    = dsP->cycle;
      instP->stage      = PROC_PIPE_STAGE_ID;
   }
}


boolean dsInstSeqNum( int* seqNum, dsInstInfoPT instP )
{
   if( instP->sequenceNum == *seqNum ) return TRUE;
   return FALSE;
}

boolean dispatch( dsPT dsP )
{
   // From the dispatchList, construct a temp list of instructions in the ID
   // state (don’t include those in the IF state – you must model the
   // 1 cycle fetch latency). Scan the temp list in ascending order of
   // tags and, if the scheduling queue is not full, then:
   // 1) Remove the instruction from the dispatchList and add it to the
   //    issueList. Reserve a schedule queue entry (e.g. increment a
   //    count of the number of instructions in the scheduling
   //    queue) and free a dispatch queue entry (e.g. decrement a count of
   //    the number of instructions in the dispatch queue).
   // 2) Transition from the ID state to the IS state.
   // 3) Rename source operands by looking up state in the register
   //    file; rename destination operands by updating state in
   //    the register file.
   //
   // For instructions in the dispatchList that are in the IF
   // state, unconditionally transition to the ID state (models the 1 cycle
   // latency for instruction fetch)

   // Clear the old list
   while( fifoNumElems( dsP->tempQ ) > 0 ){
      fifoPop( dsP->tempQ );
   }
   fifoForeach( dsP->dispatchList, dsDispatcher, dsP );

   // If scheduling queue is not full
   while( fifoNumElems( dsP->issueList ) < dsP->s && fifoNumElems( dsP->tempQ ) > 0 ){
      dsInstInfoPT instP             = fifoPop( dsP->tempQ );
      instP->idDuration              = dsP->cycle - instP->idStart;
      instP->isStart                 = dsP->cycle;
      instP->stage                   = PROC_PIPE_STAGE_IS;

      // Rename destination operands
      if( instP->src1 != -1 ){
         if( dsP->ready[ instP->src1 ] ){
            // No need to rename. set ready operand
            instP->src1Ready         = 1;
         } else{
            // Rename based on mapTable
            instP->src1              = dsP->mapTable[ instP->src1 ];
         }
      }

      if( instP->src2 != -1 ){
         if( dsP->ready[ instP->src2 ] ){
            // No need to rename. set ready operand
            instP->src2Ready         = 1;
         } else{
            // Rename based on mapTable
            instP->src2              = dsP->mapTable[ instP->src2 ];
         }
      }

      if( instP->dst != -1 ){
         // Renaming needed
         dsP->ready   [ instP->dst ] = 0;
         dsP->mapTable[ instP->dst ] = instP->sequenceNum;
      }
      fifoPush( dsP->issueList, instP );
      // Remove from dispatch list
      fifoSearchOpRemove( dsP->dispatchList, dsInstSeqNum, NULL, &(instP->sequenceNum), TRUE );
   }

   return ( fifoNumElems( dsP->dispatchList ) == 0 ) ? TRUE : FALSE;
}

boolean fetch( dsPT dsP )
{
   // Read new instructions from the trace as long as 
   // 1) you have not reached the end-of-file, 
   // 2) the fetch bandwidth is not exceeded,
   // 3) the dispatch queue is not full.
   //
   // Then, for each incoming instruction:
   // 1) Push the new instruction onto the fake-ROB. Initialize the
   //    instruction’s data structure, including setting its state to IF.
   // 2) Add the instruction to the dispatchList and reserve a
   //    dispatch queue entry (e.g., increment a count of the number
   //    of instructions in the dispatch queue)

   int numFetch  = 0;
   int n2        = 2 * dsP->n;
   while( fifoNumElems( dsP->dispatchList ) < n2 && numFetch < dsP->n ){
      // Fetch new instruction
      int pc, operation, dst, src1, src2, mem;
      if( dsP->fetchFP( dsP, &pc, &operation, &dst, &src1, &src2, &mem ) ){
         numFetch++;
         // Create instruction
         dsInstInfoPT instP = (dsInstInfoPT) calloc( 1, sizeof(dsInstInfoT) );
         instP->stage       = PROC_PIPE_STAGE_IF;
         instP->ifStart     = dsP->cycle;
         instP->type        = operation;
         instP->dst         = dst;
         instP->src1        = src1;
         instP->src2        = src2;
         instP->origSrc1    = src1;
         instP->origSrc2    = src2;
         instP->mem         = mem;
         instP->sequenceNum = dsP->seqNum++;

         // Assign execution latency based on operation type
         switch( operation ){
            case PROC_INST_TYPE0: instP->latency = PIPE_EX_LATENCY_TYPE0; break;
            case PROC_INST_TYPE1: instP->latency = PIPE_EX_LATENCY_TYPE1; break;
            default             : instP->latency = PIPE_EX_LATENCY_TYPE2; break;
         }

         // Push onto Fake ROB
         fifoPush( dsP->fakeRobP, instP );
         // Add instruction to dispatchList
         fifoPush( dsP->dispatchList, instP );
      } else{
         return TRUE;
      }
   }
   return FALSE;
}
