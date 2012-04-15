// PrpGrnGod.cpp : Defines the entry point for the console application.
//
// This is the Purple and Green Sliding Puzzle God's Algorithm Solver
// by Adrian Dale 2002
//
// Just to keep my brain in gear...
// Shame my PC is running short of memory - makes this difficult to test
//
// NB Need to alter Stack Allocation linker setting to something huge!

// What this program does is to solve the Purple and Green puzzle in
// the optimum manner by working backwards from a solved puzzle to get
// the puzzle into all possible states.
// 16,777,216 of them, although we can exclude ones with spaces in the 
// middle, leaving only 92400 positions
//
// Then to solve the puzzle we find the state that it is in and follow
// the From pointers back up to the winning Depth==1 state.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef unsigned int PuzzleDepth;
typedef unsigned int PuzzleHashId;

// This structure holds the hash id that each of the three possible
// slides could move to.
typedef struct stPuzzleState
{
   // Hash id for position we came from
   // (Wastes one byte for simplicity)
   unsigned int From;

   // What this position looks like
   PuzzleHashId PuzState;
   // This could be smaller but for now not bothered as even if Depth
   // is only one byte (which isn't enough, anyway) the struct sizes at
   // 8 bytes
   PuzzleDepth Depth;
   
} PuzzleState;

#define HASH_SIZE 100000

// A hash id is an array of three bytes, one for the Front, Middle and Back
// faces.
//
// id[0] = Front(Purple)
// id[1] = Middle(Yellow/Space)
// id[2] = Back(Green)
//
// Each face has four squares, represented by two bytes each.
// The two bytes can be:
// 00 = Space
// 01 = Green
// 10 = Purple
// 11 = Yellow
//
// In order from LSB to MSB, each face is ordered:
//
// +-+-+
// |0|2|
// +-+-+
// |4|6|
// +-+-+

typedef struct stPuzzleState *PuzzleStatePtr;

PuzzleStatePtr gpAllStates;
unsigned int   giNextState = 0;

void PopulateTableBFS();
void GetPossibleMoves( PuzzleHashId PuzTo, PuzzleHashId *NewPuzTo1,
                       PuzzleHashId *NewPuzTo2, PuzzleHashId *NewPuzTo3 );
PuzzleHashId IdBitSwap( PuzzleHashId PuzTo, int iFaceX, int iFaceY );
void DisplayPuzId( PuzzleHashId PuzId );
char GetPiece( PuzzleHashId Puz, int n );
void WriteTable( char *FileName );
void ReadTable( char *FileName );
void Solve( PuzzleHashId Puz );
void Curious( void );

#define WIN_STATE1 (0x0055fea8)
#define WIN_STATE2 (0x0055fba2)
#define WIN_STATE3 (0x0055ef8a)
#define WIN_STATE4 (0x0055bf2a)
#define WIN_STATE5 (0x0054fdaa)
#define WIN_STATE6 (0x0051f7aa)
#define WIN_STATE7 (0x0045dfaa)
#define WIN_STATE8 (0x00157faa)

int main(int argc, char* argv[])
{
	
   printf("Purple/Green Puzzle Solver - Gods Algorithm\n");

   printf("sizeof(PuzzleState)=%d    Allocating %d bytes\n",
          sizeof(PuzzleState), sizeof(PuzzleState) * HASH_SIZE);

   // Allocate our storage so that it is intialised to zero.
   // 
   gpAllStates = (PuzzleStatePtr)calloc(HASH_SIZE, sizeof(PuzzleState));
   if (gpAllStates == NULL)
      printf("Not enough memory\n");
   else
      printf("Memory successfully allocated\n");



// Bit of testing here:
//WriteTable("C:\\TEMP\\PrpGrnGod.dat");

//DisplayPuzId(WIN_STATE1);

//PuzzleHashId m1, m2, m3;
//GetPossibleMoves(WIN_STATE1, &m1, &m2, &m3);
//printf("GetPossibleMoves(%x)=%x %x %x\n",
//       WIN_STATE1, m1, m2, m3);
//DisplayPuzId(m1);
//DisplayPuzId(m2);
//DisplayPuzId(m3);
//exit(0);
// End



//#if 0
   // Make the table
   PopulateTableBFS();
   WriteTable("C:/Temp/PrpGrnGod.dat");
//#endif
   // Cheat and read it in from a file - should speed things up
   // a bit!
   ReadTable("C:/Temp/PrpGrnGod.dat");
   

   // Now need to solve the thing!
   PuzzleHashId ScrambledState = 0x76bd98;

   Solve(ScrambledState);
   //Curious();
	
   return 0;
}

// Show some statistics
void Curious()
{
   for( int i=0; i<92400; i++ )
   {
      if (gpAllStates[i].Depth > 22)
      {
         DisplayPuzId(gpAllStates[i].PuzState);
      }
   }
}

void Solve( PuzzleHashId Puz )
{
   int i;

   for( i=0; i<92400; i++ )
   {
      if (gpAllStates[i].PuzState == Puz)
         break;
   }

   if ( i == 92400 )
   {
      printf("ERROR - Puzzle state %x not found in table\n", Puz);
      exit(-1);
   }

   printf("State %x is %d moves from solved\n", Puz, gpAllStates[i].Depth);

   do
   {
      DisplayPuzId(gpAllStates[i].PuzState);
      i = gpAllStates[i].From;
   }
   while( gpAllStates[i].Depth != 1 );
   DisplayPuzId(gpAllStates[i].PuzState);
   
}

// This is my Breadth First Search attempt at Populate Table
#define MAX_DEPTH 93000

void PopulateTableBFS()
{
   unsigned int uiDepth;
   unsigned int uiHPos;
   int NewNodeFound=1;
   int FoundNode1;
   int FoundNode2;
   int FoundNode3;
   PuzzleHashId NewPuzTo1, NewPuzTo2, NewPuzTo3;

   // Pre-populate the states array with the winning states
   gpAllStates[giNextState].Depth = 1;
   gpAllStates[giNextState].From = 0; // Since this is the winning state
   gpAllStates[giNextState].PuzState = WIN_STATE1;
   giNextState++;
   gpAllStates[giNextState].Depth = 1;
   gpAllStates[giNextState].From = 0; // Since this is the winning state
   gpAllStates[giNextState].PuzState = WIN_STATE2;
   giNextState++;
   gpAllStates[giNextState].Depth = 1;
   gpAllStates[giNextState].From = 0; // Since this is the winning state
   gpAllStates[giNextState].PuzState = WIN_STATE3;
   giNextState++;
   gpAllStates[giNextState].Depth = 1;
   gpAllStates[giNextState].From = 0; // Since this is the winning state
   gpAllStates[giNextState].PuzState = WIN_STATE4;
   giNextState++;
   gpAllStates[giNextState].Depth = 1;
   gpAllStates[giNextState].From = 0; // Since this is the winning state
   gpAllStates[giNextState].PuzState = WIN_STATE5;
   giNextState++;
   gpAllStates[giNextState].Depth = 1;
   gpAllStates[giNextState].From = 0; // Since this is the winning state
   gpAllStates[giNextState].PuzState = WIN_STATE6;
   giNextState++;
   gpAllStates[giNextState].Depth = 1;
   gpAllStates[giNextState].From = 0; // Since this is the winning state
   gpAllStates[giNextState].PuzState = WIN_STATE7;
   giNextState++;
   gpAllStates[giNextState].Depth = 1;
   gpAllStates[giNextState].From = 0; // Since this is the winning state
   gpAllStates[giNextState].PuzState = WIN_STATE8;
   giNextState++;

   // Building the solution space up layer by layer up to either MAX_DEPTH
   // or all solutions found
   for( uiDepth=1; uiDepth < MAX_DEPTH && NewNodeFound == 1; uiDepth++ )
   {
      //if ( uiDepth % 10 == 0 )
      //{
         printf("Populating uiDepth=%d giNextState=%d\n", uiDepth, giNextState);
      //}

      NewNodeFound=0;
      for( uiHPos=0; uiHPos < giNextState; uiHPos++ )
      {
         if ( gpAllStates[uiHPos].Depth == uiDepth )
         {
            // Populate the three possible nodes below this one           
            GetPossibleMoves( gpAllStates[uiHPos].PuzState, &NewPuzTo1, &NewPuzTo2, &NewPuzTo3 );

            // See if we've already got any of these, in which case they are ignored, because
            // due to the nature of the algorithm we know that they will be nearer the top
            // of the tree than this one.
            FoundNode1 = FoundNode2 = FoundNode3 = 0;
            for( unsigned int i=0; i < giNextState; i++ )
            {
               if ( gpAllStates[i].PuzState == NewPuzTo1 )
                  FoundNode1 = 1;
               else if ( gpAllStates[i].PuzState == NewPuzTo2 )
                  FoundNode2 = 1;
               else if ( gpAllStates[i].PuzState == NewPuzTo3 )
                  FoundNode3 = 1;
            }
             
            // Update our max depth termination criteria
            if ( FoundNode1 == 1 || FoundNode2 == 1 || FoundNode3 == 1 )
               NewNodeFound = 1;

            // Populate the new nodes
            if ( FoundNode1 == 0 )
            {
               gpAllStates[giNextState].Depth = uiDepth+1;
               gpAllStates[giNextState].From = uiHPos;
               gpAllStates[giNextState].PuzState = NewPuzTo1;
               giNextState++;
            }
            if ( FoundNode2 == 0 )
            {
               gpAllStates[giNextState].Depth = uiDepth+1;
               gpAllStates[giNextState].From = uiHPos;
               gpAllStates[giNextState].PuzState = NewPuzTo2;
               giNextState++;
            }
            if ( FoundNode3 == 0 )
            {
               gpAllStates[giNextState].Depth = uiDepth+1;
               gpAllStates[giNextState].From = uiHPos;
               gpAllStates[giNextState].PuzState = NewPuzTo3;
               giNextState++;
            }

            if ( giNextState >= HASH_SIZE-5 )
            {
               printf("ERROR - HASH_SIZE exceeded\n");
               exit(-1);
            }
         }
      }
   }

   printf("Finished PopulateTableBFS: Depth reached %d giNextState=%d\n",
          uiDepth, giNextState);
}


// This function returns the three possible states which can be reached
// from this one.
void GetPossibleMoves( PuzzleHashId PuzTo, PuzzleHashId *NewPuzTo1,
                       PuzzleHashId *NewPuzTo2, PuzzleHashId *NewPuzTo3 )
{
   int iSpaceFace;
   int iSpaceMask;

   PuzzleHashId TempId;

#ifdef DEBUG
   // Make an assert call that checks the validity of a PuzzleHashId
#endif


   // First off find the position of the space as all moves move into this
   // face. Doing this by checking the bits of the PuzTo hash id.
   iSpaceMask = 0x03;
   for( iSpaceFace=0; iSpaceFace < 12; iSpaceFace++ )
   { 
      TempId = PuzTo;
      if ((TempId & iSpaceMask) == 0)
      {
         break;
      }

      // Move the mask along and try again
      iSpaceMask = iSpaceMask << 2;
   }
   
   if (iSpaceFace == 12)
   {
      // Something bad's happened - there's no space in the PuzzleHashId
      printf("ERROR - space not found in puzzle hash id:%x\n", PuzTo);
      exit(-1);
   }

   if (iSpaceFace > 3 && iSpaceFace < 8)
   {
      // Something bad's happened - there's a space in a middle face of the
      // PuzzleHashId
      printf("ERROR - space found in centre face (%d) of puzzle hash id:%x\n",
             iSpaceFace, PuzTo);
      exit(-1);
   }


   // First possibility is where the space moves in or out.
   if ( iSpaceFace < 4 )
   {
      // Space moves to back of puzzle
      *NewPuzTo1 = IdBitSwap( PuzTo, iSpaceFace, iSpaceFace+4 );
      *NewPuzTo1 = IdBitSwap( *NewPuzTo1, iSpaceFace+4, iSpaceFace+8 );
   }
   else // ie iSpaceFace > 7
   {
      // Space moves to back of puzzle
      *NewPuzTo1 = IdBitSwap( PuzTo, iSpaceFace, iSpaceFace-4 );
      *NewPuzTo1 = IdBitSwap( *NewPuzTo1, iSpaceFace-4, iSpaceFace-8 );
   }

   // Next possibility is where the space moves left/right
   // Slightly messy 'if' could use mod() fn but this works OK
   if ( iSpaceFace == 0 || iSpaceFace == 2 ||
        iSpaceFace == 8 || iSpaceFace == 10 )
   {
      // Move right
      *NewPuzTo2 = IdBitSwap( PuzTo, iSpaceFace, iSpaceFace+1 );
   }
   else // ie SpaceFace==1,3,9,11
   {
      // Move left
      *NewPuzTo2 = IdBitSwap( PuzTo, iSpaceFace, iSpaceFace-1 );
   }

   // Final possibility is where the space moves up/down
   if ( iSpaceFace == 0 || iSpaceFace == 1 ||
        iSpaceFace == 8 || iSpaceFace == 9 )
   {
      // Move down
      *NewPuzTo3 = IdBitSwap( PuzTo, iSpaceFace, iSpaceFace+2 );
   }
   else // ie SpaceFace==2,3,10,11
   {
      // Move up
      *NewPuzTo3 = IdBitSwap( PuzTo, iSpaceFace, iSpaceFace-2 );
   }

}

// IdBitSwap takes a hash id and references to two faces
// It returns a hash id where those two faces have been swapped
//
// There are probably much better ways to do this using XOR, etc
// NB I have TESTED this and it appears to work!
PuzzleHashId IdBitSwap( PuzzleHashId PuzTo, int iFaceX, int iFaceY )
{
   unsigned int iXBitMask = 0x03, iYBitMask = 0x03;
   int i;
   int iTempSwapBits;
   int iXSwapBits;
   int iYSwapBits;

   assert( iFaceX != iFaceY ); // Not invalid but a sign of trouble
   assert( iFaceX >=0 && iFaceX < 12 );
   assert( iFaceY >=0 && iFaceY < 12 );

   // Make the iFaceX bit mask - This may be a slightly cheesey
   // way to do something that could be done mathematically in one
   // hit if we used the pow() fn.
   // Or I could have used a lookup table for performance.
   for( i=0; i<iFaceX; i++ )
      iXBitMask = iXBitMask << 2;

   // Make the iFaceY bit mask
   for( i=0; i<iFaceY; i++ )
      iYBitMask = iYBitMask << 2;

   // Extract the bit pairs we're going to swap
   iXSwapBits = PuzTo & iXBitMask;
   iYSwapBits = PuzTo & iYBitMask;

   // Move the bit pairs back to the right so they become a number
   // between 0 and 3
   for( i=0; i<iFaceX; i++)
      iXSwapBits = iXSwapBits >> 2;

   for( i=0; i<iFaceY; i++)
      iYSwapBits = iYSwapBits >> 2;

   // Now swap the bits ...
   iTempSwapBits = iXSwapBits;
   iXSwapBits = iYSwapBits;
   iYSwapBits = iTempSwapBits;

   // ... and put them back to the right bit position
   for( i=0; i<iFaceX; i++)
      iXSwapBits = iXSwapBits << 2;

   for( i=0; i<iFaceY; i++)
      iYSwapBits = iYSwapBits << 2;

   // Mask out the bit positions in PuzTo
   PuzTo &= (iXBitMask^0xffffffff);
   PuzTo &= (iYBitMask^0xffffffff);

   // OR the newly swapped bits back in
   PuzTo |= iXSwapBits; PuzTo |= iYSwapBits;

   return PuzTo;
}

// Make a "graphic" representation of a puzzle
void DisplayPuzId( PuzzleHashId PuzId )
{
   printf("Back    Middle  Front\n");
//   printf("+-+-+   +-+-+   +-+-+\n");
   printf("|%c|%c|   |%c|%c|   |%c|%c|\n",
          GetPiece( PuzId, 8 ), GetPiece( PuzId, 9 ),
          GetPiece( PuzId, 4 ), GetPiece( PuzId, 5 ),
          GetPiece( PuzId, 0 ), GetPiece( PuzId, 1 ));
//   printf("+-+-+   +-+-+   +-+-+\n");
   printf("|%c|%c|   |%c|%c|   |%c|%c|\n",
          GetPiece( PuzId, 10 ), GetPiece( PuzId, 11 ),
          GetPiece( PuzId, 6 ), GetPiece( PuzId, 7 ),
          GetPiece( PuzId, 2 ), GetPiece( PuzId, 3 ));
//   printf("+-+-+   +-+-+   +-+-+\n");
}

char GetPiece( PuzzleHashId Puz, int n )
{
   unsigned int iBitMask = 0x03;
   int i;
   int iBits;

   assert( n >= 0 && n < 12 );
 
   // Code borrowed from IdBitSwap()
   for( i=0; i<n; i++ )
      iBitMask = iBitMask << 2;

   // Extract the bit pairs we're going to swap
   iBits = Puz & iBitMask;
  
   // Move the bit pairs back to the right so they become a number
   // between 0 and 3
   for( i=0; i<n; i++)
      iBits = iBits >> 2;

   switch(iBits)
   {
      case 0:
         return 'S';
      case 1:
         return 'G';
      case 2:
         return 'P';
      case 3:
         return 'Y';
   }

   printf("ERROR - GetPiece found an invalid piece. Bizarre!\n");
   exit(-1);
   return '*';
}

void WriteTable( char *FileName )
{
   printf("WriteTable(%s) called\n", FileName);

   FILE *fFile = fopen(FileName, "wb");

   if ( fFile == 0 )
   {
      printf("ERROR - couldn't open %s\n", FileName);
      exit(-1);
   }

   if ( fwrite( gpAllStates, sizeof(PuzzleState), 92400, fFile ) != 92400 )
   {
      printf("ERROR - writing to %s\n", FileName);
      exit(-1);
   }

   fclose(fFile);

   printf("WriteTable(%s) Finished\n", FileName);
}

// Assumes that sufficient memory has been allocated
void ReadTable( char *FileName )
{
   printf("ReadTable(%s) called\n", FileName);

   FILE *fFile = fopen(FileName, "rb");
   int iReadVal;

   if ( fFile == 0 )
   {
      printf("ERROR - couldn't open %s\n", FileName);
      exit(-1);
   }

   if ( (iReadVal = fread( gpAllStates, sizeof(PuzzleState), 92400, fFile )) != 92400 )
   {
      printf("ERROR - reading from %s (%d) %d, %d\n", FileName, iReadVal, feof(fFile), ferror(fFile));
      exit(-1);
   }

   fclose(fFile);

   printf("ReadTable(%s) Finished\n", FileName);
}
