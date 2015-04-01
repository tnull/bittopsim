SIMCMD=./src/bittopsim
#DOTCMDS=(dot neato twopi circo fdp sfdp osage)
DOTCMDS=(neato twopi fdp sfdp)
GRAPHFOLDER=./graphs
GRAPHFILE=$GRAPHFOLDER/graph.gv
SIMTIME=864000
SIMSRVNODES=(10 20 30 40 50 100 150 200)
SIMCLTNODES=(10 20 30 40 50 100 150 200 250 500 1000)

#cleanup before start:
rm $GRAPHFOLDER/*
for i in ${SIMSRVNODES[*]}; do
	for j in ${SIMCLTNODES[*]}; do
		echo "Running for $i server nodes and $j client nodes:"
		mkdir -p $GRAPHFOLDER
		$SIMCMD $i $j $SIMTIME 3 $GRAPHFILE > /dev/null
		for k in ${DOTCMDS[*]}; do
			echo "\tRunning $k:"
			$k -Tpng -Gsize=9,15\! -Gdpi=100 $GRAPHFILE -o $GRAPHFOLDER/$i-$j-$k.png
			$k -Tpng -Gsize=9,15\! -Gdpi=100 $GRAPHFILE.random.gv -o $GRAPHFOLDER/$i-$j-$k.random.png
		done
	done
done
