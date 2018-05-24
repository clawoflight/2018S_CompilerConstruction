# General Feedback Corrections

- [ ] Fix type checker: *any* assignment to a variable with array type is invalid.

# Specific Feedback Corrections

- [x] Move uthash.h to src/

- [x] List all dependencies in README --> doxygen

- [x] Make doxygen optional

- [x] Parser: segfault for "1 + 1;"

Type checker -- missing:

- [ ] error on missing `return`
    - [ ] last statement not an if-statement
    - [ ] last statement is an if-statement (with else)
    - [ ] last statement is an if-statement (without else)

- [ ] Unit-tests for TAC, or at least explain why not

- [x] Uncomment assertions in tac.c

- [ ] Minimize scope of warning suppressing pragmas

# Assignment 3

## Task 1

- [ ] Implement code generation.

## Task 2

- [x] Back-end compiler invocation.

## Task 3

- [x] --help

--version

- [x] --output

## Task 4

- [x] Extend integration tests to run executable and compare output.

- [x] Store executables in temp dir (mktemp(1)) or build dir.


