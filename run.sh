#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <suffix>";
    exit 1;
fi

suffix=$1;
results="results_${suffix}.txt";
total=0;
count=0;

for trace in `ls *.trace`; do
    echo "running cacheSim on ${trace}...";
    ./cacheSim ${trace};
    echo "done.";
    echo "appending to results...";
    avg=`sed '4q;d' ${trace}.out`;
    total=`echo "${total}+${avg}" | bc`;
    echo "avg=${avg}";
    echo "running total=${total}";
    base=`basename ${trace} .trace`;
    echo -e "${avg}\t${base}" >> ${results};
    let count=${count}+1;
    echo "done."
done

running_avg=`echo "scale=4;${total}/${count}" | bc`;
echo -e "${running_avg}\tAverage time" >> ${results};
echo "" >> ${results};
echo "ALL DONE.";
