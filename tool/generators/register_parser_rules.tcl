source tool/generators/registers.tcl

proc register_parsing {registers} {
    proc register_parser_rule {size batch} {
        puts [format "register%ss: %s \{ \$\$.number = GR0; \}"\
                    $size\
                    [string toupper [lindex $batch 0]]\
            ]
        set accumulator 1
        foreach register [lrange $batch 1 end] {
            puts [format "    | %-5s \{ \$\$.number = GR%s; \}"\
                    [string toupper $register]\
                    $accumulator\
                ]
            incr accumulator
        }
        puts "    ;"
        puts ""
    }

    foreach {key value} $registers {
        register_parser_rule $key $value
    }
}

register_parsing $registers
