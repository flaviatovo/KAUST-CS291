EXEC = afl
# Getting all files finished by .cpp and put it on files variable
# FILES = Matrix.cpp Vector.cpp
FILES = $(wildcard *.cpp)
# Getting all files from files variable, replacing .cpp by .o and put it on objs variable
OBJS = $(patsubst %.cpp,%.o,$(FILES))

CC = gcc

all: $(EXEC)

$(EXEC) : Makefile $(OBJS)
	-@echo 'Generating executable'
#	-@$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

.cpp.o :
	$(CC) $(CFLAGS) -c $<

clean :
	rm -f $(EXEC) $(OBJS) *~
