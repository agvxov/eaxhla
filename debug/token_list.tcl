source debug/instructions.tcl

proc make_token_list {is} {
    puts -nonewline "%token"
    foreach i $is {
        puts -nonewline [format " I%s" [string toupper [lindex $i 0]]]
    }
}

make_token_list $instructions
