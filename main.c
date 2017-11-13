/*H**********************************************************************
* FILENAME    :       main.c 
* DESCRIPTION :       Program3 for ECE 563. Dynamic Instruction Scheduler
* NOTES       :       Entry to sim program
*
* AUTHOR      :       Utkarsh Mathur           START DATE :    10 Nov 17
*
* CHANGES :
*
*H***********************************************************************/


#include "all.h"
#include "ds.h"

int numInstructions = 0;

// Trace function to be mapped with init
boolean doTrace( dsPT dsP, int* pcP, int* operationP, int* dstP, int* src1P, int* src2P, int* memP )
{
   int pc, operation, dst, src1, src2, mem;
   if( !feof(dsP->fp) ){
      int bytesRead = fscanf( dsP->fp, "%x %d %d %d %d %x\n", &pc, &operation, &dst, &src1, &src2, &mem );
      // Just to safeguard on byte reading
      ASSERT(bytesRead <= 0, "fscanf read nothing!");

      // Operation can only be 0, 1 or 2
      ASSERT( !( operation >= 0 && operation <= 2 ), "Operation can only be 0, 1 or 2");

      // Register can be from -1 to 127
      ASSERT(!(dst  >= -1 && dst  <= 127), "dst reg out of bounds[-1, 127]: %d\n", dst);
      ASSERT(!(src1 >= -1 && src1 <= 127), "src1 reg out of bounds[-1, 127]: %d\n", src1);
      ASSERT(!(src2 >= -1 && src2 <= 127), "src2 reg out of bounds[-1, 127]: %d\n", src2);

      *pcP        = pc;
      *operationP = operation;
      *dstP       = dst;
      *src1P      = src1;
      *src2P      = src2;
      *memP       = mem;
      numInstructions++;
      return TRUE;
   }
   return FALSE;
}

int main( int argc, char** argv )
{
   char traceFile[128];
   int s                   = atoi( argv[1] );
   int n                   = atoi( argv[2] );
   int blockSize           = atoi( argv[3] );
   int l1Size              = atoi( argv[4] );
   int l1Assoc             = atoi( argv[5] );
   int l2Size              = atoi( argv[6] );
   int l2Assoc             = atoi( argv[7] );
   sprintf( traceFile, "%s", argv[8] );

   FILE* fp                = fopen( traceFile, "r" ); 
   ASSERT(!fp, "Unable to read file: %s\n", traceFile);

   dsPT dsP                = dynamicSchedulerInit( "DS", fp, s, n, doTrace, blockSize, l1Size, l1Assoc, l2Size, l2Assoc );
   while( !dsProcess( dsP ) );

   cachePrintContents( dsP->l1P );
   cachePrintContents( dsP->l2P );

   // Print coniguration of dsP
   printf("CONFIGURATION\n");
   printf(" superscalar bandwidth (N) = %d\n", dsP->n);
   printf(" dispatch queue size (2*N) = %d\n", 2*dsP->n);
   printf(" schedule queue size (S)   = %d\n", dsP->s);
   printf("RESULTS\n");
   printf(" number of instructions = %d\n", numInstructions);
   // Cycle - 1 as it stands one ahead
   int cycles              = dsP->cycle - 1;
   printf(" number of cycles       = %d\n", cycles);
   printf(" IPC                    = %0.2f\n", (double)numInstructions / (double)(cycles));

}
