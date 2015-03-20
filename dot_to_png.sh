SIMCMD=./src/bittopsim
#DOTCMDS=(dot neato twopi circo fdp sfdp osage)
DOTCMDS=(neato twopi fdp sfdp)
GRAPHFOLDER=./graphs
GRAPHFILE=$GRAPHFOLDER/graph.gv
SIMTIME=86400
#SIMSRVNODES=50
SIMSRVNODES=(10 20 30 40 50 100 150 200 250 500 1000)
SIMCLTNODES=0

#cleanup before start:
rm $GRAPHFOLDER/*
for i in ${SIMSRVNODES[*]}; do
	echo "Running for $SIMSRVNODES server nodes and $i client nodes:"
	mkdir -p $GRAPHFOLDER
	$SIMCMD $i $SIMCLTNODES $SIMTIME $GRAPHFILE > /dev/null
	for j in ${DOTCMDS[*]}; do
		echo "\tRunning $j:"
		$j -Tpng -Gsize=9,15\! -Gdpi=100 $GRAPHFILE -o $GRAPHFOLDER/$i-$j.png
		$j -Tpng -Gsize=9,15\! -Gdpi=100 $GRAPHFILE.random.gv -o $GRAPHFOLDER/$i-$j.random.png
	done
done
