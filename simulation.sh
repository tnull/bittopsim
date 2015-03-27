#!/bin/bash
SIMCMD=./src/bittopsim
SIMTIME=864000
SIMSRVNODES=(50 100 500 1000 3000 6500 10000)
SIMCLTNODES=(0 50 100 500 1000 3000 5000 10000 25000)

rm .argfile
mkdir -p log
echo "Generating Args file"
for i in ${SIMSRVNODES[*]}; do
	for j in ${SIMCLTNODES[*]}; do
		echo "\"$i $j $SIMTIME 3 >> log/bittopsim-$i-$j-$SIMTIME.log\"" >> .argfile
	done
done

#cat argfile|xargs -I % -P4 -t sh -c "$SIMCMD %"
#rm .argfile
