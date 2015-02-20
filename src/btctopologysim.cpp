#include "btctopologysim.h"
#include <iostream>
#include "constants.h"


time_t BTCTopologySimulation::simClock; /// the current time for the simulation

BTCTopologySimulation::BTCTopologySimulation(unsigned int numberOfServerNodes, unsigned int numberOfClientNodes, time_t simDuration, std::string graphFilePath)
{

	// time our sim should stop
	time_t endTime = getSimClock() + simDuration;

	// generate spawn times:
	Node::ptr n;
	time_t timeSlot;
	for (unsigned int i = 0; i < numberOfServerNodes; ++i) {
		try {
			n = std::make_shared<Node>(this);
		} catch(std::bad_alloc& ba) {
			std::cerr << "Not enough memory: " << ba.what() << std::endl;
		}
		LOG("Creating Server Node " << n->getID() << ".");
		timeSlot = (time_t) getSimClock() + rand() % simDuration;
		bootSchedule[timeSlot].push_back(n);
		allNodes.push_back(n);
	}

	for (unsigned int i = 0; i < numberOfClientNodes; ++i) {
		try {
			n = std::make_shared<Node>(this, false);
		} catch(std::bad_alloc& ba) {
			std::cerr << "Not enough memory: " << ba.what() << std::endl;
		}
		LOG("Creating Client Node " << n->getID() << ".");
		timeSlot = (time_t) getSimClock() + rand() % simDuration;
		bootSchedule[timeSlot].push_back(n);
		allNodes.push_back(n);
	}

	seed = std::make_shared<DNSSeeder>(this);
	
	// To test, generate some nodes at first
	for (; getSimClock() < endTime; tickSimClock()) {
		for (Node::ptr node : bootSchedule[getSimClock()]) {
			node -> bootstrap();
		}

		for (Node::ptr node : schedule[getSimClock()]) {
			node -> fillConnections();
		}
	}

	// generate the graph
	Graph g(allNodes.size());
	nodeVectorToGraph(allNodes, g);

	// calculate clustering coefs
	ClusteringContainer coefs(boost::num_vertices(g));
	ClusteringMap cm(coefs, g);
	float cc = boost::all_clustering_coefficients(g, cm);
	std::cout << std::endl << std::endl;
	std::cout << "\t\tStatistics!" << std::endl;
	std::cout << "\t\t-----------" << std::endl;
	std::cout << "The clustering coefficient is: " << cc << std::endl;

	// calculate mean geodesic path
	WeightMap wm(boost::get(&EdgeProperty::probability, g));
	DistanceMatrix distances(boost::num_vertices(g));
    DistanceMatrixMap dm(distances, g);

	boost::floyd_warshall_all_pairs_shortest_paths(g, dm, weight_map(wm));
	GeodesicContainer geodesic(boost::num_vertices(g));
	GeodesicMap geodesicMap(geodesic, g);
	float mean_geodesic = boost::all_mean_geodesics(g, dm, geodesicMap);
	std::cout << "The mean geodesic distance is: " << mean_geodesic << std::endl;

	// calculate the diameter:
	unsigned long max_distance = 0;
	for(unsigned long i = 0; i < boost::num_vertices(g); ++i) {
		for(unsigned long j = 0; j < boost::num_vertices(g); ++j) {
			if(distances[i][j] > max_distance) max_distance = distances[i][j];
		}
	}
	std::cout << "The graph diameter is: " << max_distance << std::endl;
	
	// write the graph
	std::map<std::string,std::string> graph_attr, vertex_attr, edge_attr;
	graph_attr["ratio"] = "auto";
	edge_attr["arrowsize"] = "0.3";
	edge_attr["penwidth"] = "0.3";
	vertex_attr["shape"] = "point";

	std::ofstream graphFile(graphFilePath);
	if(graphFile.is_open()) {
		boost::write_graphviz(graphFile, g, 
				boost::default_writer(),
				boost::default_writer(),
				boost::make_graph_attributes_writer(graph_attr, vertex_attr, edge_attr)
		);
		graphFile.close();
	}
	
}

BTCTopologySimulation::~BTCTopologySimulation()
{
	// clean up
	allNodes.clear();
}

void BTCTopologySimulation::addToSchedule(Node::ptr node, time_t timeSlot)
{
	schedule[timeSlot].push_back(node);
}

time_t BTCTopologySimulation::getSimClock()
{
	return BTCTopologySimulation::simClock;
}

time_t BTCTopologySimulation::tickSimClock()
{
	return ++BTCTopologySimulation::simClock;
}

DNSSeeder::ptr BTCTopologySimulation::getDNSSeeder() 
{
	return seed;
}

Node::vector BTCTopologySimulation::getAllNodes() 
{
	return allNodes;
}

int main(int argc, char* argv[]) 
{
	// number of server nodes to create
	int numberOfServerNodes;

	// number of client nodes to create
	int numberOfClientNodes = 0;

	// duration of the simulation
	int simDuration = 86400;

	// outpath for the graph
	std::string graphFilePath = "./graph.gv";

	// check arguments
	switch(argc) {
		case 5: 
			graphFilePath = argv[4];
		case 4: 
				simDuration = std::stoi(argv[3]);
		case 3: 
				numberOfClientNodes = std::stoi(argv[2]);
		case 2: 
				numberOfServerNodes = std::stoi(argv[1]);
				break;
		case 1:
		default:
			std::cout << "usage: " << argv[0] << " number_of_server_nodes [number_of_client_nodes] [duration_of_simulation] [graphviz graph file path]" << std::endl;
			std::cout << "the duration should be provided in seconds, default is 86400 (one day)" << std::endl;
			std::cout << "the default file path for the graph is \"./graph.gv\"" << std::endl;
			return 0;
			break;
	}
	// seed random number generator
	srand(time(NULL));
	BTCTopologySimulation sim(numberOfServerNodes, numberOfClientNodes, simDuration, graphFilePath);
	return 0;
}
