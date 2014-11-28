/**
 * @mainpage btctopologysim
 *
 * Bitcoin topology simulator
 * @author Elias Rohrer
 */

/**
 * @file btctopologysim.cpp
 *
 * @brief This is where everything starts, the application is initialzied and run.
 * 
 */

#include <iostream>
#include <vector>
#include "node.h"
#include "constants.h"

std::vector<Node*> allNodes;

/**
 * bootstrap this Node with the hardcoded dnsseeds
 */
void bootstrapNode(Node& node) 
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

/**
 * @brief AppInit is where the application starts.
 */
bool AppInit(int argc, char* argv[])
{
	// number of nodes to create
	int numberOfNodes;

	// check arguments
	switch(argc) {
		case 2: 
				numberOfNodes = std::stoi(argv[1]);
				break;
		case 1:
		default:
			std::cout << "usage: " << argv[0] << " number_of_nodes" << std::endl;
			return true;
			break;
	}
	// seed random number generator
	srand(time(nullptr));

	std::cout << "Starting btctopologysim" << std::endl;

	// To test, generate some nodes at first
	Node* n;
	for (int i = 0; i < numberOfNodes; i++) {
		try {
			n = new Node();
		} catch(std::bad_alloc& ba) {
			std::cerr << "Not enough memory: " << ba.what() << std::endl;
			return false;
		}
		std::cout << "Creating & bootstrapping Node " << n -> getID() << "." << std::endl;
		allNodes.push_back(n);
		bootstrapNode(*n);
	}

	// clean up
	for_each(allNodes.begin(), allNodes.end(), std::default_delete<Node>());
	allNodes.clear();
	return true;
}

/**
 * @brief main is just there to call AppInit.
 */
int main(int argc, char* argv[]) 
{
	if (!AppInit(argc, argv)) return -1;
	return 0;
}

