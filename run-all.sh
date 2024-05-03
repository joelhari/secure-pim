#! /bin/bash

available_dpus=$1

echo " ----- get-sys-info  ----------"
./get-sys-info.sh 2>&1 | tee get-sys-info.log

# echo " ----- execute-tests ----------"
# ./execute-tests.sh $available_dpus 2>&1 | tee execute-tests.log

# echo " ----- execute-add-tests ----------"
# ./execute-add-tests.sh $available_dpus 2>&1 | tee execute-add-tests.log

# echo " ----- execute-cmodulus-tests ----------"
# ./execute-cmodulus-tests.sh $available_dpus 2>&1 | tee execute-cmodulus-tests.log

# echo " ----- execute-ftt-layer-tests ----------"
# ./execute-ftt-layer-tests.sh $available_dpus 2>&1 | tee execute-ftt-layer-tests.log

echo " ----- execute-butterfly-loop-tests ----------"
./execute-butterfly-loop-tests.sh $available_dpus 2>&1 | tee execute-butterfly-loop-tests.log