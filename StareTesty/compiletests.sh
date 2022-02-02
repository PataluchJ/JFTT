#!/bin/bash
SCRIPT=$(readlink -f "$0")
# Absolute path this script is in, thus /home/user/bin
SCRIPTPATH=$(dirname "$SCRIPT")
echo $SCRIPTPATH
cd $SCRIPTPATH
../compiler Sources/0-div-mod.imp Compiled/0.mr
../compiler Sources/1-numbers.imp Compiled/1.mr
../compiler Sources/2-fib.imp Compiled/2.mr
../compiler Sources/3-fib-factorial.imp Compiled/3.mr
../compiler Sources/4-factorial.imp Compiled/4.mr
../compiler Sources/5-tab.imp Compiled/5.mr
../compiler Sources/6-mod-mult.imp Compiled/6.mr
../compiler Sources/7-loopiii.imp Compiled/7.mr
../compiler Sources/8-for.imp Compiled/8.mr
../compiler Sources/9-sort.imp Compiled/9.mr
../compiler Sources/program0.imp Compiled/p0.mr
../compiler Sources/program1.imp Compiled/p1.mr
../compiler Sources/program2.imp Compiled/p2.mr
