all: 1.out 2.out 3.out 4.out 5.out

1.out: 1.cpp
	g++ -Wall -pedantic -o 1.out 1.cpp

2.out: 2.cpp
	g++ -Wall -pedantic -o 2.out 2.cpp

3.out: 3.cpp ExtEuc.cpp EllipCurveUtils.cpp CorrectModulus.cpp
	g++ -Wall -pedantic -o 3.out ExtEuc.cpp EllipCurveUtils.cpp CorrectModulus.cpp 3.cpp

4.out: 4.cpp ExtEuc.cpp EllipCurveUtils.cpp CorrectModulus.cpp
	g++ -g -Wall -pedantic -o 4.out ExtEuc.cpp EllipCurveUtils.cpp CorrectModulus.cpp 4.cpp

5.out: 5.cpp ExtEuc.cpp EllipCurveUtils.cpp CorrectModulus.cpp
	g++ -Wall -pedantic -o 5.out ExtEuc.cpp EllipCurveUtils.cpp CorrectModulus.cpp 5.cpp