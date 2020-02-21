#include <math.h>
#include <modified_feasibility_tests.h>
#include <stdio.h>

int completion_time_feasibility( U32_T numServices,
                                 U32_T period[],
                                 U32_T wcet[],
                                 U32_T deadline[] )
{
   int i, j;         // i is n in the equation above, j is the same as the equation
   U32_T an, anext;  // an is an(t), anext is single term for particular n index

   // assume feasible until we find otherwise
   int set_feasible = TRUE;

   // this is the for loop that goes
   // through all a1(t), a2(t) ...... an(t) tests
   for ( i = 0; i < numServices; i++ )

   {
      // initalize them to be zeroes
      an    = 0;
      anext = 0;

      // start from Service0(S0) all the way to current i value
      for ( j = 0; j <= i; j++ )
      {
         // cumulative adding wect to an, to get the total
         // computation time up to i tasks
         an += wcet[ j ];
      }
      while ( 1 )
      {
         // add  wcet for task i
         anext = wcet[ i ];

         // this is the right hand side of the formula
         // it is the sum of the demand for all tasks from 0 to i-1
         for ( j = 0; j < i; j++ )
            anext += ceil( ( (double)an ) / ( (double)period[ j ] ) ) * wcet[ j ];
         // check if anext is the same as the total worst case excution time
         if ( anext == an )
            break;
         // else replace an with anext
         else
            an = anext;
      }
      // if demand is larger than deadline, no way this is feasible
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
   // iterate from highest to lowest priority
   for ( i = 0; i < numServices; i++ )
   {
      // status is false so that it does not break in the for loop
      status = 0;
      // loop through all 0<=k<=i, we start from 0
      // because service index from 0
      for ( k = 0; k <= i; k++ )
      {
         // left hand of inequalitym, also include the code
         // for figure out upper bound of l
         for ( l = 1; l <= ( floor( (double)period[ i ] / (double)period[ k ] ) ); l++ )
         {
            temp = 0;

            // time required by Sj to execute within l periods of Sk
            for ( j = 0; j <= i; j++ )
               temp += wcet[ j ] * ceil( (double)l * (double)period[ k ] / (double)period[ j ] );
            // if the sum of these times for the set of tasks is smaller than l
            // period of Sk, then the service set is feasible
            if ( temp <= ( l * period[ k ] ) )
            {
               status = 1;
               break;
            }
         }
         // just fall out of for loop since feasible
         if ( status )
            break;
      }
      // just fall out of for loop since feasible
      if ( !status )
         rc = FALSE;
   }
   // return success
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
   // Delcare all variables used
   U32_T lcm = getLcmOfPeriods( numServices, period );
   U32_T min;
   U32_T i;
   U32_T k;
   U32_T remainingComputationTime[ numServices ];
   U32_T timeUntilDeadline[ numServices ];

   // assign values for every service
   for ( i = 0; i < numServices; ++i )
   {
      remainingComputationTime[ i ] = wcet[ i ];
      timeUntilDeadline[ i ]        = deadline[ i ];
   }

   // We assume 1 second is the smallest interval for
   // checking if the service is the earliest deadline
   // every second.
   // This for loop dose loop every second until LCM.
   for ( k = 1; k <= lcm; k++ )
   {
      // get the earliest deadline service
      min = getIndexOfMin( timeUntilDeadline, numServices );
      // now computation time for the earliest deadline
      // service is advanced by 1 second, so remaining is
      // subtracted by 1 second
      if ( remainingComputationTime[ min ] > 0 )
      {
         remainingComputationTime[ min ]--;
      }
      // loop over all possible services to update all service status
      for ( i = 0; i < numServices; i++ )
      {
         // This just loop over make sure when we reach
         // LCM, we set deadline back to LCM, so that
         // the test can complete without error
         if ( remainingComputationTime[ i ] == 0 )
         {
            timeUntilDeadline[ i ] = lcm;
         }
         if ( timeUntilDeadline[ i ] > 0 )
         {
            // decrement deadline for all tasks since
            // we advanced a second
            timeUntilDeadline[ i ]--;
         }
         // Check if all services are able to finish computing before
         // deadline
         if ( timeUntilDeadline[ i ] < remainingComputationTime[ i ] )
         {
            // if cannot finish computting before deadline, return FALSE
            return FALSE;
         }
         // update timeUntilDeadline and remaining computation time
         if ( k % period[ i ] == 0 )
         {
            timeUntilDeadline[ i ]        = deadline[ i ];
            remainingComputationTime[ i ] = wcet[ i ];
         }
      }
   }
   return TRUE;
}

int least_laxity_first_scheduler( U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[] )
{
   // Delcare all variables used
   U32_T lcm = getLcmOfPeriods( numServices, period );
   U32_T min;
   U32_T i;
   U32_T k;
   U32_T remainingComputationTime[ numServices ];
   U32_T timeUntilDeadline[ numServices ];
   U32_T laxity[ numServices ];
   // assign values for every service
   for ( i = 0; i < numServices; ++i )
   {
      remainingComputationTime[ i ] = wcet[ i ];
      timeUntilDeadline[ i ]        = deadline[ i ];
   }
   // We assume 1 second is the smallest interval for
   // checking if the service is the earliest deadline
   // every second.
   // This for loop dose loop every second until LCM.
   for ( k = 1; k <= lcm; ++k )
   {
      // compute laxity for every services
      for ( i = 0; i < numServices; ++i )
      {
         laxity[ i ] = timeUntilDeadline[ i ] - remainingComputationTime[ i ];
      }
      // find the least laxity service
      min = getIndexOfMin( laxity, numServices );

      // Now computation time for the earliest deadline
      // service is advanced by 1 second, so remaining is
      // subtracted by 1 second
      if ( remainingComputationTime[ min ] > 0 )
      {
         remainingComputationTime[ min ]--;
      }

      // loop over all possible services to update all service status
      for ( i = 0; i < numServices; i++ )
      {
         // This just loop over make sure when we reach
         // LCM, we set deadline back to LCM, so that
         // the test can complete without error
         if ( remainingComputationTime[ i ] == 0 )
         {
            timeUntilDeadline[ i ] = lcm;
         }
         // Update deadline
         if ( timeUntilDeadline[ i ] > 0 )
         {
            // decrement deadline for all tasks since
            // we advanced a second
            timeUntilDeadline[ i ]--;
         }
         // Check if all services are able to finish computing before
         // deadline
         if ( timeUntilDeadline[ i ] < remainingComputationTime[ i ] )
         {
            // if cannot finish computting before deadline, return FALSE
            return FALSE;
         }
         // update timeUntilDeadline and remaining computation time if
         // service is requested again
         if ( k % period[ i ] == 0 )
         {
            timeUntilDeadline[ i ]        = deadline[ i ];
            remainingComputationTime[ i ] = wcet[ i ];
         }
      }
   }
   return TRUE;
}
