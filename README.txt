About:
	Program for analysis of circuits using Euler 'backward' method or Gear's second order method, user option, attached to the Newton-Raphson method using modified nodal analysis.
	This program was created as a final project for the course of Electrical Circuits 2 given by the Professor Antonio Carlos Moreirao de Queiroz at the Department of Electronic and Computation Engineering of the Polytechnic School of the Federal University of Rio de Janeiro in the first semester of 2007.

Authors:
	Ana Carolina Schmidt Borges
	Flavia Correia Tovo
	Leonardo Cidade Ribeiro

HOW TOs:
- compile
	run 'make' or 'make src' on current directory
- compile as debug
	run 'make debug' on current directory
- compile/run profiling
	run 'make prof' on current directory
	results are stored in exec/prof.txt
- compile/run unit tests
	run 'make tests' on current directory
- clean
	run 'make clean' on current directory
- run code
	run './exec/afl' or './exec/afl filename' or 'make run' on current directory
	output files have the same names as input files but finish by '.dat'

WHERE:
- src
	all source files
- include
	all headers files
- exec
	where executable and profiling will be stored
- tests
	all unit tests
- files
	examples of input files with their output

WRITING NETLISTS (SPICE) (http://www.seas.upenn.edu/~jan/spice/spice.overview.html)
   Component               Syntax                              Obs
 Resistor               Rname N1 N2 Value
 Capacitor              Cname N1 N2 Value <IC>           IC is initial condition, N1 + and N2 -
 Inductor               Lname N1 N2 Value <IC>           IC is initial condition, N1 + and N2 -
 Voltage source         Vname N1 N2 TYPE                 TYPE in Source Types
 Current source         Iname N1 N2 TYPE                 TYPE in Source Types
 Diode                  Dname N+ N- <Is <Vt>>
 Bipolar transistor     Qname C B E Param1 Params        Param1 is NPN or PNP, Params is described below
 Voltage amplifier      Ename N1 N2 NC1 NC2 Value        Voltage-Controlled Voltage Source
 Transconductor         Gname N1 N2 NC1 NC2 Value        Voltage-Controlled Current source 
 Transresistor          Hname N1 N2 NC1 NC2 Value        Current-Controlled Voltage Source
 Current amplifier      Fname N1 N2 NC1 NC2 Value        Current-Controlled Current source

Source Types               
- Sinusoidal sources
     Syntax: SIN(O A FREQ TD THETA PHASE)
     Where:
       O - offset
       A - amplitude
       FREQ - the frequency in herz.
       TD - delay in seconds
       THETA - damping factor per second
       Phase - phase in degrees
- Pulse sources
     Syntax: PULSE(A1 A2 TD Tr Tf PW Period)
     Where:
       A1 - initial amplitude
       A2 - peak amplitude
       TD - initial delay time
       Tr - rise time
       Tf - fall time
       pwf - pulse-wise
       Period - period
- DC sources
     Syntax: DC(A)
       A - amplitude

Transistor Params (optional)
 Syntax: <alpha <alphar <Isbe <Vtbe <Isbc <Vtbc>>>>>>
       alpha - BJT efficiency
       alphar - 
       Isbe - Is of diode base-emissor
       Vtbe - Vt of diode base-emissor
       Isbc - Is of diode base-colector
       Vtbc - Vt of diode base-colector
