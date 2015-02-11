#include "btctopologysim.h"
#include <iostream>
#include "constants.h"


time_t BTCTopologySimulation::simClock; /// the current time for the simulation

BTCTopologySimulation::BTCTopologySimulation(unsigned int numberOfServerNodes, unsigned int numberOfClientNodes, time_t simDuration)
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

	Graph g;
	nodeVectorToGraph(allNodes, g);
	std::map<std::string,std::string> graph_attr, vertex_attr, edge_attr;
	graph_attr["size"] = "3,3";
	graph_attr["rankdir"] = "LR";
	graph_attr["ratio"] = "fill";
	vertex_attr["shape"] = "circle";

	std::ofstream dot("graph.dot");
	if(dot.is_open()) {
		boost::write_graphviz(dot, g, boost::make_label_writer(boost::get(&VertexProperty::ID, g)));
		dot.close();
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

	// check arguments
	switch(argc) {
		case 4: 
				numberOfServerNodes = std::stol(argv[1]);
				numberOfClientNodes = std::stoi(argv[2]);
				simDuration = std::stoi(argv[3]);
				break;
		case 3: 
				numberOfServerNodes = std::stoi(argv[1]);
				numberOfClientNodes = std::stoi(argv[2]);
				break;
		case 2: 
				numberOfServerNodes = std::stoi(argv[1]);
				break;
		case 1:
		default:
			std::cout << "usage: " << argv[0] << " number_of_server_nodes [number_of_client_nodes] [duration_of_simulation]" << std::endl;
			std::cout << "the duration should be provided in seconds, default is 86400 (one day)" << std::endl;
			return 0;
			break;
	}
	// seed random number generator
	srand(time(NULL));
	BTCTopologySimulation sim(numberOfServerNodes, numberOfClientNodes, simDuration);
	return 0;
}
