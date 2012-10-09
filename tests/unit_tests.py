import subprocess # For calling shell functions
import sys # To get arguments
import ast # Used to convert from string to number
import math # Used to get abs of number

subprocess.call(["echo", "Starting tests"])
subprocess.call(["cp", "../exec/"+sys.argv[1],"./"+sys.argv[1]])

tests_results = list()
exit_code = 0

# Testing circuit 1 (only resistors)
subprocess.call(["cp", "../files_examples/circ1.net","./circ1.net"])
subprocess.call(["cp", "../files_examples/circ1_model.dat","./circ1_model.dat"])
exit_code = subprocess.call(["./"+sys.argv[1],"circ1.net"])

if (exit_code != 0):
    # if execution fails
    tests_results.append("FAIL")
else:
    # check if output is correct
    execution_file = list()
    execution_solution = list()
    model_file = list()
    model_solution = list()
    
    with open("circ1.dat","r") as file1:
        # reading solution file
        execution_file = [elem for elem in file1.read().split('\n') if elem]
    with open('circ1_model.dat', 'r') as file2:
        # reading model solution
        model_file = [elem for elem in file2.read().split('\n') if elem]
    
    for line in execution_file:
        execution_solution.append(filter (lambda a: a != "", line.split(' ')))
    for line in model_file:
        model_solution.append(filter (lambda a: a != "", line.split(' ')))

    execution_solution.pop(0)
    model_solution.pop(0)
    
    number_of_errors = 0
    for row_counter in range(0,len(execution_solution),1):
        for col_counter in range(0,len(execution_solution[row_counter]),1):
            exec_value = ast.literal_eval(execution_solution[row_counter][col_counter])
            model_value = ast.literal_eval(model_solution[row_counter][col_counter])
            # test if difference is bigger than 0.01%
            if (math.fabs(exec_value)-math.fabs(model_value))/model_value > 0.0001:
                ++number_of_errors
    
    if (number_of_errors > 0):
        tests_results.append("FAIL")
    else:
        tests_results.append("PASS")

# Printing tests results
for i in range(0,len(tests_results),1):
    print "   Test",i,tests_results[i]
print "Succeded", tests_results.count("PASS"), "of", len(tests_results), " (",tests_results.count("PASS")/len(tests_results)*100,"%)"
