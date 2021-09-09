#!/bin/bash
# tests/scripts/temp.sh while_dynamic.tflite "1 2 3 4"

SHELL_PATH=`pwd -P`
TFLITE_PATH="$1"
NODES=$2

mkdir "$SHELL_PATH/test_tmp"
TEMP_PATH="$SHELL_PATH/test_tmp"

# tflite to circle
$SHELL_PATH/build/compiler/tflite2circle/tflite2circle $TFLITE_PATH $TEMP_PATH/origin.circle

# trim tflite
echo $NODES > $TEMP_PATH/opcodelist.txt
python3 $SHELL_PATH/tools/tflitefile_tool/select_operator.py $TFLITE_PATH $TEMP_PATH/opcodelist.txt $TEMP_PATH/trimmed.tflite

# trim circle
eval $SHELL_PATH/build/compiler/circle-opselector/opselector --by_id \"$NODES\" --input $TEMP_PATH/origin.circle --output $TEMP_PATH/trimmed.circle

# tflite to nnpacakge and create golden value
$SHELL_PATH/tools/nnpackage_tool/sth2nnpkgtc/tflite2nnpkgtc.sh -o $TEMP_PATH $TEMP_PATH/trimmed.tflite 

# sed 's/tflite/circle/' $TEMP_PATH/trimmed/metadata/MANIFEST > $TEMP_PATH/trimmed/metadata/MANIFESTT
# mv $TEMP_PATH/trimmed/metadata/MANIFESTT $TEMP_PATH/trimmed/metadata/MANIFEST
# 위의 두 줄을 아래 한 줄로 처리할 수 있음!
sed -i 's/tflite/circle/g' $TEMP_PATH/trimmed/metadata/MANIFEST

# 패키진 내의 trimmed.tflite를 삭제하고 trimmed.circle로 교체
# rm $TEMP_PATH/trimmed/metadata/MANIFEST $TEMP_PATH/trimmed/trimmed.tflite
rm $TEMP_PATH/trimmed/trimmed.tflite
mv $TEMP_PATH/trimmed.circle $TEMP_PATH/trimmed/

# 패키지를 현재 위치로 이동(one root 폴더)
# 이동하지 않으면 어째서인지 onert-test가 인식을 못함
mv $TEMP_PATH/trimmed $SHELL_PATH
rm -r $TEMP_PATH
# compare trimmed tflite and trimmed circle files
$SHELL_PATH/Product/x86_64-linux.release/out/test/onert-test nnpkg-test trimmed
# $SHELL_PATH/Product/x86_64-linux.debug/out/test/onert-test nnpkg-test trimmed
rm -r $SHELL_PATH/trimmed