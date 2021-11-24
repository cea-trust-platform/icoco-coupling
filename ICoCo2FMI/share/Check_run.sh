#!/bin/bash

#DIR=$(cd $(dirname $0);pwd)
reffile=$1
shift
mkdir -p test2
cd test2
ln -sf ../*fmu .
rm -f res.csv 
fmuCheck.linux64  -o res.csv  $*
status=$?
cd -
[ $status -ne 0 ] && echo run KO && exit $status

diff test2/res.csv $reffile > diff.out
status=$?
if [ $status -ne 0 ]
then
sed "s/,/ /g;s/^\"/#\" /" test2/res.csv > test2/res.csv.txt 
sed "s/,/ /g;s/^\"/#\" /" $reffile > $reffile.txt
compare_sonde test2/res.csv.txt $reffile.txt -max_par_compo -max_delta -seuil_erreur 1e-3

status=$?
fi

echo "OK ? $status"
exit $status
