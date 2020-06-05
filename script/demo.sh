cd ..
mkdir -p build
mkdir -p output
cd build
cmake ..
make

cd ../output

./fastMapper -h 50 -w 60 -s 1 -N 2 -c 3 -l 1 -i ../samples/o3.png -o ../output/done.png  -t img