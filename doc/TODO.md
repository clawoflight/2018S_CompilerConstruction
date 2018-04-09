# General Feedback Corrections:

* only one Readme which contains 
  * list of all required dependencies    
  * instructions on how to build everything and execute unit / integration tests   
  * a section listing known issues
  
* library symbols: Double check that all exported symbols of your library are prefixed. Have a look using nm -g --defined-only libmCc.so. If one of your symbols has no prefix, either add one or make it static.


# Assignment 1

## Tasks 1 and 2: AST, lexer, parser


###Primitives

~~alpha            = /[a-zA-Z_]/~~

~~alpha_num        = /[a-zA-Z0-9_]/~~

~~digit            = /[0-9]/~~

~~identifier       = alpha , [ { alpha_num } ]~~

~~bool_literal     = "true" | "false"~~

~~int_literal      = { digit }~~

~~float_literal    = { digit } , "." , { digit }~~

~~string_literal   = /"[^"]*"/~~


### Operators

~~unary_op         = "-" | "!"~~

~~binary_op        = "+"  | "-" | "*" | "/" 
                 | "<"  | ">" | "<=" | ">=" 
                 | "&&" | "||" 
                 | "==" | "!="~~


### Types

~~type             = "bool" | "int" | "float" | "string"~~


### Declaration / Assignment

~~declaration      = type , [ "[" , int_literal , "]" ] , identifier~~

~~assignment       = identifier , [ "[" , expression , "]" ] , "=" , expression~~


### Expressions

~~expression       = single_expr , [ binary_op , expression ]~~

~~single_expr      = literal
                 | identifier , [ "[" , expression , "]" ]
                 | call_expr
                 | unary_op , expression
                 | "(" , expression , ")"~~

~~literal          = bool_literal
                 | int_literal
                 | float_literal
                 | string_literal~~

### Statements

~~statement        = if_stmt
                 | while_stmt  
                 | ret_stmt  
                 | declaration , ";"
                 | assignment  , ";"
                 | expression  , ";" 
                 | compound_stmt ~~

~~if_stmt          = "if" , "(" , expression , ")" , statement , [ "else" , statement ]~~

~~while_stmt       = "while" , "(" , expression , ")" , statement~~

~~ret_stmt         = "return" , [ expression ] , ";"~~

~~compound_stmt    = "{" , [ { statement } ] , "}"~~


### Function Definition / Call

~~function_def     = ( "void" | type ) , identifier , "(" , [ parameters ] , ")" , compound_stmt~~

~~parameters       = declaration , [ { "," , declaration } ]~~

~~call_expr      = identifier , "(" , [ arguments ] , ")"~~

~~arguments        = expression , [ { "," expression } ]~~


### Program

~~program          = [ { function_def } ]~~


## Task 3: Error handling
~~- [Location tracking](https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html#Tracking-Locations)
  and also [SO](https://stackoverflow.com/questions/22407730/bison-line-number-included-in-the-error-messages) and [IBM](https://www.ibm.com/developerworks/library/l-flexbison/index.html)~~
~~- Kill memory leaks with fire! --> Always handle `malloc` failures, and whenever a constructor returns `NULL`, destroy all children first!~~
~~-Error handling ~~
~~- parser: pure (re-entrant)~~

## Task 4: Examples

# Assignment 2 

## Task 0 
integrate all the test inputs and test if all inputs are parsed correctly.

## Task 1
Find out what symbol tables are and how they are typically used.

Implement the creation of symbol tables.

## Task 2
Implement a check to detect uses of unknown functions.

Implement a check which ensures there is one function named main present. It should not take any parameters and return an int.

Implement a check which ensures that every function has a unique name. Also take the built-in functions into account.

## Task 3
Implement a check which ensures that all execution paths of a function return a value, unless the function is declared to return void

## Task 4
Implement type checking. (also for return types)

## Task 5 
Implement the conversion from AST (including symbol tables) to three-address code (TAC).

Implement a way to output the generated TAC

## Bonus Task
Implement some form of trace output for the type checking process. 

Provide an executable to dump symbol tables in a human readable format.

Use static single assignment (SSA).
