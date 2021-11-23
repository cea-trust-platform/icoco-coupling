all: check_python check_fmi
ref:	superv libTest.so
	./superv | grep res > res.superv.txt
check_python : ref libTest.so
	sed "s/your_code.h/ProblemTest.hxx/;s/ProblemYourCode/ProblemTest/g;s/your_code_icoco/ProblemTest/" ../swig/your_code.i  > ProblemTest.i
	swig -python -c++ -I. -I../include -I../swig ProblemTest.i
	g++ ProblemTest_wrap.cxx  -fPIC  -I ../include/ -I/usr/include/python`python  -c "import sys; print (sys.version[:3])"` --shared -o _ProblemTest.so -L /usr/lib64/ -lpython3 ./libTest.so 
	python superv.py  | grep res > res.python.txt
check_fmi : ref test.fmu
	fmuCheck.linux64 -s 3. -h 0.1  -l 6 -i in.csv  -o res.csv test.fmu 
	sed "s/,/ /g;s/^\"/#\" /" res.csv > res.fmuCheck.txt
	fmpy simulate --stop-time 3. test.fmu  --input in.csv --output-interval 0.1  --fmi-logging --output-file res2.csv
	sed "s/,/ /g;s/^\"/#\" /" res2.csv > res.fmpy.txt

test.fmu: vars.json libTest.so
	../ICoCo2FMI/ICoCo2FMI.sh --datafile toto.data -v vars.json -n test -l $(PWD)/libTest.so
libTest.so:	ProblemTest.cxx 
	g++ -fPIC ProblemTest.cxx -I . -I ../include/  ../src/*.cpp  --shared -o libTest.so -g
superv: superv.cpp
	g++ superv.cpp -I ../include/ -ldl -o superv -g
clean: 
	rm -f res*csv test.fmu superv *.so ProblemTest.i ProblemTest.py   ProblemTest_wrap.*
	rm -fr build/ -r res.*.txt toto.data*
