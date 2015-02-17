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
		BTCTopologySimulation(unsigned int numberOfServerNodes, unsigned int numberOfClientNodes, time_t endSimulationTime, std::string graphFilePath);
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


		/**
		 * @brief add a node to the fillConnections-Schedule
		 * @param node to add
		 * @param timeSlot to add to
		 */
		void addToSchedule(Node::ptr node, time_t timeSlot);

		/**
		 * @brief returns the dns seeder
		 */
		DNSSeeder::ptr getDNSSeeder();

		/**
		 * @brief returns the list of nodes spawned
		 */
		Node::vector getAllNodes();
	private:
		static time_t simClock; /// the current time for the simulation
		DNSSeeder::ptr seed; /// the DNSSeeder
		Node::vector allNodes; /// all nodes spawned
		std::unordered_map<time_t, Node::vector> bootSchedule; /// the times at which a node should be bootstrapped
		std::unordered_map<time_t, Node::vector> schedule; /// the times at which a node should be scheduled
};
#endif //BTCTOPOLOGYSIM_H
