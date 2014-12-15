#include "btctopologysim.h"
#include <iostream>
#include "constants.h"


time_t BTCTopologySimulation::simClock; /// the current time for the simulation

BTCTopologySimulation::BTCTopologySimulation(unsigned int numberOfTestNodes, time_t simDuration)
{
	// time our sim should stop
	time_t endTime = getSimClock() + simDuration;

	// generate spawn times:
	spawnTimes.reserve(numberOfTestNodes);
	time_t timeSlot;
	for (unsigned int i = 0; i < numberOfTestNodes; ++i) {
		timeSlot = (time_t) getSimClock() + rand() % simDuration;
		if(spawnTimes.find(timeSlot) != spawnTimes.end()) {
			spawnTimes[timeSlot]++;
		} else {
			spawnTimes[timeSlot] = 1;
		}
	}
	
	// To test, generate some nodes at first
	Node::ptr n;
	for (; getSimClock() < endTime; tickSimClock()) {
		// spawn Nodes
		if(spawnTimes.find(getSimClock()) != spawnTimes.end()) {
			for(unsigned int i = 0; i < spawnTimes[getSimClock()]; ++i) {
				try {
					n = std::make_shared<Node>();
				} catch(std::bad_alloc& ba) {
					std::cerr << "Not enough memory: " << ba.what() << std::endl;
				}
				LOG("Creating & bootstrapping Node " << n->getID() << ".");
				allNodes.push_back(n);
				bootstrapNode(n);
			}
		}
	}
}

void BTCTopologySimulation::bootstrapNode(Node::ptr node) 
{
	// if allNodes is empty, do nothing
	if(allNodes.empty()) return;

	// get Minimum of MAXSEEDPEERS and allNodes.size() to determine to how many nodes we can connect
	int numberOfConnections = MAXSEEDPEERS < allNodes.size() ? MAXSEEDPEERS : allNodes.size();
	// Choose random Nodes of allNodes
	int randomIndex;
	for(int i = 0; i < numberOfConnections; ++i) {
		randomIndex = rand() % allNodes.size();
		Node::ptr n = allNodes.at(randomIndex);
		node->connect(n);
		node->addKnownNode(n);
	}
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
