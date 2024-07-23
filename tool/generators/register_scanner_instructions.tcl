source tool/generators/registers.tcl

proc scan_registers {registers} {
    foreach {key value} $registers {
        foreach {register} $value {
            puts [format "%-6s    \{ return %s; \}"\
                    $register\
                    [string toupper $register]\
                ]
        }
    }
}

puts "    */"

scan_registers   $registers
