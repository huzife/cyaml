# yaml_parser

## Description
scutech 实习项目：yaml 解析器<br>
该项目是一个 C++ 实现的 yaml 库，提供 yaml 读写功能，编译产物为 cyaml 动态库

## Build
使用一键编译、测试脚本<br>
```
./bootstrap.sh
```

手动编译 cyaml 库<br>
```
mkdir build
cd build
cmake ..
make
```

手动编译时，如果需要编译测试文件，需要在 cmake 命令中启用测试
```
cmake .. -DBUILD_TEST=ON
```

编译成功后可在 build/lib 目录下找到 libcyaml.so 动态库<br>
测试可执行文件位于 build/bin/test 目录<br>

## Install
编译完成后，在 build 目录下执行命令<br>
```
make install
```

## Usage
### Import Manually
手动导入头文件目录 include/cyaml 以及动态库 build/lib/libcyaml.so

### Use CMake
在 CMake 使用 find_package 导入 cyaml
```
find_package(cyaml)
include_directories(${CYAML_INCLUDE_DIR})
link_directories(${CYAML_LIBRARY_DIR})
```
