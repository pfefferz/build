puts -nonewline "Connecting to $env(TARGET)... "
connect -url "$env(TARGET)"
puts "connected"

targets -set -nocase -filter {name =~ "*PSU*"}
stop
mwr  0xff5e0200 0x0100
rst -system

puts "Ready for JTAG boot"
