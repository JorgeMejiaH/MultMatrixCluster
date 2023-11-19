#serial processing
#!/bin/bash

echo "Serial test in progress..."
echo "Size;Time" > Mult_Matrix.csv

for i in {1..10}; do
    echo "Running iteration $i"
    for j in 32 64 128 256 512 1024 2048; do
        result=$(./MultMatrixSerial $j)
        echo "$j;$result" >> Mult_Matrix.csv
    
    done
done

echo "" >> 'Mult_Matrix.csv'
echo "done"