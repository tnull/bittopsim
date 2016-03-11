# bittopsim #
`bittopsim` is a simple tool to simulate the behavior of bitcoin nodes in the overlay network. It simulates the join procedure of bitcoin nodes ('servers' who allow inbound connections and 'clients' who don't) via the DNS-seeder mechanism over a certain timeframe and calculates measurements of the resulting topology (clustering coefficient, mean geodesic distance, diameter). 
An average churn-rate of leaving and joining nodes per 10 seconds of simulation time can be supplied to simulate *some* network dynamics.

`bittopsim` can also output the resulting network graph in a file in [Graphviz dot syntax](http://www.graphviz.org/), which can then be read by `dot`, `circo`, etc.

## Building ##
```
# to build the tool:
$ make

# to build the documentation
$ make doc
```

## Running ##
`bittopsim` takes the following options:
```
$ ./bittopsim
usage: ./bittopsim number_of_server_nodes [number_of_client_nodes] [duration_of_simulation] [churn rate in node change per 10 sec. (optional)] [graphviz graph file path (optional)]
the duration should be provided in 1/10 seconds, default is 864000 (one day)
```