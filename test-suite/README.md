# Testing
test_snd_rcv_all_loop.sh transmits a series of infrared codes 200 times, and irmpconfig receives them in test mode and writes them to files.
compare_testfiles.sh compares these files with a reference file and displays the differences.

test_snd_rcv_all_loop2_shortTimings.sh transmits the short timings 256 times, and test_snd_rcv_all_loop2_longTimings.sh transmits the long timings 256 times, and irmpconfig receives them in test2 mode and writes them to a file.

test_snd_rcv_all_loop3_shortTimings.sh transmits the short timings 20,000 times, and test_snd_rcv_all_loop3_longTimings.sh transmits the long timings 20,000 times, and irmpconfig receives them in test3 mode and writes them to a file.

Error-free test results can only be obtained under ideal conditions.
For example, a monitor with CCFL backlighting disrupts the test when turned on.
A fluorescent-based energy-saving lamp is even more disruptive.
The distance and angle between the transmitting LED and the TSOP also influence the result. Being too close to a strong transmitter is undesirable.
When using identical TSOPs under difficult conditions, one TSOP lost more and different protocols than the other.

I used a TSOP4838 for the long protocols and a TSOP34338 for the short ones.

For testing purposes, a wire can be used instead of a TSOP.
This eliminates errors caused by the TSOP and reveals other errors.
