#!/bin/bash

# 3 = p
# 4 = n
# 5 = i
test () { 
    for ((i = 0; i < $2; i++)) do
        while IFS=" " read p seed threads iters time
        do
            printf "%s %s %s %s %s\n" \
                $p $seed $threads $iters $time
        done < <(
                 ./saxpy.out -p $3 -n $4 -i $5 \
                 | tail -4 | head -2   \
                 | sed 's/[^0-9.]/ /g' \
                 | sed 's/ \+/ /g'     \
                 | sed 'N;s/\n/ /'     \
                ) >> $1
    done
}

main () {
    test "data.txt" 20 10000000 1 50
    test "data.txt" 20 10000000 2 50
    test "data.txt" 20 10000000 4 50
    test "data.txt" 20 10000000 8 50
    exit 0
}

main "$@"
