/**
 * @mainpage btctopologysim
 *
 * Bitcoin topology simulator
 * @author Elias Rohrer
 */
#ifndef BTCTOPOLOGYSIM_H
#define BTCTOPOLOGYSIM_H

#include "node.h"
#include <ctime>
#include <unordered_map>

class BTCTopologySimulation
{
	public:

		/**
		 * @brief This is where the simultion starts.
		 * @param number of nodes that should be spawned
		 * @param the time the simulation should stop
		 */
		BTCTopologySimulation(int numberOfTestNodes, time_t endSimulationTime);
		~BTCTopologySimulation();

		/**
		 * bootstrap this Node with the hardcoded dnsseeds
		 */
		void bootstrapNode(Node& node);

	private:
		std::vector<Node*> allNodes; /// all nodes spawned
		time_t simClock; /// the current time for the simulation
		std::unordered_map<time_t,int> spawnTimes; /// the times at which a node should be spawned and how many nodes per timeslot should be spawned
};
#endif //BTCTOPOLOGYSIM_H
