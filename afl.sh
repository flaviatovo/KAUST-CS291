g++ -Wall -c Matrix.cpp
g++ -Wall -c Vector.cpp
g++ -Wall -c aflComponent.cpp
g++ -Wall -c aflAplicativo.cpp
g++ -Wall -c afl.cpp
g++ -Wall -o afl Matrix.o Vector.o aflComponent.o aflAplicativo.o afl.o
