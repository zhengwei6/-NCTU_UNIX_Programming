all: sandbox sandbox.so

sandbox.so: inject.cpp
	g++ -Wall  -shared -fPIC  -o sandbox.so inject.cpp -ldl

sandbox: sandbox.cpp test.cpp
	g++ -o test test.cpp
	g++ -o sandbox sandbox.cpp

clean:
	rm sandbox sandbox.so

