#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <suffix>";
    exit 1;
fi

suffix=$1;
results="results_${suffix}.txt";

for trace in `ls *.trace`; do
    echo "running cacheSim on ${trace}...";
    ./cacheSim ${trace};
    echo "done.";
    echo "appending to results...";
    avg=`sed '4q;d' ${trace}.out`;
    echo "avg=${avg}";
    base=`basename ${trace} .trace`;
    echo -e "${avg}\t${base}" >> ${results};
    echo "done."
done

echo "" >> ${results};
echo "ALL DONE.";
