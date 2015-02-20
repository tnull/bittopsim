/*!
 * \mainpage btctopologysim
 *
 * Bitcoin topology simulator
 * \author Elias Rohrer
 */
#ifndef BTCTOPOLOGYSIM_H
#define BTCTOPOLOGYSIM_H

#include "node.h"
#include <ctime>
#include <memory>
#include <unordered_map>

/*!
 * \brief Represents a simulation of the Bitcoin network's topology.
 */
class BTCTopologySimulation
{
	public:
		/*!
		 * \brief This is where the simultion starts.
		 * \param numberOfClientNodes: number of client nodes that should be spawned.
		 * \param numberOfServerNodes: number of server nodes that should be spawned.
		 * \param endSimulationTime: the time the simulation should stop.
		 * \param graphFilePath: the file path the graphviz graph will be written to.
		 */
		BTCTopologySimulation(unsigned int numberOfServerNodes, unsigned int numberOfClientNodes, time_t endSimulationTime, std::string graphFilePath);
		~BTCTopologySimulation();

		/*!
		 * bootstrap this Node with the hardcoded dnsseeds
		 */
		void bootstrapNode(Node::ptr node);

		/*!
		 * \brief return the current time of the simulation
		 * \return current time of simulation
		 */
		static time_t getSimClock();

		/*!
		 * \brief increases the simulation time by one second, aka "tick"
		 * \return the increased time
		 */
		static time_t tickSimClock();


		/*!
		 * \brief add a node to the fillConnections-Schedule
		 * \param node to add
		 * \param timeSlot to add to
		 */
		void addToSchedule(Node::ptr node, time_t timeSlot);

		/*!
		 * \brief returns the dns seeder
		 */
		DNSSeeder::ptr getDNSSeeder();

		/*!
		 * \brief returns the list of nodes spawned
		 */
		Node::vector getAllNodes();
	private:

		/*! \brief calculate and print the data analysis */
		void calculateAndPrintData(Graph& g, Graph& randomGraph);

		/*! \brief write the graphs to a graphviz file */
		void writeGraphs(Graph& g, Graph& randomGraph, std::string graphFilePath);

		/*! \brief calculates the clustering coefficient of a graph */
		float calculateClustering(Graph& g);

		/*! \brief calculate all distances between vertices of a graph */
		DistanceMatrix calculateDistances(Graph& g);

		/*! \brief calculate the mean geodesic path of a graph */
		float calculateMeanGeodesic(Graph& g, DistanceMatrix& distances);

		/*! \brief calculate the diameter of a graph */
		unsigned long calculateDiameter(Graph& g, DistanceMatrix& distances);


		static time_t simClock; //!< the current time for the simulation
		DNSSeeder::ptr seed; //!< the DNSSeeder
		Node::vector allNodes; //!< all nodes spawned
		std::unordered_map<time_t, Node::vector> bootSchedule; //!< the times at which a node should be bootstrapped.
		std::unordered_map<time_t, Node::vector> schedule; //!< the times at which a node should be scheduled to start.
};

#endif //BTCTOPOLOGYSIM_H
