make clean
make
echo "========GSHARE========"
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --gshare:13
echo ""
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --gshare:13
echo ""
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --gshare:13
echo ""
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --gshare:13
echo ""
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --gshare:13
echo ""
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --gshare:13
echo ""
echo "========Tournament========"
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --tournament:9:10:10
echo ""
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --tournament:9:10:10
echo ""
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --tournament:9:10:10
echo ""
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --tournament:9:10:10
echo ""
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --tournament:9:10:10
echo ""
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --tournament:9:10:10
echo ""
echo "========Custom========"
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --custom
echo ""
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --custom
echo ""
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --custom
echo ""
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --custom
echo ""
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --custom
echo ""
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --custom
echo ""
