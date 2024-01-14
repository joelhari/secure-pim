
TEST_DIR=./build/tests

if [ \( ! -d "$TEST_DIR" \) -o \( -z "$(ls -A $TEST_DIR)" \) ]; then
    echo "no tests found"
    echo "run 'make tests' to compile the tests"
else
    echo "running all tests"
    for file in $TEST_DIR/*
    do
        echo "========================================================="
        echo "run test $file"
        "$file"
    done
    echo "========================================================="
fi




