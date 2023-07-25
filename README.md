# yaml_parser

## Description
scutech 实习项目：yaml 解析器<br>
该项目是一个 C++ 实现的 yaml 库，提供 yaml 读写功能，编译产物为 cyaml 动态库

## Usage
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

编译成功后可在 build/lib 目录下找到 libcyaml.so 动态库<br>
测试可执行文件位于 build/bin/test 目录
