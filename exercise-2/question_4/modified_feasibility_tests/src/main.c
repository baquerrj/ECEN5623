#include <math.h>
#include <stdio.h>

#include <modified_feasibility_tests.h>

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

//int completion_time_feasibility( U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[] );
//int scheduling_point_feasibility( U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[] );
//void print_set_information( U32_T numExercise, U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[] );

int main( void )
{
   int i;
   U32_T numServices;

   printf( "******** Completion Test Feasibility Example\n" );

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

   numServices = sizeof( ex1_period ) / sizeof( U32_T );
   print_set_information( 1, numServices, ex1_period, ex1_wcet, ex1_period );
   if ( completion_time_feasibility( numServices, ex1_period, ex1_wcet, ex1_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex2_period ) / sizeof( U32_T );
   print_set_information( 2, numServices, ex2_period, ex2_wcet, ex2_period );
   if ( completion_time_feasibility( numServices, ex2_period, ex2_wcet, ex2_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex3_period ) / sizeof( U32_T );
   print_set_information( 3, numServices, ex3_period, ex3_wcet, ex3_period );
   if ( completion_time_feasibility( numServices, ex3_period, ex3_wcet, ex3_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex4_period ) / sizeof( U32_T );
   print_set_information( 4, numServices, ex4_period, ex4_wcet, ex4_period );

   if ( completion_time_feasibility( numServices, ex4_period, ex4_wcet, ex4_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex5_period ) / sizeof( U32_T );
   print_set_information( 5, numServices, ex5_period, ex5_wcet, ex5_period );

   if ( completion_time_feasibility( numServices, ex5_period, ex5_wcet, ex5_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex6_period ) / sizeof( U32_T );
   print_set_information( 6, numServices, ex6_period, ex6_wcet, ex6_deadline );

   if ( completion_time_feasibility( numServices, ex6_period, ex6_wcet, ex6_deadline ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex7_period ) / sizeof( U32_T );
   print_set_information( 7, numServices, ex7_period, ex7_wcet, ex7_period );
   if ( completion_time_feasibility( numServices, ex7_period, ex7_wcet, ex7_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex8_period ) / sizeof( U32_T );
   print_set_information( 8, numServices, ex8_period, ex8_wcet, ex8_period );
   if ( completion_time_feasibility( numServices, ex8_period, ex8_wcet, ex8_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex9_period ) / sizeof( U32_T );
   print_set_information( 9, numServices, ex9_period, ex9_wcet, ex9_period );
   if ( completion_time_feasibility( numServices, ex9_period, ex9_wcet, ex9_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   printf( "\n\n" );
   printf( "******** Scheduling Point Feasibility Example\n" );

   numServices = sizeof( ex0_period ) / sizeof( U32_T );
   print_set_information( 0, numServices, ex0_period, ex0_wcet, ex0_period );

   if ( scheduling_point_feasibility( numServices, ex0_period, ex0_wcet, ex0_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex1_period ) / sizeof( U32_T );
   print_set_information( 1, numServices, ex1_period, ex1_wcet, ex1_period );

   if ( scheduling_point_feasibility( numServices, ex1_period, ex1_wcet, ex1_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex2_period ) / sizeof( U32_T );
   print_set_information( 2, numServices, ex2_period, ex2_wcet, ex2_period );

   if ( scheduling_point_feasibility( numServices, ex2_period, ex2_wcet, ex2_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex3_period ) / sizeof( U32_T );
   print_set_information( 3, numServices, ex3_period, ex3_wcet, ex3_period );

   if ( scheduling_point_feasibility( numServices, ex3_period, ex3_wcet, ex3_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex4_period ) / sizeof( U32_T );
   print_set_information( 4, numServices, ex4_period, ex4_wcet, ex4_period );

   if ( scheduling_point_feasibility( numServices, ex4_period, ex4_wcet, ex4_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }
   numServices = sizeof( ex5_period ) / sizeof( U32_T );
   print_set_information( 5, numServices, ex5_period, ex5_wcet, ex5_period );

   if ( scheduling_point_feasibility( numServices, ex5_period, ex5_wcet, ex5_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex6_period ) / sizeof( U32_T );
   print_set_information( 6, numServices, ex6_period, ex6_wcet, ex6_deadline );

   if ( scheduling_point_feasibility( numServices, ex6_period, ex6_wcet, ex6_deadline ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex7_period ) / sizeof( U32_T );
   print_set_information( 7, numServices, ex7_period, ex7_wcet, ex7_period );
   if ( scheduling_point_feasibility( numServices, ex7_period, ex7_wcet, ex7_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex8_period ) / sizeof( U32_T );
   print_set_information( 8, numServices, ex8_period, ex8_wcet, ex8_period );
   if ( scheduling_point_feasibility( numServices, ex8_period, ex8_wcet, ex8_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex9_period ) / sizeof( U32_T );
   print_set_information( 9, numServices, ex9_period, ex9_wcet, ex9_period );
   if ( scheduling_point_feasibility( numServices, ex9_period, ex9_wcet, ex9_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   printf( "\n\n" );
   printf( "******** EDF Scheduling Example\n" );

   numServices = sizeof( ex0_period ) / sizeof( U32_T );
   print_set_information( 0, numServices, ex0_period, ex0_wcet, ex0_period );

   if ( earliest_deadline_first_scheduler( numServices, ex0_period, ex0_wcet, ex0_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex1_period ) / sizeof( U32_T );
   print_set_information( 1, numServices, ex1_period, ex1_wcet, ex1_period );

   if ( earliest_deadline_first_scheduler( numServices, ex1_period, ex1_wcet, ex1_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex2_period ) / sizeof( U32_T );
   print_set_information( 2, numServices, ex2_period, ex2_wcet, ex2_period );

   if ( earliest_deadline_first_scheduler( numServices, ex2_period, ex2_wcet, ex2_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex3_period ) / sizeof( U32_T );
   print_set_information( 3, numServices, ex3_period, ex3_wcet, ex3_period );

   if ( earliest_deadline_first_scheduler( numServices, ex3_period, ex3_wcet, ex3_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex4_period ) / sizeof( U32_T );
   print_set_information( 4, numServices, ex4_period, ex4_wcet, ex4_period );

   if ( earliest_deadline_first_scheduler( numServices, ex4_period, ex4_wcet, ex4_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }
   numServices = sizeof( ex5_period ) / sizeof( U32_T );
   print_set_information( 5, numServices, ex5_period, ex5_wcet, ex5_period );

   if ( earliest_deadline_first_scheduler( numServices, ex5_period, ex5_wcet, ex5_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex6_period ) / sizeof( U32_T );
   print_set_information( 6, numServices, ex6_period, ex6_wcet, ex6_period );

   if ( earliest_deadline_first_scheduler( numServices, ex6_period, ex6_wcet, ex6_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex7_period ) / sizeof( U32_T );
   print_set_information( 7, numServices, ex7_period, ex7_wcet, ex7_period );
   if ( earliest_deadline_first_scheduler( numServices, ex7_period, ex7_wcet, ex7_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex8_period ) / sizeof( U32_T );
   print_set_information( 8, numServices, ex8_period, ex8_wcet, ex8_period );
   if ( earliest_deadline_first_scheduler( numServices, ex8_period, ex8_wcet, ex8_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex9_period ) / sizeof( U32_T );
   print_set_information( 9, numServices, ex9_period, ex9_wcet, ex9_period );
   if ( earliest_deadline_first_scheduler( numServices, ex9_period, ex9_wcet, ex9_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   printf( "\n\n" );
   printf( "******** LLF Scheduling Example\n" );

   numServices = sizeof( ex0_period ) / sizeof( U32_T );
   print_set_information( 0, numServices, ex0_period, ex0_wcet, ex0_period );

   if ( least_laxity_first_scheduler( numServices, ex0_period, ex0_wcet, ex0_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex1_period ) / sizeof( U32_T );
   print_set_information( 1, numServices, ex1_period, ex1_wcet, ex1_period );

   if ( least_laxity_first_scheduler( numServices, ex1_period, ex1_wcet, ex1_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex2_period ) / sizeof( U32_T );
   print_set_information( 2, numServices, ex2_period, ex2_wcet, ex2_period );

   if ( least_laxity_first_scheduler( numServices, ex2_period, ex2_wcet, ex2_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex3_period ) / sizeof( U32_T );
   print_set_information( 3, numServices, ex3_period, ex3_wcet, ex3_period );

   if ( least_laxity_first_scheduler( numServices, ex3_period, ex3_wcet, ex3_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex4_period ) / sizeof( U32_T );
   print_set_information( 4, numServices, ex4_period, ex4_wcet, ex4_period );

   if ( least_laxity_first_scheduler( numServices, ex4_period, ex4_wcet, ex4_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }
   numServices = sizeof( ex5_period ) / sizeof( U32_T );
   print_set_information( 5, numServices, ex5_period, ex5_wcet, ex5_period );

   if ( least_laxity_first_scheduler( numServices, ex5_period, ex5_wcet, ex5_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex6_period ) / sizeof( U32_T );
   print_set_information( 6, numServices, ex6_period, ex6_wcet, ex6_period );

   if ( least_laxity_first_scheduler( numServices, ex6_period, ex6_wcet, ex6_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex7_period ) / sizeof( U32_T );
   print_set_information( 7, numServices, ex7_period, ex7_wcet, ex7_period );
   if ( least_laxity_first_scheduler( numServices, ex7_period, ex7_wcet, ex7_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex8_period ) / sizeof( U32_T );
   print_set_information( 8, numServices, ex8_period, ex8_wcet, ex8_period );
   if ( least_laxity_first_scheduler( numServices, ex8_period, ex8_wcet, ex8_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }

   numServices = sizeof( ex9_period ) / sizeof( U32_T );
   print_set_information( 9, numServices, ex9_period, ex9_wcet, ex9_period );
   if ( least_laxity_first_scheduler( numServices, ex9_period, ex9_wcet, ex9_period ) == TRUE )
   {
      printf( "FEASIBLE\n" );
   }
   else
   {
      printf( "INFEASIBLE\n" );
   }
}