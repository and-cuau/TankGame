#!/bin/bash



echo "Enter your name:"
read user_name

echo "Hello, $user_name"

current_date=$(date)
echo "Today's date is: $current_date"

# num=10
# if [ $num -gt 5 ]; then
#     echo "Greater than 5"
# elif [ $num -eq 5 ]; then
#     echo "Equal to 5"
# else
#     echo "Less than 5"
# fi

# for i in {1..5}; do
#     echo "Number: $i"
# done

# count=1
# while [ $count -le 5 ]; do
#     echo "Count: $count"
#     ((count++))
# done

# function greet() {
#     echo "Hello, $1!"
# }
# greet "Lydia"

# echo "First argument: $1"
# echo "Second argument: $2"