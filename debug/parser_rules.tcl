source debug/instructions.tcl

proc make_parser_rules {is} {
    proc init_iarg {a n} {
        set r [dict create]
        switch $a {
            "register"  {
                dict set r enum  "REG"
                dict set r value "\$$n.number"
                dict set r size  "\$$n.size"
            }
            "immediate" {
                dict set r enum  "IMM"
                dict set r value "(int)$e"
                dict set r size  "32"
            }
            "memory"    {
                dict set r enum  "MEM"
                dict set r value "0 /* ??? */"
                dict set r size  "0 /* ??? */"
            }
            default     { malformed_instruction $n }
        }
        return $r
    }
    proc make_parser_rule {i} {
        if {[llength $i] == 1} {
            set rule [format "    | IT%s { append_instruction_t1(%s); }" \
                [string toupper [lindex $i 0]] \
                [string toupper [lindex $i 0]] \
            ]
        } elseif {[llength $i] == 2} {
            set arg [init_iarg [lindex $i 1] 2]
            set rule [format "    | IT%s %s \{ append_instruction_t4(\
                                                    %s,\
                                                    %s,\
                                                    %s,\
                                                    %s\
                            ); \}" \
                [string toupper [lindex $i 0]] \
                [lindex $i 1]                  \
                \
                [string toupper [lindex $i 0]] \
                [dict get $arg size]           \
                [dict get $arg enum]           \
                [dict get $arg value]          \
            ]
        } elseif {[llength $i] == 3} {
            set arg1 [init_iarg [lindex $i 1] 2]
            set arg2 [init_iarg [lindex $i 2] 3]
            set rule [format "    | IT%s %s %s \{ append_instruction_t6(\
                                                    %s,\
                                                    %s,\
                                                    %s,\
                                                    %s,\
                                                    %s,\
                                                    %s\
                            ); \}" \
                [string toupper [lindex $i 0]] \
                [lindex $i 1]                  \
                [lindex $i 2]                  \
                \
                [string toupper [lindex $i 0]] \
                [dict get $arg1 size]          \
                [dict get $arg1 enum]          \
                [dict get $arg1 value]         \
                [dict get $arg2 enum]          \
                [dict get $arg2 value]         \
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

make_parser_rules $instructions
