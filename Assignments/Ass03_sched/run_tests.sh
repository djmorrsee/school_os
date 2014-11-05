repeat=25
echo "Running Low Load Tests"
#~ 
#~ echo "Running CPU Tests"
#~ echo "Running Normal"
#~ ./cpu $repeat 0 test/cpu_normal_low.txt
#~ echo "Running FIFO"
#~ ./cpu $repeat 1 test/cpu_fifo_low.txt
#~ echo "Running RR"
#~ ./cpu $repeat 2 test/cpu_rr_low.txt
#~ echo "\n"

echo "Running IO Tests"
echo "Running Normal"
./io $repeat 0 test/io_normal_low.txt
echo "Running FIFO"
./io $repeat 1 test/io_fifo_low.txt
echo "Running RR"
./io $repeat 2 test/io_rr_low.txt
echo "\n"

#~ echo "Running Mix Tests"
#~ echo "Running Normal"
#~ ./mix $repeat 0 test/mix_normal_low.txt
#~ echo "Running FIFO"
#~ ./mix $repeat 1 test/mix_fifo_low.txt
#~ echo "Running RR"
#~ ./mix $repeat 2 test/mix_rr_low.txt
#~ echo "\n"

repeat=250
echo "Running Medium Load Tests"

#~ echo "Running CPU Tests"
#~ echo "Running Normal"
#~ ./cpu $repeat 0 test/cpu_normal_medium.txt
#~ echo "Running FIFO"
#~ ./cpu $repeat 1 test/cpu_fifo_medium.txt
#~ echo "Running RR"
#~ ./cpu $repeat 2 test/cpu_rr_medium.txt
#~ echo "\n"

echo "Running IO Tests"
echo "Running Normal"
./io $repeat 0 test/io_normal_medium.txt
echo "Running FIFO"
./io $repeat 1 test/io_fifo_medium.txt
echo "Running RR"
./io $repeat 2 test/io_rr_medium.txt
echo "\n"

#~ echo "Running Mix Tests"
#~ echo "Running Normal"
#~ ./mix $repeat 0 test/mix_normal_medium.txt
#~ echo "Running FIFO"
#~ ./mix $repeat 1 test/mix_fifo_medium.txt
#~ echo "Running RR"
#~ ./mix $repeat 2 test/mix_rr_medium.txt


repeat=1000
echo "Running High Load Tests"

#~ echo "Running CPU Tests"
#~ echo "Running Normal"
#~ ./cpu $repeat 0 test/cpu_normal_high.txt
#~ echo "Running FIFO"
#~ ./cpu $repeat 1 test/cpu_fifo_high.txt
#~ echo "Running RR"
#~ ./cpu $repeat 2 test/cpu_rr_high.txt
#~ echo "\n"

echo "Running IO Tests"
echo "Running Normal"
./io $repeat 0 test/io_normal_high.txt
echo "Running FIFO"
./io $repeat 1 test/io_fifo_high.txt
echo "Running RR"
./io $repeat 2 test/io_rr_high.txt
echo "\n"

#~ echo "Running Mix Tests"
#~ echo "Running Normal"
#~ ./mix $repeat 0 test/mix_normal_high.txt
#~ echo "Running FIFO"
#~ ./mix $repeat 1 test/mix_fifo_high.txt
#~ echo "Running RR"
#~ ./mix $repeat 2 test/mix_rr_high.txt
