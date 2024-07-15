source tool/generators/registers.tcl

proc register_tokens {registers} {
    proc generate_register_token_batch {batch} {
        puts [format "%%token %s" [string toupper $batch]]
    }

    foreach {key value} $registers {
        generate_register_token_batch [join $value " "]
    }
}

register_tokens  $registers
