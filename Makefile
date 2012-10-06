EXEC = afl
# Getting all files finished by .cpp and put it on files variable
FILES = $(wildcard *.cpp)
# Getting all files from files variable, replacing .cpp by .o and put it on objs variable
OBJS = $(patsubst %.cpp,%.o,$(FILES))

CC = g++

all: $(EXEC)

$(EXEC) : Makefile $(OBJS)
	-@echo 'Generating executable'
	-@$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

.cpp.o :
	-@$(CC) $(CFLAGS) -c $<

clean :
	-@echo 'Cleaning ...'
	-@rm -f $(EXEC) $(OBJS) *~
