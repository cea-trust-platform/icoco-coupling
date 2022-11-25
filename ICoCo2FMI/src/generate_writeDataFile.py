from __future__ import print_function

import sys

try:
    datafile = open(sys.argv[1])
    lines = datafile.readlines()
except FileNotFoundError:
    lines = []


with open('writeDataFile.h', 'w') as outfile:
    outfile.write("#include <fstream>\n")
    outfile.write("void    writeDataFile(std::string& datafile)\n")
    outfile.write("{\n")
    import os
    base = os.path.basename(sys.argv[1])
    outfile.write(" datafile=\"" + base + "\";\n")
    outfile.write(" std::ofstream f(datafile.c_str());\n")
    for line in lines:
        outfile.write(
            " f << \"" + line[:-1].replace('"', '\\"') + "\"<<std::endl;\n")

    outfile.write("f.close();\n")
    outfile.write("}\n")
