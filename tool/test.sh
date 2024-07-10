#!/bin/bash

set -xe

#~diff -y -W 40 --ignore-trailing-space --ignore-blank-lines --suppress-common-lines --color=always flatten.txt xlatten.txt
#~diff -y -W 40 --ignore-trailing-space --ignore-blank-lines --color=always flatten.txt xlatten.txt
#~diff -y flatten.txt xlatten.txt
diff flatten.txt xlatten.txt

exit
