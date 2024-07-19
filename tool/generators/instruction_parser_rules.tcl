source tool/generators/instructions.tcl

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
                dict set r enum  "$$n.type"
                dict set r value "$$n.value"
                # XXX
                dict set r size  "D32"
            }
            "relative"  {
                dict set r enum  "REL"
                dict set r value "$$n"
                dict set r size  "D32"
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
        set token_name [string toupper [lindex $i 0]]
        if {[llength $i] == 1} {
            set rule [format "    | IT%s { append_instructions(%s); }" \
                $token_name \
                $token_name \
            ]
        } elseif {[llength $i] == 2} {
            set arg [init_iarg [lindex $i 1] 2]
            set rule [format "    | IT%s %s \{ append_instructions(\
                                                    %s,\
                                                    %s,\
                                                    %s,\
                                                    %s\
                            ); \}" \
                $token_name \
                [lindex $i 1]                  \
                \
                $token_name \
                [dict get $arg size]           \
                [dict get $arg enum]           \
                [dict get $arg value]          \
            ]
        } elseif {[llength $i] == 3} {
            set arg1 [init_iarg [lindex $i 1] 2]
            set arg2 [init_iarg [lindex $i 2] 3]
            set rule [format "    | IT%s %s %s \{ append_instructions(\
                                                    %s,\
                                                    %s,\
                                                    %s,\
                                                    %s,\
                                                    %s,\
                                                    %s\
                            ); \}" \
                $token_name \
                [lindex $i 1]                  \
                [lindex $i 2]                  \
                \
                $token_name \
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
