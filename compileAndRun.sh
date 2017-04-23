#! /bin/sh
#
# compile.sh
# Copyright (C) 2017 blueyi <blueyi@blueyi-mint>
#
# Distributed under terms of the MIT license.
#

g++ -fPIC -shared -o libIceAnalysis.so cca.cpp -I$JAVA_HOME/include/ -I$JAVA_HOME/include/linux/ -std=c++11

javac win/dutoe/TestCCA.java

java -Djava.library.path=. win.dutoe.TestCCA $1



