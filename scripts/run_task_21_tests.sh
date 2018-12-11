echo "Single cell tests"
./bin/task_21 --val -x 3 -y 5 -s 8 -t 4
./bin/task_21 --val -x 122 -y 79 -s 57 -t -5
./bin/task_21 --val -x 217 -y 196 -s 39 -t 0
./bin/task_21 --val -x 101 -y 153 -s 71 -t 4
echo "Multi cell tests"
./bin/task_21 -x 33 -y 45 -s 18 -t 29
./bin/task_21 -x 21 -y 61 -s 42 -t 30
echo "Global maximizing tests"
./bin/task_21 -s 18 -t 29
./bin/task_21 -s 42 -t 30
