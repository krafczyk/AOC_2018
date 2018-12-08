cp src/task_$1.cpp src/task_$2.cpp
for file in `ls problems/task_$1*`; do
	new_file=`echo ${file} | sed "s/$1/$2/"`
	cp ${file} ${new_file}
done;
for file in `ls tests/task_$1*`; do
	new_file=`echo ${file} | sed "s/$1/$2/"`
	cp ${file} ${new_file}
done;
