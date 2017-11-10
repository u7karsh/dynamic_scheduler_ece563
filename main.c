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

void doTrace( dsPT dsP, FILE* fp )
{
   int pc, operation, dst, src1, src2, mem;
   int cycle = 0;
   do{
      int bytesRead = fscanf( fp, "%x %d %d %d %d %x\n", &pc, &operation, &dst, &src1, &src2, &mem );
      // Just to safeguard on byte reading
      ASSERT(bytesRead <= 0, "fscanf read nothing!");

      // Operation can only be 0, 1 or 2
      ASSERT( !( operation >= 0 && operation <= 2 ), "Operation can only be 0, 1 or 2");

      // Register can be from -1 to 127
      ASSERT(!(dst  >= -1 && dst  <= 127), "dst reg out of bounds[-1, 127]: %d\n", dst);
      ASSERT(!(src1 >= -1 && src1 <= 127), "src1 reg out of bounds[-1, 127]: %d\n", src1);
      ASSERT(!(src2 >= -1 && src2 <= 127), "src2 reg out of bounds[-1, 127]: %d\n", src2);

      dsProcess( dsP, pc, operation, dst, src1, src2 );

      // Advance cycle
      cycle++;
   } while( !feof(fp) );
}


void printStats( )
{
}

int main( int argc, char** argv )
{
   char traceFile[128];
   int s                   = atoi( argv[1] );
   int n                   = atoi( argv[2] );
   sprintf( traceFile, "%s", argv[3] );

   FILE* fp                = fopen( traceFile, "r" ); 
   ASSERT(!fp, "Unable to read file: %s\n", traceFile);

   dsPT dsP                = dynamicSchedulerInit( "DS", s, n );

   doTrace( dsP, fp );
}
