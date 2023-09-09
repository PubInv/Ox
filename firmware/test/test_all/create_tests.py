import os
import glob
import re
import shutil
from pathlib import Path 

HEADERS = '''#include <unity.h>
#ifdef ARDUINO
#include <Arduino.h>
#endif

'''

RUN1 = '''

void process() {
    UNITY_BEGIN();
'''
    
    #RUN_TEST(init);
RUN2 ='''    UNITY_END();
}

#ifdef ARDUINO
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);
    process();
}
void loop() {
    //
}
#else
int main(int argc, char **argv) {
    process();
    return 0;
}
#endif'''

#======================================#
# Assumptions:
# 1. test files are named test_*.cpp
#    eg. test_foo.cpp
# 2. tests are named test_*()
#    eg. test_bar()
#======================================#

#cwd = os.getcwd()
#tmpDir = os.path.join(cwd, '.tmp')
#os.mkdir(tmpDir)
cwd = '..'

# Build a list of test function calls
test_funcs = []
tests = glob.glob('../../lib/**/test_*.cpp', recursive=True)
test_headers = glob.glob('../../lib/**/test_*.h', recursive=True)

header_includes = []

if len(test_headers) == 0:
    print("No tests found. Exiting...")
    exit()


# for t in tests:
#     print(t)
#     with open(t) as temp_f:
#         data = temp_f.readlines()
#     for line in data:
#         m = re.search("(test_)\w+(\(\))", line) # search for: test_*()
#         if m != None:
#             print(m.group(0))

# get test function declarations from the header files
for t in test_headers:
    print(t)
    h = re.search("(test_)\w+(.h)", t) # search for: test_*.h
    # print(h.group(0))
    header_includes.append(h.group(0))

    with open(t) as temp_f:
        data = temp_f.readlines()
    for line in data:
        m = re.match(r"^void test_(\w+)(\(\))", line, re.MULTILINE) # search for: void test_*()
        if m != None:
            test_func = m.group(0)
            print(test_func)
            test_funcs.append(test_func)

# Format test functions and wrap in macro
run_test_funcs = []
for f in test_funcs:        
    # print(f)
    test_func_name = f.replace("()", "")
    test_func_name= test_func_name.replace("void ", "")
    run_test_str = "    RUN_TEST(" + test_func_name + ");\n"
    #print(run_test_str)
    run_test_funcs.append(run_test_str)


# Write runner cpp
runner = open('test_all.cpp', 'w')
runner.write(HEADERS)
for h in header_includes:
    runner.write('#include "' + h + '\"\n')

runner.write(RUN1)
runner.writelines(run_test_funcs)
runner.write(RUN2)

# Write h
# GUARD_H = '''#ifndef RUNNER_H
# #define RUNNER_H

# '''
# x = open('test_all.h', 'w')
# x.write(GUARD_H)
# for y in test_funcs:
#     x.write('void ' + y + ';\n')
# x.writelines('\n#endif\n')


# Copy test files
for t in tests:
    shutil.copyfile(t, Path(t).name)
for h in test_headers:
    shutil.copyfile(h, Path(h).name)
