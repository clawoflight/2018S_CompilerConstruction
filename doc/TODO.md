# General Feedback Corrections:

# Assignment 3


## Task 1 
Implement Code Generation
The task is to convert your TAC to assembly code which can be compiled with the GNU Assembler.

## Task 2 
Implement back-end compiler invocation.

Now that we obtained assembly code we can convert it to an executable by running it through an assembler and linker. For this we'll simply pass it to GCC. This also allows us to attach the built-in functions, just pass the C source file to the compiler call.
Double check whether the built-in functions provided to your back-end compiler use the same calling convention.
You should now have a working compiler which can convert valid mC input programs to executables.

## Task 3

Polish the commandline parameter handling of your main compiler executable. At least the following flags must be provided:

  * -h / --help to display usage information
  * -v / --version to display version information
  * -o / --output to specify the resulting executable


## Task 4
Extend your integration test runner to also execute the generated binary. Pass the content of the corresponding .stdin file to the binary and compare its output with the corresponding .stdout file. Matching output indicates success.

Either use a temporary directory (mktemp) or the build directory to store the integration test executables.

