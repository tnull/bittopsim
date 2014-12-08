#include "btctopologysim.h"
#include <iostream>
#include <vector>
#include "constants.h"


BTCTopologySimulation::BTCTopologySimulation(int numberOfTestNodes, time_t simDuration)
{
	// init our time
	simClock = time(NULL);

	// time our sim should stop
	time_t endTime = simClock + simDuration;

	// generate spawn times:
	spawnTimes.reserve(numberOfTestNodes);
	time_t timeSlot;
	for (int i = 0; i < numberOfTestNodes; ++i) {
		timeSlot = (time_t) simClock + rand() % simDuration;
		if(spawnTimes.find(timeSlot) != spawnTimes.end()) {
			spawnTimes[timeSlot]++;
		} else {
			spawnTimes[timeSlot] = 1;
		}
	}
	
	// To test, generate some nodes at first
	Node* n;
	for (; simClock < endTime; ++simClock) {
		// spawn Nodes
		if(spawnTimes.find(simClock) != spawnTimes.end()) {
			for(int i = 0; i < spawnTimes[simClock]; ++i) {
				try {
					n = new Node();
				} catch(std::bad_alloc& ba) {
					std::cerr << "Not enough memory: " << ba.what() << std::endl;
				}
				std::cout << simClock << ": Creating & bootstrapping Node " << n -> getID() << "." << std::endl;
				allNodes.push_back(n);
				bootstrapNode(*n);
			}
		}
	}
}

void BTCTopologySimulation::bootstrapNode(Node& node) 
{
	// if allNodes is empty, do nothing
	if(allNodes.empty()) return;

	// Choose random Nodes of allNodes
	int randomIndex;
	for(int i = 0; i < MAXSEEDPEERS && i < (int) allNodes.size(); ++i) {
		randomIndex = rand() % allNodes.size();
		Node* n = allNodes.at(randomIndex);
		node.addKnownNode(*n);
		node.connect(*n);
	}
}


BTCTopologySimulation::~BTCTopologySimulation()
{
	// clean up
	for_each(allNodes.begin(), allNodes.end(), std::default_delete<Node>());
	allNodes.clear();
}

/**
 * @brief main is just there to call AppInit.
 */
int main(int argc, char* argv[]) 
{
	// number of nodes to create
	int numberOfNodes;

	// duration of the simulation
	int simDuration = 86400;

	// check arguments
	switch(argc) {
		case 3: 
				numberOfNodes = std::stoi(argv[1]);
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
	srand(time(nullptr));
	std::cout << "Starting btctopologysim" << std::endl;
	BTCTopologySimulation sim(numberOfNodes, simDuration);
	return 0;
}

