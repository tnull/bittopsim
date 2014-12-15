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
#include <memory>
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
		void bootstrapNode(Node::ptr node);

		/**
		 * @brief return the current time of the simulation
		 * @return current time of simulation
		 */
		static time_t getSimClock();

		/**
		 * @brief increases the simulation time by one second, aka "tick"
		 * @return the increased time
		 */
		static time_t tickSimClock();

	private:
		static time_t simClock; /// the current time for the simulation
		Node::vector allNodes; /// all nodes spawned
		std::unordered_map<time_t,int> spawnTimes; /// the times at which a node should be spawned and how many nodes per timeslot should be spawned
};
#endif //BTCTOPOLOGYSIM_H
