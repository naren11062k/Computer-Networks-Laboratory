set ns [new Simulator]
$ns rtproto DV
set nf [open prac3.nam w]
$ns namtrace-all $nf
proc finish {} {
global nf ns
$ns flush-trace
close $nf
exec nam prac3.nam &
exit 0
}
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
$ns duplex-link $n0 $n1 10Mb 10ms DropTail
$ns duplex-link $n1 $n2 10Mb 10ms DropTail
$ns duplex-link $n2 $n3 10Mb 10ms DropTail
$ns duplex-link $n3 $n0 10Mb 10ms DropTail
$ns duplex-link $n0 $n2 10Mb 10ms DropTail
$ns duplex-link $n1 $n3 10Mb 10ms DropTail
$ns duplex-link-op $n0 $n1 orient right
$ns duplex-link-op $n1 $n2 orient down
$ns duplex-link-op $n2 $n3 orient left
$ns duplex-link-op $n3 $n0 orient up
$ns duplex-link-op $n0 $n2 orient right-down
$ns duplex-link-op $n1 $n3 orient left-down
set tcp [new Agent/TCP]
$tcp set class_ 2
$ns attach-agent $n0 $tcp
set sink [new Agent/TCPSink]
$ns attach-agent $n2 $sink
$ns connect $tcp $sink
set ftp [new Application/FTP]
$ftp attach-agent $tcp
$ftp set type_ FTP
$ftp set packet_size_ 1000
$ftp set rate_ 1mb
$ns at 1.0 "$ftp start"
$ns rtmodel-at 2.0 down $n0 $n2
$ns rtmodel-at 3.0 up $n0 $n2
$ns at 4.0 "$ftp stop"
$ns at 5.0 "finish"
$ns run
