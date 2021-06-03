set val(stop) 10.0 ;
set ns [new Simulator]
set tracefile [open p1.tr w]
$ns trace-all $tracefile
set namfile [open p1.nam w]
$ns namtrace-all $namfile
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]
$ns duplex-link $n0 $n1 100.0Mb 10ms DropTail
$ns queue-limit $n0 $n1 50
$ns duplex-link $n1 $n3 100.0Mb 10ms DropTail
$ns queue-limit $n1 $n3 50
$ns duplex-link $n3 $n5 100.0Mb 10ms DropTail
$ns queue-limit $n3 $n5 50
$ns duplex-link $n0 $n2 100.0Mb 10ms DropTail
$ns queue-limit $n0 $n2 50

$ns duplex-link $n2 $n4 100.0Mb 10ms DropTail
$ns queue-limit $n2 $n4 50
$ns duplex-link $n4 $n5 100.0Mb 10ms DropTail
$ns queue-limit $n4 $n5 50
$ns duplex-link $n2 $n3 100.0Mb 10ms DropTail
$ns queue-limit $n2 $n3 50
$ns duplex-link $n0 $n3 100.0Mb 10ms DropTail
$ns queue-limit $n0 $n3 50
$ns duplex-link-op $n0 $n1 orient right-up
$ns duplex-link-op $n1 $n3 orient right
$ns duplex-link-op $n3 $n5 orient right-down
$ns duplex-link-op $n0 $n2 orient right-down
$ns duplex-link-op $n2 $n4 orient right
$ns duplex-link-op $n4 $n5 orient right-up
$ns duplex-link-op $n2 $n3 orient right-up
$ns duplex-link-op $n0 $n3 orient right-up
$ns cost $n0 $n1 1
$ns cost $n0 $n2 1
$ns cost $n0 $n3 3
$ns cost $n1 $n0 1
$ns cost $n1 $n3 2
$ns cost $n2 $n0 1
$ns cost $n2 $n3 2

$ns cost $n2 $n4 1
$ns cost $n3 $n2 2
$ns cost $n3 $n1 2
$ns cost $n3 $n0 3
$ns cost $n3 $n5 3
$ns cost $n4 $n2 1
$ns cost $n4 $n5 3
$ns cost $n5 $n3 3
$ns cost $n5 $n4 3
set udp0 [new Agent/UDP]
$ns attach-agent $n0 $udp0
set null1 [new Agent/Null]
$ns attach-agent $n5 $null1
$ns connect $udp0 $null1
$udp0 set packetSize_ 1500
set cbr0 [new Application/Traffic/CBR]
$cbr0 attach-agent $udp0
$cbr0 set packetSize_ 1000
$cbr0 set rate_ 1.0Mb
$cbr0 set random_ null

$ns at 1.0 "$cbr0 start"
$ns at 2.0 "$cbr0 stop"
$ns rtproto LS
proc finish {} {
global ns tracefile namfile
$ns flush-trace
close $tracefile
close $namfile
exec nam p1.nam &
exit 0
}
$ns at $val(stop) "$ns nam-end-wireless $val(stop)"
$ns at $val(stop) "finish"
$ns at $val(stop) "puts \"done\" ; $ns halt"
$ns run
