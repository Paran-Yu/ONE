# OpSelector-test

## Usage

- This test didn't consider **if** and **while** yet. 

**Single file test**

- `tflite-file-name` must be contained in [tflite receipes](https://github.com/Samsung/ONE/tree/master/res/TensorFlowLiteRecipes)
- `selected nodes` format is `"0 1 3"`

```
$ ./opselector_test.sh [tflite-file-name] [selected nodes]
# ex)
$ ./compiler/circle-opselector-test/opselector_test.sh Part_Sqrt_Rsqrt_Add_002 "0 1 2 3"
```

**Multiple file test**

- `test.lst` : Write tflite file names you want to test
- This test only covers **continuous select cases**

```Shell
python ./opselector_test.py
```

