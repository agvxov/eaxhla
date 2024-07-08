#!/bin/bash

set -xe

diff -s -y -t --color=auto flatten.txt xlatten.txt

exit
