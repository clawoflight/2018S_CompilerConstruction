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
Then build meson 
```
meson builddir
cd builddir
```
Now build ninja
```
ninja 
```

## Execution
The parser has two mains. The first one takes a file as imput. The second one takes the input to compile via the std. input of the user.

```
./mCc filname.c
./mC_to_dot
```
The unit test can be found in /test and can all be run with :
```
ninja test 
```

All the integration tests can be run with both main programs either providing the input file or by coping the code into the std. input. 
```
./mC /doc/examples/ackermann.mC
./mC_to_dot /doc/examples/ackermann.mC
```
To run all integration test a script is provided named test-i.sh, which can run both main programs with all integration examples. In the script the selected main program is currently mC_to_dot this can be switched to mC with a file input in order to test the examples with the mC program
```
./test-i.sh
``` 

# Kown issues
