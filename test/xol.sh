#!/bin/bash

set -xe

fasm heyo.asm heyo_asm
./../eaxhla heyo.eax
mv a.out heyo_eax
xop heyo_eax > heyo_eax_xop.txt
xop heyo_asm > heyo_asm_xop.txt
diff heyo_eax_xop.txt heyo_asm_xop.txt

exit

