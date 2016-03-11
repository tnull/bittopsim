# bittopsim #
`bittopsim` is a simple tool to simulate the behavior of bitcoin nodes in the overlay network. It simulates the join procedure of bitcoin nodes ('servers' who allow inbound connections and 'clients' who don't) via the DNS-seeder mechanism over a certain timeframe. It calculates measurements of the resulting topology (clustering coefficient, mean geodesic distance, diameter) and compares them to those of a random graph with an equal node count. This allows to estimate if the network has the attributes of a [Small-world network](https://en.wikipedia.org/wiki/Small-world_network).
An average churn-rate of leaving and joining nodes per 10 seconds of simulation time can be supplied to simulate *some* network dynamics.

`bittopsim` can also output the resulting topology of the simulated network and of the comparable random graph in [Graphviz dot syntax](http://www.graphviz.org/), which can then be read by `dot`, `circo`, etc. to generate visually comparable graphs.

E.g. here is a comparison of the simulated bitcoin topology and a random graph for 50 servers and 500 clients:
![Comparison of the simulated bitcoin topology and a random graph for 50 servers and 500 clients](/50-500-fdp.png)

## Building ##
```
# to build the tool:
$ make

# to build the documentation
$ make doc
```

## Usage ##
`bittopsim` takes the following options:
```
$ ./bittopsim
usage: ./bittopsim number_of_server_nodes number_of_client_nodes [duration_of_simulation] [churn rate in node change per 10 sec.] [graphviz graph file path]
the duration should be provided in simulation ticks (1/10 second), default is 864000 (one day)
```