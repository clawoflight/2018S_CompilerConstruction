# 2018S_CompilerConstruction
Homework for compiler construction

## Requirments

* Meson in a recent version (0.44.0) (you may want to install it via pip3 install --user meson)

* Ninja

* time, typically located at /usr/bin/time, do not confuse with the Bash built-in

* flex for generating the lexer

* bison for generating the parser

* a compiler supporting C11 (and C++14 for unit tests) -- typically GCC or Clang

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
The compiler can be called with `mCc`, see it's help message for details.
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

In order to print the Three Address Code (TAC), the flag --print-tac and the corresponding output may be specified. `-` for stdout is supported.
```
./mCc ackermann.mC --print-tac tac-out.txt

# decent syntax highlighting:
./mCc ackermann.mC --print-tac - | highlight -O xterm256 --syntax asm | less
./mCc ackermann.mC --print-tac - | highlight --syntax asm > tac-out.html
```

# Kown issues
