
total_lines=0

for directory in */ .main/; do
    lines=$(cat $directory*.c $directory*.h $directory.*.c 2>/dev/null | wc -l)
    if [[ $lines -eq 0 ]]; then
        continue
    fi
    
    if [[ "$directory" = "parser/" ]]; then
        lines=$(cat $directory*.l $directory*.y | wc -l)
    fi
    
    printf "%s    \t" ${directory%/}
    
    if [[ $lines -lt 100 ]]; then
        printf "\e[90m"
    elif [[ $lines -lt 250 ]]; then
        printf "\e[37m"
    elif [[ $lines -lt 500 ]]; then
        printf "\e[39m"
    elif [[ $lines -lt 1000 ]]; then
        printf "\e[33m"
    elif [[ $lines -lt 2500 ]]; then
        printf "\e[32m"
    elif [[ $lines -lt 5000 ]]; then
        printf "\e[31m"
    elif [[ $lines -lt 10000 ]]; then
        printf "\e[34m"
    else
        printf "\e[36m"
    fi
    printf "%5d\e[39m\n" $lines
    
    (( total_lines += lines ))
done

build_lines=$(cat .count.sh Makefile 2>/dev/null | wc -l)
printf "build    \t"

if [[ $build_lines -lt 100 ]]; then
    printf "\e[90m"
elif [[ $build_lines -lt 250 ]]; then
    printf "\e[37m"
elif [[ $build_lines -lt 500 ]]; then
    printf "\e[39m"
elif [[ $build_lines -lt 1000 ]]; then
    printf "\e[33m"
elif [[ $build_lines -lt 2500 ]]; then
    printf "\e[32m"
elif [[ $build_lines -lt 5000 ]]; then
    printf "\e[31m"
elif [[ $build_lines -lt 10000 ]]; then
    printf "\e[34m"
else
    printf "\e[36m"
fi
printf "%5d\e[39m\n" $build_lines

(( total_lines += build_lines))

printf "\nTotal: \e[36m %d\e[39m\n" $total_lines
