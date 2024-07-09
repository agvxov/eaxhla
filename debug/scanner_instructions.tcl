source debug/instructions.tcl

proc make_scanner_instructions {is} {
    proc make_scanner_instruction {i} {
        puts [format "%s    { return I%s; }" \
                    [lindex $i 0] \
                    [string toupper [lindex $i 0]]
            ]
    }
    foreach i $is {
        make_scanner_instruction $i
    }
}

make_scanner_instructions $instructions
