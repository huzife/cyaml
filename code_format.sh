#! /bin/bash

root_path=$(cd $(dirname $0) && pwd)
codefiles=$(find $root_path | grep -E "\.(h|cpp)$" | grep -Ev "[\^/](build|\.vscode|\.git)/")
textfiles=$(find $root_path -type f | grep -Ev "(\.png|\.gitkeep)|[\^/](build|\.vscode|\.git)/")

# clang format
for file in $codefiles; do
    clang-format -style=file -i $file
done;

# check and insert eol
for file in $textfiles; do
    if [ 0 -eq $(tail -n1 $file | wc -l) ]; then
        echo >> $file
    fi;
done;
