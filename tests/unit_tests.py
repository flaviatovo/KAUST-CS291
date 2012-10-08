import subprocess
import sys

subprocess.call(["echo", "Starting tests"])
subprocess.call(["cp", "../exec/"+sys.argv[1],"./"+sys.argv[1]])
