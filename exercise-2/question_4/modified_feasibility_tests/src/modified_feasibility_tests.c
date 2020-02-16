#include <math.h>
#include <modified_feasibility_tests.h>
#include <stdio.h>

int completion_time_feasibility( U32_T numServices,
                                 U32_T period[],
                                 U32_T wcet[],
                                 U32_T deadline[] )
{
   int i, j;
   U32_T an, anext;

   // assume feasible until we find otherwise
   int set_feasible = TRUE;

   //printf("numServices=%d\n", numServices);

   for ( i = 0; i < numServices; i++ )
   {
      an    = 0;
      anext = 0;

      for ( j = 0; j <= i; j++ )
      {
         an += wcet[ j ];
      }

      //printf("i=%d, an=%d\n", i, an);

      while ( 1 )
      {
         anext = wcet[ i ];

         for ( j = 0; j < i; j++ )
            anext += ceil( ( (double)an ) / ( (double)period[ j ] ) ) * wcet[ j ];

         if ( anext == an )
            break;
         else
            an = anext;

         //printf("an=%d, anext=%d\n", an, anext);
      }

      //printf("an=%d, deadline[%d]=%d\n", an, i, deadline[i]);

      if ( an > deadline[ i ] )
      {
         set_feasible = FALSE;
      }
   }

   return set_feasible;
}

int scheduling_point_feasibility( U32_T numServices,
                                  U32_T period[],
                                  U32_T wcet[],
                                  U32_T deadline[] )
{
   int rc = TRUE, i, j, k, l, status, temp;

   for ( i = 0; i < numServices; i++ )  // iterate from highest to lowest priority
   {
      status = 0;

      for ( k = 0; k <= i; k++ )
      {
         for ( l = 1; l <= ( floor( (double)period[ i ] / (double)period[ k ] ) ); l++ )
         {
            temp = 0;

            for ( j = 0; j <= i; j++ )
               temp += wcet[ j ] * ceil( (double)l * (double)period[ k ] / (double)period[ j ] );

            if ( temp <= ( l * period[ k ] ) )
            {
               status = 1;
               break;
            }
         }
         if ( status )
            break;
      }
      if ( !status )
         rc = FALSE;
   }
   return rc;
}

double calculate_utility( U32_T numServices, U32_T period[], U32_T wcet[] )
{
   int i          = 0;
   double utility = 0.0;
   for ( i = 0; i < numServices; ++i )
   {
      utility = utility + ( (double)wcet[ i ] / (double)period[ i ] );
   }
   return utility;
}

void print_set_information( U32_T numExercise,
                            U32_T numServices,
                            U32_T period[],
                            U32_T wcet[],
                            U32_T deadline[] )
{
   int i          = 0;
   double utility = calculate_utility( numServices, period, wcet );
   printf( "Ex-%u U=%4.2f (",
           numExercise,
           utility );
   for ( i = 0; i < numServices; ++i )
   {
      printf( "C%u=%u%c ",
              i + 1,
              wcet[ i ],
              ( i == ( numServices - 1 ) ? ';' : ',' ) );
   }
   U32_T periods_equal_deadlines = TRUE;
   for ( i = 0; i < numServices; ++i )
   {
      printf( "T%u=%u%c ",
              i + 1,
              period[ i ],
              ( i == ( numServices - 1 ) ? ';' : ',' ) );
      if ( period[ i ] != deadline[ i ] )
      {
         periods_equal_deadlines = FALSE;
      }
   }
   printf( "T%sD): ", ( ( periods_equal_deadlines == FALSE ) ? "/=" : "=" ) );
}

// source: https://www.programiz.com/c-programming/examples/hcf-gcd
static int getGcd( U32_T n1, U32_T n2 )
{
   while ( n1 != n2 )
   {
      if ( n1 > n2 )
      {
         n1 -= n2;
      }
      else
      {
         n2 -= n1;
      }
   }
   return n1;
}

static int getLcm( U32_T n1, U32_T n2 )
{
   U32_T gcd = getGcd( n1, n2 );
   return ( ( n1 * n2 ) / gcd );
}

static int getLcmOfPeriods( U32_T numServices, U32_T period[] )
{
   U32_T lcm;
   U32_T i;
   for ( i = 0; i < numServices; ++i )
   {
      if ( i == 0 )
      {
         lcm = getLcm( period[ 0 ], period[ 1 ] );
      }
      else
      {
         lcm = getLcm( lcm, period[ i ] );
      }
   }
   return lcm;
}

static int getIndexOfMin( U32_T array[], U32_T n )
{
   U32_T i;
   U32_T current_min  = array[ 0 ];
   U32_T index_of_min = 0;

   for ( i = 1; i < n; ++i )
   {
      if ( array[ i ] < current_min )
      {
         index_of_min = i;
         current_min  = array[ i ];
      }
   }
   return index_of_min;
}

int earliest_deadline_first_scheduler( U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[] )
{
   U32_T lcm = getLcmOfPeriods( numServices, period );
   U32_T i;
   U32_T j;
   U32_T k;

   U32_T remainingComputationTime[ numServices ];
   U32_T timeUntilDeadline[ numServices ];

   for ( i = 0; i < numServices; ++i )
   {
      remainingComputationTime[ i ] = wcet[ i ];
      timeUntilDeadline[ i ]        = deadline[ i ];
   }

   j = getIndexOfMin( timeUntilDeadline, numServices );
   for ( k = 1; k < lcm; ++k )
   {
      if ( remainingComputationTime[ j ] > 0 )
      {
         remainingComputationTime[ j ]--;
      }
      for ( i = 0; i < numServices; i++ )
      {
         //printf("\nservice %d,DEAD_TM=%d,comp_rem=%d\n",i,timeUntilDeadline[i],comp_rem[i]);
         if ( remainingComputationTime[ i ] == 0 )
         {
            timeUntilDeadline[ i ] = lcm;
         }
         if ( timeUntilDeadline[ i ] > 0 )
         {
            //decrement all deadline times
            timeUntilDeadline[ i ]--;
         }
         if ( timeUntilDeadline[ i ] < remainingComputationTime[ i ] )
         {
            return FALSE;
         }

         if ( k % period[ i ] == 0 )
         {
            timeUntilDeadline[ i ]        = deadline[ i ];
            remainingComputationTime[ i ] = wcet[ i ];
         }
      }
      j = getIndexOfMin( timeUntilDeadline, numServices );
   }

   return TRUE;
}

int least_laxity_first_scheduler( U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[] )
{
   return TRUE;
}