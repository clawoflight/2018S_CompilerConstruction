# General Feedback Corrections:

* only one Readme which contains 
  * list of all required dependencies    
  * instructions on how to build everything and execute unit / integration tests   
  * a section listing known issues
  
* library symbols: Double check that all exported symbols of your library are prefixed. Have a look using nm -g --defined-only libmCc.so. If one of your symbols has no prefix, either add one or make it static.

* Update integration script: https://github.com/W4RH4WK/mCc/commit/d4d9fef8fc4e1e4f1d393b032dc2f26a4d6c70eb

# Assignment 2 

## Task 0 
~~integrate all the test inputs and test if all inputs are parsed correctly.~~

## Task 1
~~Find out what symbol tables are and how they are typically used.~~

~~Implement the creation of symbol tables.~~

~~extend tests~~

## Task 2
~~Implement a check to detect uses of unknown functions in every scope~~

~~Implement a check which ensures there is one function named main present. It should not take any parameters and return void.~~

~~Implement a check which ensures that every function has a unique name. Also take the built-in functions into account.~~

~~tests~~

## Task 3
~~Implement a check which ensures that all execution paths of a function return a value, unless the function is declared to return void~~

~~tests~~

## Task 4
~~Implement type checking. (also for return types)~~

~~tests~~

## Task 5 
Implement the conversion from AST (including symbol tables) to three-address code (TAC).

Implement a way to output the generated TAC~~

tests

## Bonus Task
Implement some form of trace output for the type checking process. 

Provide an executable to dump symbol tables in a human readable format.

Use static single assignment (SSA).
