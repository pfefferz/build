puts -nonewline "Connecting to $env(TARGET)... "
connect -url "$env(TARGET)"
puts "connected"

source [file join $env(PETALINUX_BUILD_TOOLS) load_util.tcl]
reset

puts "Reset"
