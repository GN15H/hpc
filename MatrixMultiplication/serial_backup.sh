#! /bin/bash
echo "10,100,200,400,800,1600,3200" >csv/serial.csv
for j in {1..10}; do
  for i in 10 100 200 400 800 1600 3200; do
    ./serial.exe $i >>csv/serial.csv
  done
  echo "" >>csv/serial.csv
done
