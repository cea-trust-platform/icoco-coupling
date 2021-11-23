# A simple ProblemTest

## EDP
dy/dt= a -b y

with:

* y(0)=0
* a(0)=1
* b(0)=0


## scenrio
* a set to 0 à t=1  
* b set to 4 à t=2  

## solution
* t<1 y(t)=t
* 1<= t<=2 y(t)=1
* 2<=t y(t)=exp(-4*(t-2))


## tests
* c++ with superv.cpp
* python with superv.py
* FMI with in.csv and vars.json (results ok with fmpy, ko with fmufmuCheck)
