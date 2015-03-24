#!/bin/bash
SIMCMD=./src/bittopsim
SIMTIME=86400
SIMSRVNODES=(50 100 500 1000 3000 6500 10000)
SIMCLTNODES=(0 50 100 500 1000 3000 5000 10000 25000)

mkdir -p log
for i in ${SIMSRVNODES[*]}; do
	for j in ${SIMCLTNODES[*]}; do
		echo "Running $SIMCMD for $i server nodes and $j client nodes, in $SIMTIME:"
		echo $SIMCMD $i $j $SIMTIME > log/bittopsim-$i-$j-$SIMTIME.log
		$SIMCMD $i $j $SIMTIME >> log/bittopsim-$i-$j-$SIMTIME.log
	done
done
