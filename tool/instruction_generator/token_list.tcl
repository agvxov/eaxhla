source tool/instruction_generator/instructions.tcl

proc make_token_list {is} {
    proc uniq_instl {is} {
        set r []
        foreach i $is {
            lappend r [lindex $i 0]
        }
        return [lsort -unique $r]
    }
    puts -nonewline "%token"
    foreach i [uniq_instl $is] {
        puts -nonewline [format " IT%s" [string toupper [lindex $i 0]]]
    }
}

make_token_list $instructions
