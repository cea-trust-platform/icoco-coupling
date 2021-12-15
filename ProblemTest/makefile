all: check_python check_fmi check_fmi_step
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
check_fmi_step: test_step.fmu
	fmuCheck.linux64 -s 3. -h 0.1  -l 6 -i in_step.csv  -o res_step.csv test_step.fmu 

test.fmu: vars.json libTest.so
	../ICoCo2FMI/ICoCo2FMI.sh --datafile toto.data -v vars.json -n test -l $(PWD)/libTest.so
libTest.so:	ProblemTest.cxx 
	g++ -fPIC ProblemTest.cxx -I . -I ../include/  ../src/*.cpp  --shared -o libTest.so -g
test_step.fmu: vars_step.json libTest_step.so
	../ICoCo2FMI/ICoCo2FMI.sh --datafile toto.data -v vars_step.json -n test_step -l $(PWD)/libTest_step.so
libTest_step.so:	ProblemTest.cxx 
	g++ -fPIC -D_step_ ProblemTest.cxx -I . -I ../include/  ../src/*.cpp  --shared -o libTest_step.so -g
superv: superv.cpp
	g++ superv.cpp -I ../include/ -ldl -o superv -g
clean: 
	rm -f res*csv test*.fmu superv *.so ProblemTest.i ProblemTest.py   ProblemTest_wrap.*
	rm -fr build/ -r res.*.txt toto.data* test_daccosim-2* res_dacco.txt
daccosim: test_step.fmu test.fmu
	java -jar daccosim-shell-2021-v2.5.1.jar input:test_daccosim.dng
	sed "s/,/ /g;s/;/ /g;s/^time/#time/" `ls -rt test_daccosim-2*csv | tail -1`  > res_dacco.txt
graph:
	xmgr -nxy res_dacco*.txt res.*.txt
