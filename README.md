# 2018S_CompilerConstruction
Homework for compiler construction

## Requirments

* Meson in a recent version (0.44.0) (you may want to install it via pip3 install --user meson)

* Ninja

* time, typically located at /usr/bin/time, do not confuse with the Bash built-in

* flex for generating the lexer

* bison for generating the parser

* a compiler supporting C11 (and C++14 for unit tests) -- typically GCC or Clang
  (We currently only support GCC inside mCc when compiling assembly, but our sources should compile fine with any compiler)

* doxygen (optional), to generate HTML documentation.

## Building
First get the Code of the compiler:

```
$ git clone https://github.com/clawoflight/2018S_CompilerConstruction.git
$ cd mCc
```
Then setup the build system:
```
meson builddir
cd builddir
```
Then call ninja to compile.
```
ninja 
```

## Execution
The compiler can be called with `mCc`, **see it's help message for details**.

`mC_to_dot` is provided to visualize the AST. It can only read from stdin and write to stdout.
```
./mCc --help
./mC_to_dot
```
The unit tests can be found in `test` and can be run with:
```
ninja test
ninja testgrind # tests behind valgrind
```
Integration tests can be run with:
```
../test/integration
```
In order dump the symbol tables, `--print-symtab` and the corresponding output may be specified. `-` or no argument for stdout is supported.
The symbol tables will be printed as markdown table.
```
./mCc ackermann.mC --print-symtab=st-out.md

# Generate HTML
./mCc ackermann.mC --print-symtab | markdown > st-out.html
./mCc ackermann.mC --print-symtab | pandoc -f markdown -o st-out.html
```

In order to print the Three Address Code (TAC), `--print-tac` and the corresponding output may be specified. `-` or no argument for stdout is supported.
```
./mCc ackermann.mC --print-tac tac-out.txt

# decent syntax highlighting:
./mCc ackermann.mC --print-tac | vim -c "setf asm" -
./mCc ackermann.mC --print-tac | highlight -O xterm256 --syntax asm | less
./mCc ackermann.mC --print-tac | highlight --syntax asm > tac-out.html
```

The generated assembly can also be printed using `--print-asm`, and it is also stored in `a.s` during normal compilation.

The control-flow graphs can be printed in DOT format using `--print-cfg`.

# Kown issues
- Due to lack of time, overall code quality went down a notch. Sorry.
- Insufficient error checking during TAC construction.
- No unit tests for TAC and ASM because time. sorry. For the latter (and now the former), integration tests are close to good enough though.

# Remarks
- **Floats are returned in %eax instead of the FPU**. We did this so we can treat everything the same to save time, and adapted `read_float` accordingly: it returns the float as a `long` without converting it.
  This is our only deviation from the cdecl calling convention.
