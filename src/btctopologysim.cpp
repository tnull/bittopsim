#include "btctopologysim.h"
#include <iostream>
#include "constants.h"
#include <boost/graph/random.hpp> // for the random graph
#include <boost/random/mersenne_twister.hpp> // for the random number generator


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
			node -> start();
		}
	}

	// generate the graph
	Graph g(allNodes.size());
	nodeVectorToGraph(allNodes, g);

	// generate random graph for comparison
	Graph randomGraph;
	boost::random::mt19937 rng;
	boost::generate_random_graph(randomGraph, num_vertices(g), num_edges(g), rng, false, false);

	// start the calculations and print the results
	calculateAndPrintData(g, randomGraph);
	
	// write the graph
	writeGraphs(g, randomGraph, graphFilePath);
}


BTCTopologySimulation::~BTCTopologySimulation()
{
	// clean up
	allNodes.clear();
}

time_t BTCTopologySimulation::getSimClock()
{
	return BTCTopologySimulation::simClock;
}

time_t BTCTopologySimulation::tickSimClock()
{
	return ++BTCTopologySimulation::simClock;
}

void BTCTopologySimulation::calculateAndPrintData(Graph& g, Graph& randomGraph)
{
	// calculate clustering coefs
	float cc =calculateClustering(g);
	float randomCC = calculateClustering(randomGraph);

	// calculate mean geodesic path
	DistanceMatrix distances = calculateDistances(g);
	DistanceMatrix randomDistances = calculateDistances(randomGraph);

	float meanGeodesic = calculateMeanGeodesic(g, distances);
	float randomMeanGeodesic = calculateMeanGeodesic(randomGraph, randomDistances);

	// calculate the diameter:
	unsigned long diameter = calculateDiameter(g, distances);
	unsigned long randomDiameter = calculateDiameter(randomGraph, randomDistances);

	// print results:
	std::cout << std::endl << std::endl;
	std::cout << "\t\tStatistics!" << std::endl;
	std::cout << "\t\t-----------" << std::endl;
	std::cout << std::setw(20) << "" << "\t | " << std::setw(10) << "Bitcoin" << " | " << std::setw(10) << "Random Graph" << std::endl;
	std::cout << std::setw(20) << "Clustering Coef" << "\t | " << std::setw(10) << cc << " | " << std::setw(10) << randomCC << std::endl;

	std::cout << std::setw(20) << "Mean Geodesic Dist" << "\t | " << std::setw(10) << meanGeodesic << " | " << std::setw(10) << randomMeanGeodesic << std::endl;

	std::cout << std::setw(20) << "Diameter" << "\t | " << std::setw(10) << diameter << " | " << std::setw(10) << randomDiameter << std::endl;
}

void BTCTopologySimulation::writeGraphs(Graph& g, Graph& randomGraph, std::string graphFilePath)
{
	std::map<std::string,std::string> graph_attr, vertex_attr, edge_attr;
	graph_attr["ratio"] = "auto";
	edge_attr["arrowsize"] = "0.3";
	edge_attr["penwidth"] = "0.3";
	vertex_attr["shape"] = "point";

	std::ofstream graphFile(graphFilePath);
	std::ofstream randomGraphFile(graphFilePath+=".random.gv");
	if(graphFile.is_open()) {
		boost::write_graphviz(graphFile, g, 
				boost::default_writer(),
				boost::default_writer(),
				boost::make_graph_attributes_writer(graph_attr, vertex_attr, edge_attr)
		);
		graphFile.close();
	}
	if(randomGraphFile.is_open()) {
		boost::write_graphviz(randomGraphFile, randomGraph, 
				boost::default_writer(),
				boost::default_writer(),
				boost::make_graph_attributes_writer(graph_attr, vertex_attr, edge_attr)
		);
		randomGraphFile.close();
	}
}

float BTCTopologySimulation::calculateClustering(Graph& g)
{
	ClusteringContainer coefs(boost::num_vertices(g));

	ClusteringMap cm(coefs, g);

	float cc = boost::all_clustering_coefficients(g, cm);
	return cc;
}

DistanceMatrix BTCTopologySimulation::calculateDistances(Graph& g)
{
	WeightMap wm(boost::get(&EdgeProperty::probability, g));
	DistanceMatrix distances(boost::num_vertices(g));
    DistanceMatrixMap dm(distances, g);
	boost::floyd_warshall_all_pairs_shortest_paths(g, dm, weight_map(wm));
	return distances;
}

float BTCTopologySimulation::calculateMeanGeodesic(Graph& g, DistanceMatrix& distances)
{
    DistanceMatrixMap dm(distances, g);
	GeodesicContainer geodesic(boost::num_vertices(g));
	GeodesicMap gm(geodesic, g);
	float mean_geodesic = boost::all_mean_geodesics(g, dm, gm);
	return mean_geodesic;
}

unsigned long BTCTopologySimulation::calculateDiameter(Graph& g, DistanceMatrix& distances)
{
	unsigned long maxDistance = 0;
	for(unsigned long i = 0; i < boost::num_vertices(g); ++i) {
		for(unsigned long j = 0; j < boost::num_vertices(g); ++j) {
			if(distances[i][j] > maxDistance) maxDistance = distances[i][j];
		}
	}
	return maxDistance;
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
