# insert, query experiments
for consistency in linear eventual; do
    for replica_factor in 1 3 5; do
        dsemu start
        dsemu start -n 1 -r ${replica_factor} -c ${consistency}
        for i in {2..10}; do
            dsemu start -n ${i} -r ${replica_factor} -c ${consistency}
            sleep 1
            dsemu join -n ${i}
        done
        for file in insert.txt query.txt; do
            echo "c: ${consistency} r: ${replica_factor} f: ${file}" >> time.txt
            { time ./test_parser.py ${file} ; } 2>> time.txt
        done

        dsemu terminate
        echo
        sleep 5
    done
done

# requests experiments
for consistency in linear eventual; do
    dsemu start >>results.txt
    dsemu start -n 1 -r 3 -c ${consistency}
    for i in {2..10}; do
        dsemu start -n ${i} -r 3 -c ${consistency}
        sleep 1
        dsemu join -n ${i}
    done

    echo "c: ${consistency} r: 3 f: requests.txt" >> time.txt
    { time ./test_parser.py requests.txt ; } 2>> time.txt
    dsemu terminate
    echo
    sleep 5
done
