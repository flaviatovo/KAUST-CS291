# Just in case putting all of the targets here
.PHONY: src debug prof tests
.SILENT:

EXEC = afl

default: src
all: src
src:
	-@echo 'Generating executable using 03'
	-@cd src; make src "CFLAGS=-O3"

debug:
	-@echo 'Generating executable using debug options'
	-@cd src; make src "CFLAGS=-g"

prof:
	-@echo 'Calling profiling'
	-@cd src; make src "CFLAGS=-pg"
	-@cd exec; ./$(EXEC); gprof -ba $(EXEC) > prof.txt; rm -f $(EXEC) gmon.out
	-@cd src; make clean

tests:
	-@echo 'Calling unit tests'

run:
	-@echo 'Running the code'
	-@cd exec; ./$(EXEC) $(FILENAME)

clean:
	-@echo 'Cleaning ...'
	-@rm -f *~
	-@rm -f exec/$(EXEC) exec/prof.txt
	-@cd src; make clean
