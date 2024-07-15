source tool/generators/instructions.tcl

proc make_scanner_instructions {is} {
    proc make_scanner_instruction {i} {
        puts [format "%-12s    { return IT%s; }" \
                    [lindex $i 0] \
                    [string toupper [lindex $i 0]]
            ]
    }

    set isc {}
    foreach i $is {
        lappend isc [lindex $i 0]
    }
    set is [lsort -unique $isc]

    foreach i $is {
        make_scanner_instruction $i
    }
}

puts "    */"

make_scanner_instructions $instructions
