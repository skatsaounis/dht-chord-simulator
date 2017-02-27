# insert, query experiments
for consistency in linear eventual; do
    for replica-factor in 1 3 5; do
        dsemu start
        dsemu start -n 1 -r ${replica-factor} -c ${consistency}
        for i in {2..10}; do
            dsemu start -n ${i} -r ${replica-factor} -c ${consistency}
            dsemu join -n ${i}
        done
        for file in insert.txt query.txt; do
            echo "time for consistency: ${consistency} replica-factor: ${replica-factor} Command-file: ${file}"
            time test-parser.py ${file}
        done

        dsemu terminate
    done
done

# requests experiments
for consistency in linear eventual; do
    dsemu start
    dsemu start -n 1 -r 3 -c ${consistency}
    for i in {2..10}; do
        dsemu start -n ${i} -r 3 -c ${consistency}
        dsemu join -n ${i}
    done

    echo "time for consistency: ${consistency} replica-factor: 3 Command-file: requests.txt"
    time test-parser.py requests.txt
    dsemu terminate
done
