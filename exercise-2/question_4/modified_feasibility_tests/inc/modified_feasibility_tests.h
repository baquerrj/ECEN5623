#ifndef MODIFIED_FEASIBILITY_TESTS_H
#define MODIFIED_FEASIBILITY_TESTS_H

#define U32_T unsigned int
#define FALSE 0
#define TRUE 1

int completion_time_feasibility( U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[] );
int scheduling_point_feasibility( U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[] );
int earliest_deadline_first_scheduler( U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[] );
int least_laxity_first_scheduler( U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[] );
void print_set_information( U32_T numExercise, U32_T numServices, U32_T period[], U32_T wcet[], U32_T deadline[] );

#endif  // MODIFIED_FEASIBILITY_TESTS_H
