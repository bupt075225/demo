#!/bin/bash

# Play with the assemble and machine code

# Define color
BLUE='\E[1;34m'
YELLOW='\E[1;33m'
RES='\E[0m'       # reset color setting to default

CUR_DIR=`pwd`

function print_msg() {
    echo -e "${BLUE}$1\n${RES}"
}

# Using option -S, GCC generates the assemble file code.s
gcc -S $CUR_DIR/src/code.c

print_msg "---------Output assemble using gcc----------"
print_msg "gcc -S $CUR_DIR/src/code.c"
read -p "Show the assemble code:" user_reply
test [ $user_reply -ne "n" ] || cat code.s

# Using option -c, GCC generates the object file code.o
gcc -c $CUR_DIR/src/code.c

print_msg "---------Check the object file using objdump---------"
print_msg "objdump -d code.o"
read -p "Show the object file:" user_reply
test [ $user_reply -ne "n" ] || objdump -d code.o | more
print_msg "The length of instruct code from 1 up to 6 bytes:"
print_msg "\tInstruct Type Byte, Register Specifier Byte, Constant Word(four bytes)"
print_msg "Another method using GDB to check the object file:"
print_msg "$ gdb code.o"
print_msg "Then run gdb command:"
print_msg "(gdb) x/16xb sum"
print_msg "This means that checking 16 bytes hexadecimal at the begin of sum"
print_msg "Have a try pls."

print_msg "---------Generate the executable file prog------------"
print_msg "gcc -o prog code.o $CUR_DIR/src/main.c"
gcc -o prog code.o $CUR_DIR/src/main.c
print_msg "---------Disassemble prog------------"
read -p "Show the disassembled executable file:" user_reply
test [ $user_reply -ne "n" ] || objdump -d prog | more
