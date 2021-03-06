/*!
 * \mainpage bittopsim - Bitcoin Topology Simulation
 *
 * Bitcoin topology simulator
 * \author Elias Rohrer
 */
#ifndef BITTOPSIM_H
#define BITTOPSIM_H

#include "node.h"
#include <ctime>
#include <memory>
#include <unordered_map>

/*!
 * \brief Represents a simulation of the Bitcoin network's topology.
 */
class Simulation
{
	public:
		/*!
		 * \brief This is where the simultion starts.
		 * \param numberOfClientNodes: number of client nodes that should be spawned.
		 * \param numberOfServerNodes: number of server nodes that should be spawned.
		 * \param endSimulationTime: the time the simulation should stop.
		 * \param graphFilePath: the file path the graphviz graph will be written to.
		 */
		Simulation(unsigned int numberOfServerNodes, unsigned int numberOfClientNodes, unsigned long endSimulationTime, std::string graphFilePath, int churn);
		~Simulation();

		/*!
		 * bootstrap this Node with the hardcoded dnsseeds
		 */
		void bootstrapNode(Node::ptr node);

		/*!
		 * \brief return the current time of the simulation
		 * \return current time of simulation
		 */
		static unsigned long getSimClock();

		/*!
		 * \brief increases the simulation time by one second, aka "tick"
		 * \return the increased time
		 */
		static unsigned long tickSimClock();

		/*!
		 * \brief returns the dns seeder
		 */
		DNSSeeder::ptr getDNSSeeder();

		/*!
		 * \brief returns the list of nodes spawned
		 */
		Node::vector getAllNodes();

		/*!
		 * \brief set the online status of a Node
		 * \param node is the node to be set online
		 */
		void setNodeOnline(Node::ptr node);

		/*!
		 * \brief set the offline status of a Node
		 * \param node is the node to be set offline
		 */
		void setNodeOffline(Node::ptr node);

		Node::vector getOnlineNodes();
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


		static unsigned long simClock; //!< the current time for the simulation
		DNSSeeder::ptr seed; //!< the DNSSeeder
		Node::vector allNodes; //!< all nodes spawned
		Node::vector onlineNodes; //!< all online nodes
		Node::vector offlineNodes; //!< all offline nodes
		std::unordered_map<unsigned long, Node::vector> bootSchedule; //!< the times at which a node should be bootstrapped.
};

#endif //BITTOPSIM_H
