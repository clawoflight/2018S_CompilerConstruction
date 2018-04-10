#!/bin/bash

cd builddir
for file in ~/CLionProjects/2018S_CompilerConstruction/doc/examples/*.mC
do
    ./mC_to_dot
done