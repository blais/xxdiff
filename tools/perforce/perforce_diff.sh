#!/bin/sh

#########################################################
# Author:      Christian Fowler <cafweb@lulu.com>
# Description: cracks open xxdiff given two perforce file versions
###

if [ $# -ne 2 ]; then
        echo "Usage: $0 file#rev1 file#rev2"
        echo "   or: $0 file@change2 file@change2"
        exit 1
fi

p4 print -q $1 > /tmp/p4-$1
p4 print -q $2 > /tmp/p4-$2
xxdiff /tmp/p4-$1 /tmp/p4-$2
rm /tmp/p4-$1
rm /tmp/p4-$2
