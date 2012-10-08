
default: all
all: Makefile unit_tests.py
	-@cd ../src; make src "CFLAGS=-O3"
	-@python unit_tests.py $(EXEC)

clean:
	-@rm -f *~ $(EXEC)
	-@cd ../src; make clean
	-@rm -f *.dat *.net
