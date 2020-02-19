# Exercise #2 – Service Scheduling Feasibility #
## Roberto Baquerizo and Chautao Wei ##

1. Create new user account on Dev Kit.
2. Explanation and critique of the Architecture of the Space Shuttle Primary Avionics Software System.
3. Describe the concept of the Cyclic Executive and how this compares to the Linux POSIX RT threading and RTOS approaches we have discussed.
4. Feasibility with Cheddar.
5. Contraints, assumptions and things we don't understand from RM LUB derivation.

#### Completion Test Feasibility Example
* Ex-0 U=0.73 (C1=1, C2=1, C3=2; T1=2, T2=10, T3=15; T=D)
FEASIBLE
* Ex-1 U=0.99 (C1=1, C2=1, C3=2; T1=2, T2=5, T3=7; T=D)
INFEASIBLE
* Ex-2 U=1.00 (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D)
INFEASIBLE
* Ex-3 U=0.93 (C1=1, C2=2, C3=3; T1=3, T2=5, T3=15; T=D)
FEASIBLE
* Ex-4 U=1.00 (C1=1, C2=1, C3=4; T1=2, T2=4, T3=16; T=D)
FEASIBLE
* Ex-5 U=1.00 (C1=1, C2=2, C3=1; T1=2, T2=5, T3=10; T=D)
FEASIBLE
* Ex-6 U=1.00 (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T/=D)
FEASIBLE
* Ex-7 U=1.00 (C1=1, C2=2, C3=4; T1=3, T2=5, T3=15; T=D)
FEASIBLE
* Ex-8 U=1.00 (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D)
INFEASIBLE
* Ex-9 U=1.00 (C1=1, C2=2, C3=4, C4=6; T1=6, T2=8, T3=12, T4=24; T=D)
FEASIBLE


#### Scheduling Point Feasibility Example
* Ex-0 U=0.73 (C1=1, C2=1, C3=2; T1=2, T2=10, T3=15; T=D)
FEASIBLE
* Ex-1 U=0.99 (C1=1, C2=1, C3=2; T1=2, T2=5, T3=7; T=D)
INFEASIBLE
* Ex-2 U=1.00 (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D)
INFEASIBLE
* Ex-3 U=0.93 (C1=1, C2=2, C3=3; T1=3, T2=5, T3=15; T=D)
FEASIBLE
* Ex-4 U=1.00 (C1=1, C2=1, C3=4; T1=2, T2=4, T3=16; T=D)
FEASIBLE
* Ex-5 U=1.00 (C1=1, C2=2, C3=1; T1=2, T2=5, T3=10; T=D)
FEASIBLE
* Ex-6 U=1.00 (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T/=D)
INFEASIBLE
* Ex-7 U=1.00 (C1=1, C2=2, C3=4; T1=3, T2=5, T3=15; T=D)
FEASIBLE
* Ex-8 U=1.00 (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D)
INFEASIBLE
* Ex-9 U=1.00 (C1=1, C2=2, C3=4, C4=6; T1=6, T2=8, T3=12, T4=24; T=D)
FEASIBLE


#### EDF Scheduling Example
* Ex-0 U=0.73 (C1=1, C2=1, C3=2; T1=2, T2=10, T3=15; T=D)
FEASIBLE
* Ex-1 U=0.99 (C1=1, C2=1, C3=2; T1=2, T2=5, T3=7; T=D)
FEASIBLE
* Ex-2 U=1.00 (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D)
FEASIBLE
* Ex-3 U=0.93 (C1=1, C2=2, C3=3; T1=3, T2=5, T3=15; T=D)
FEASIBLE
* Ex-4 U=1.00 (C1=1, C2=1, C3=4; T1=2, T2=4, T3=16; T=D)
FEASIBLE
* Ex-5 U=1.00 (C1=1, C2=2, C3=1; T1=2, T2=5, T3=10; T=D)
FEASIBLE
* Ex-6 U=1.00 (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D)
FEASIBLE
* Ex-7 U=1.00 (C1=1, C2=2, C3=4; T1=3, T2=5, T3=15; T=D)
FEASIBLE
* Ex-8 U=1.00 (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D)
FEASIBLE
* Ex-9 U=1.00 (C1=1, C2=2, C3=4, C4=6; T1=6, T2=8, T3=12, T4=24; T=D)
FEASIBLE


#### LLF Scheduling Example
* Ex-0 U=0.73 (C1=1, C2=1, C3=2; T1=2, T2=10, T3=15; T=D)
FEASIBLE
* Ex-1 U=0.99 (C1=1, C2=1, C3=2; T1=2, T2=5, T3=7; T=D)
FEASIBLE
* Ex-2 U=1.00 (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D)
FEASIBLE
* Ex-3 U=0.93 (C1=1, C2=2, C3=3; T1=3, T2=5, T3=15; T=D)
FEASIBLE
* Ex-4 U=1.00 (C1=1, C2=1, C3=4; T1=2, T2=4, T3=16; T=D)
FEASIBLE
* Ex-5 U=1.00 (C1=1, C2=2, C3=1; T1=2, T2=5, T3=10; T=D)
FEASIBLE
* Ex-6 U=1.00 (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D)
FEASIBLE
* Ex-7 U=1.00 (C1=1, C2=2, C3=4; T1=3, T2=5, T3=15; T=D)
FEASIBLE
* Ex-8 U=1.00 (C1=1, C2=1, C3=1, C4=2; T1=2, T2=5, T3=7, T4=13; T=D)
FEASIBLE
* Ex-9 U=1.00 (C1=1, C2=2, C3=4, C4=6; T1=6, T2=8, T3=12, T4=24; T=D)
FEASIBLE


*Requirements
 [link](Exercise-2-Requirements.pdf)*