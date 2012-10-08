EXEC = ../exec/afl
# Getting all files finished by .cpp and put it on files variable
FILES = $(wildcard *.cpp)
# Getting all files from files variable, replacing .cpp by .o and put it on objs variable
OBJS = $(patsubst %.cpp,%.o,$(FILES))

CC = g++
INCLUDES = -I../include

default: src
src: $(EXEC)
all: src

$(EXEC): Makefile $(OBJS)
	-@echo 'Generating executable'
	-@$(CC) $(CFLAGS) $(INCLUDES) -o $(EXEC) $(OBJS)
	-@mv $(EXEC) ../exec/$(EXEC)

.cpp.o:
	-$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean:
	-@rm -f $(OBJS) *~
