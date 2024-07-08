set instructions {
    {syscall}
    {nop}
    {sysret}
    {sysexit}
    {sysenter}
    {leave}
    {retf}
    {retn}
    {pause}
    {hlt}
    {lock}
}

    # {mov register immediate}
    
proc malformed_instruction {i} {
    error [format "Instruction {%s} is malformed" [join $i ", "]]
}

proc make_token_list {is} {
    puts -nonewline "%token"
    foreach i $is {
        puts -nonewline [format " I%s" [string toupper [lindex $i 0]]]
    }
}

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

proc make_parser_rules {is} {
    proc type_name_to_type_enum {n} {
        switch $n {
            "register" {
                return "REG"
            }
            "immediate" {
                return "IMM"
            }
            "memory" {
                return "MEM"
            }
            default {
                malformed_instruction $n
            }
        }
    }
    proc type_name_to_type_value {n e} {
        switch $n {
            "register" {
                return "$e.number"
            }
            "immediate" {
                return "(int)$e"
            }
            "memory" {
                return "???"
            }
            default {
                malformed_instruction $n
            }
        }
    }
    proc make_parser_rule {i} {
        if {[llength $i] == 1} {
            set rule [format "    | I%s { append_instruction_t1(%s); }" \
                [string toupper [lindex $i 0]] \
                [string toupper [lindex $i 0]]
            ]
        } elseif {[llength $i] == 3} {
            set rule [format "    | I%s %s %s \{ append_instruction_t4(\
                                                    %s,\
                                                    \$2.size,\
                                                    %s,\
                                                    %s,\
                                                    %s,\
                                                    %s\
                            );" \
                [string toupper [lindex $i 0]] \
                [lindex $i 1]                  \
                [lindex $i 2]                  \
                \
                [string toupper [lindex $i 0]] \
                [type_name_to_type_enum  [lindex $i 1]]       \
                [type_name_to_type_value [lindex $i 1] "\$2"] \
                [type_name_to_type_enum  [lindex $i 2]]       \
                [type_name_to_type_value [lindex $i 2] "\$3"] \
            ]
        } else {
            malformed_instruction $i
        }
        puts $rule
    }

    foreach i $is {
        make_parser_rule $i
    }
}

make_token_list $instructions

puts ""

make_scanner_instructions $instructions

puts ""

make_parser_rules $instructions
