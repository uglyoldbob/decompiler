# decompiler
A decompiler targeting c and similar languages.

[![Build Status](https://travis-ci.org/uglyoldbob/decompiler.svg?branch=master)](https://travis-ci.org/uglyoldbob/decompiler)

[![Open Source Helpers](https://www.codetriage.com/uglyoldbob/decompiler/badges/users.svg)](https://www.codetriage.com/uglyoldbob/decompiler)

The goal of this program is to decompile an executable program to source code (represented by C), that can then be compiled back into a program that performs the same as the original.

**Generated programs**

*decompile16*

This is intended to target 16 bit executables.

*decompile32*

This is intended to target 32 bit executables.

*decompile64*

This is intended to target 64 bit executables.


**Getting started**

These are suggested steps for cloning and compiling. The steps vary depending on which version of git you are using. I am assuming you are using git from a command line of some variety.

git clone https://github.com/uglyoldbob/decompiler.git

cd decompiler


***Building***

cargo build

***Reference materials:***

**Books**

Dang, Bruce, et al. Practical Reverse Engineering: x86, x64, ARM, Windows Kernel, Reversing Tools, and Obfuscation. John Wiley & Sons, 2014. ISBN 978-1-118-78731-1
