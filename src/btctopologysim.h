/**
 * @mainpage btctopologysim
 *
 * Bitcoin topology simulator
 * @author Elias Rohrer
 */
#ifndef BTCTOPOLOGYSIM_H
#define BTCTOPOLOGYSIM_H

#include "node.h"

class BTCTopologySimulation
{
	public:

		/**
		 * @brief This is where the simultion starts.
		 */
		BTCTopologySimulation(int numberOfTestNodes);
		~BTCTopologySimulation();

		/**
		 * bootstrap this Node with the hardcoded dnsseeds
		 */
		void bootstrapNode(Node& node);
	private:
		std::vector<Node*> allNodes;
};
#endif //BTCTOPOLOGYSIM_H
