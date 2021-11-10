import os
import glob
import re

RUN1 = '''#include <unity.h>

void process() {
    UNITY_BEGIN();
'''
    
    #RUN_TEST(init);
RUN2 ='''    UNITY_END();
}

#ifdef ARDUINO
#include <Arduino.h>
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
cwd = '~/git/Ox_cpp/firmware'

# Build a list of test function calls
test_funcs = []
tests = glob.glob('/home/ben/git/Ox_cpp/firmware/**/test_*.cpp', recursive=True)

if len(tests) == 0:
    print("No tests found. Exiting...")
    exit()

for t in tests:
    print(t)
    with open(t) as temp_f:
        data = temp_f.readlines()
    for line in data:
        m = re.search("(test_)\w+(\(\))", line) # search for: test_*()
        if m != None:
            test_func = m.group(0)
            test_funcs.append(test_func)


# Format test functions and wrap in macro
run_test_funcs = []
for f in test_funcs:        
    #print("found: " + test_func)
    test_func_name = f.replace("()", "")
    run_test_str = "    RUN_TEST(" + test_func_name + ");\n"
    #print(run_test_str)
    run_test_funcs.append(run_test_str)

# Write runner cpp
runner = open('runner.cpp', 'w')
runner.write(RUN1)
runner.writelines(run_test_funcs)
runner.write(RUN2)