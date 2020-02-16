#include <math.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define U32_T unsigned int

// U=0.7333
U32_T ex0_period[] = {2, 10, 15};
U32_T ex0_wcet[]   = {1, 1, 2};

// U=0.9857
U32_T ex1_period[] = {2, 5, 7};
U32_T ex1_wcet[]   = {1, 1, 2};

// U=0.9967
U32_T ex2_period[] = {2, 5, 7, 13};
U32_T ex2_wcet[]   = {1, 1, 1, 2};

// U=0.93
U32_T ex3_period[] = {3, 5, 15};
U32_T ex3_wcet[]   = {1, 2, 3};

// U=1.0
U32_T ex4_period[] = {2, 4, 16};
U32_T ex4_wcet[]   = {1, 1, 4};

// U=1.0
U32_T ex5_period[] = {2, 5, 10};
U32_T ex5_wcet[]   = {1, 2, 1};

// U=0.9967033
U32_T ex6_period[]   = {2, 5, 7, 13};
U32_T ex6_wcet[]     = {1, 1, 1, 2};
U32_T ex6_deadline[] = {2, 3, 7, 15};

// U=1.0
U32_T ex7_period[] = {3, 5, 15};
U32_T ex7_wcet[]   = {1, 2, 4};

// U=0.9967033
U32_T ex8_period[] = {2, 5, 7, 13};
U32_T ex8_wcet[]   = {1, 1, 1, 2};

// U=1.0
// f0 = 0.041667
// f1 = 0.083333  (2*f0)
// f2 = 0.125     (3*f0)
// f3 = 0.166667  (4*f0)
U32_T ex9_period[] = {6, 8, 12, 24};
U32_T ex9_wcet[]   = {1, 2, 4, 6};

int completion_time_feasibility( U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[] );
int scheduling_point_feasibility( U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[] );
void print_set_information( U32_T numExercise, U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[] );

int main( void )
{
   int i;
   U32_T numServices;

   printf( "******** Completion Test Feasibility Example\n" );

   //printf( "Ex-0 U=%4.2f (C1=1, C2=1, C3=2; T1=2, T2=10, T3=15; T=D): ",
   //        ( ( 1.0 / 2.0 ) + ( 1.0 / 10.0 ) + ( 2.0 / 15.0 ) ) );
   numServices = sizeof( ex0_period ) / sizeof( U32_T );
   print_set_information( 0, numServices, ex0_period, ex0_wcet, ex0_period );
   if ( completion_time_feasibility( numServices, ex0_period, ex0_wcet, ex0_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   printf( "Ex-1 U=%4.2f (C1=1, C2=1, C3=2; T1=2, T2=5, T3=7; T=D): ",
           ( ( 1.0 / 2.0 ) + ( 1.0 / 5.0 ) + ( 1.0 / 7.0 ) ) );
   numServices = sizeof( ex1_period ) / sizeof( U32_T );
   if ( completion_time_feasibility( numServices, ex1_period, ex1_wcet, ex1_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   printf( "Ex-2 U=%4.2f (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D): ",
           ( ( 1.0 / 2.0 ) + ( 1.0 / 5.0 ) + ( 1.0 / 7.0 ) + ( 2.0 / 13.0 ) ) );
   numServices = sizeof( ex2_period ) / sizeof( U32_T );
   if ( completion_time_feasibility( numServices, ex2_period, ex2_wcet, ex2_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   printf( "Ex-3 U=%4.2f (C1=1, C2=2, C3=3; T1=3, T2=5, T3=15; T=D): ",
           ( ( 1.0 / 3.0 ) + ( 2.0 / 5.0 ) + ( 3.0 / 15.0 ) ) );
   numServices = sizeof( ex3_period ) / sizeof( U32_T );
   if ( completion_time_feasibility( numServices, ex3_period, ex3_wcet, ex3_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }
   printf( "Ex-4 U=%4.2f (C1=1, C2=1, C3=4; T1=2, T2=4, T3=16; T=D): ",
           ( ( 1.0 / 2.0 ) + ( 1.0 / 4.0 ) + ( 4.0 / 16.0 ) ) );
   numServices = sizeof( ex4_period ) / sizeof( U32_T );
   if ( completion_time_feasibility( numServices, ex4_period, ex4_wcet, ex4_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex9_period ) / sizeof( U32_T );
   print_set_information( 6, numServices, ex6_period, ex6_wcet, ex6_deadline );
   if ( completion_time_feasibility( numServices, ex6_period, ex6_wcet, ex6_deadline ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   printf( "\n\n" );
   printf( "******** Scheduling Point Feasibility Example\n" );

   printf( "Ex-0 U=%4.2f (C1=1, C2=1, C3=2; T1=2, T2=10, T3=15; T=D): ",
           ( ( 1.0 / 2.0 ) + ( 1.0 / 10.0 ) + ( 2.0 / 15.0 ) ) );
   numServices = sizeof( ex0_period ) / sizeof( U32_T );
   if ( scheduling_point_feasibility( numServices, ex0_period, ex0_wcet, ex0_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   printf( "Ex-1 U=%4.2f (C1=1, C2=1, C3=2; T1=2, T2=5, T3=7; T=D): ",
           ( ( 1.0 / 2.0 ) + ( 1.0 / 5.0 ) + ( 1.0 / 7.0 ) ) );
   numServices = sizeof( ex1_period ) / sizeof( U32_T );
   if ( scheduling_point_feasibility( numServices, ex1_period, ex1_wcet, ex1_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   printf( "Ex-2 U=%4.2f (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D): ",
           ( ( 1.0 / 2.0 ) + ( 1.0 / 5.0 ) + ( 1.0 / 7.0 ) + ( 2.0 / 13.0 ) ) );
   numServices = sizeof( ex2_period ) / sizeof( U32_T );
   if ( scheduling_point_feasibility( numServices, ex2_period, ex2_wcet, ex2_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   printf( "Ex-3 U=%4.2f (C1=1, C2=2, C3=3; T1=3, T2=5, T3=15; T=D): ",
           ( ( 1.0 / 3.0 ) + ( 2.0 / 5.0 ) + ( 3.0 / 15.0 ) ) );
   numServices = sizeof( ex3_period ) / sizeof( U32_T );
   if ( scheduling_point_feasibility( numServices, ex3_period, ex3_wcet, ex3_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   printf( "Ex-4 U=%4.2f (C1=1, C2=1, C3=4; T1=2, T2=4, T3=16; T=D): ",
           ( ( 1.0 / 2.0 ) + ( 1.0 / 4.0 ) + ( 4.0 / 16.0 ) ) );
   numServices = sizeof( ex4_period ) / sizeof( U32_T );
   if ( scheduling_point_feasibility( numServices, ex4_period, ex4_wcet, ex4_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }
}

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