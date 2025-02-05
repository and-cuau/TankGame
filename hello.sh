#!/bin/bash


# num_files=$(ls -1 | wc -l)

# for i in {1..5}; do
#   echo "Iteration $i"
# done

# for file in *; do
#     if [[ $file == *.exe ]]; then
#         echo "$file is an object file."
#     elif [[ $file == *.cpp ]]; then
#         echo "$file is a C++ file."
#     fi
# done

# g++ -std=c++20 -I"C:\Users\andre\CPP_TTT\boost_1_86_0" gameserver.cpp -o gameserver -L"C:\Users\andre\CPP_TTT\boost_1_86_0\stage\lib" -lstdc++ "C:\Users\andre\CPP_TTT\boost_1_86_0\stage\lib\libboost_system-vc143-mt-x64-1_86.lib" -lws2_32

g++ -std=c++2a -I"/mnt/c/Users/andre/CPP_TTT/boost_1_86_0" \
    gameserver.cpp \
    -o gameserver \
    -L"/mnt/c/Users/andre/CPP_TTT/boost_1_86_0/stage/lib" \
    -lstdc++ \
    "/mnt/c/Users/andre/CPP_TTT/boost_1_86_0/stage/lib/libboost_system-vc143-mt-x64-1_86.lib" \
    -lws2_32


# if command -v g++ &> /dev/null
# then
#     echo "g++ version: $(g++ --version)"
# else
#     echo "g++ is not installed"
# fi