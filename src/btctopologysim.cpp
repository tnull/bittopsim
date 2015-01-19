#include "btctopologysim.h"
#include <iostream>
#include "constants.h"


time_t BTCTopologySimulation::simClock; /// the current time for the simulation

BTCTopologySimulation::BTCTopologySimulation(unsigned int numberOfTestNodes, time_t simDuration)
{

	// time our sim should stop
	time_t endTime = getSimClock() + simDuration;

	// generate spawn times:
	Node::ptr n;
	time_t timeSlot;
	for (unsigned int i = 0; i < numberOfTestNodes; ++i) {
		try {
			n = std::make_shared<Node>(this);
		} catch(std::bad_alloc& ba) {
			std::cerr << "Not enough memory: " << ba.what() << std::endl;
		}
		LOG("Creating Node " << n->getID() << ".");
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
	// number of nodes to create
	int numberOfNodes;

	// duration of the simulation
	int simDuration = 86400;

	// check arguments
	switch(argc) {
		case 3: 
				numberOfNodes = std::stol(argv[1]);
				simDuration = std::stoi(argv[2]);
				break;
		case 2: 
				numberOfNodes = std::stoi(argv[1]);
				break;
		case 1:
		default:
			std::cout << "usage: " << argv[0] << " number_of_nodes [duration_of_simulation]" << std::endl;
			std::cout << "the duration should be provided in seconds, default is 86400 (one day)" << std::endl;
			return 0;
			break;
	}
	// seed random number generator
	srand(time(NULL));
	BTCTopologySimulation sim(numberOfNodes, simDuration);
	return 0;
}
