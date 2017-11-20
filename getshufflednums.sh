rm -f shufflednums.txt;
for i in `seq 100`;
do echo $i;
done > shufflednums.txt;
sort -r shufflednums.txt;
./a.out < shufflednums.txt
                              
