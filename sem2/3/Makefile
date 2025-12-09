all: mac_calc.out mac_verify.out

mac_calc.out: mac_calc.cpp
	g++ -Wall -pedantic -o mac_calc.out mac_calc.cpp -L./libs/cryptoPP -lcryptopp

mac_verify.out: mac_verify.cpp
	g++ -Wall -pedantic -o mac_verify.out mac_verify.cpp -L./libs/cryptoPP -lcryptopp
