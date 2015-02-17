#DOTCMDS=(dot neato twopi circo fdp sfdp osage)
DOTCMDS=(neato twopi fdp sfdp)
NODENUM=(10 20 30 40 50 100 150 200 250 500 1000)
for i in ${NODENUM[*]}; do
	echo "Running for $i nodes:"
	mkdir -p ./graphs
	./src/btctopologysim 1 $i 86400 ./graphs/graph.gv > /dev/null # test with 50 server nodes
	#./btctopologysim  $i > /dev/null
	for j in ${DOTCMDS[*]}; do
		echo "\tRunning $j:"
		$j -Tpng -Gsize=9,15\! -Gdpi=100 ./graphs/graph.gv -o ./graphs/$i-$j.png
	done
done
