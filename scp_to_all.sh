#!/bin/bash

FILE=$1
PATH=$2
echo "file: $FILE"
echo "path: $PATH"
echo "continue? Ctl+c to quit"
read resp

scp $FILE pi@192.168.1.21:$PATH
scp $FILE pi@192.168.1.22:$PATH
scp $FILE pi@192.168.1.23:$PATH
scp $FILE pi@192.168.1.31:$PATH
scp $FILE pi@192.168.1.32:$PATH
scp $FILE pi@192.168.1.33:$PATH

