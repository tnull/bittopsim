/**
 * @brief Node represents a bitcoin peer/node
 */

#ifndef NODE_H
#define NODE_H

#include <vector>
#include <netinet/in.h>

class Node
{
public:
	/**
	 * @brief initialize the Node
	 */
	Node();
	~Node();

	/**
	 * connect this Node to another Node
	 * @param node: the Node to connect to
	 */
	void connect(Node& node);

	/**
	 * disconnect this Node from another Node
	 * @param node: the Node to disconnect from
	 */
	void disconnect(Node& node);

	/** 
	 * add a Node to our knownNodes list
	 * @param node: the Node to add
	 */
	void addKnownNode(Node& node);

	/** 
	 * remove a Node from our knownNodes list
	 * @param node: the Node to remove
	 */
	void removeKnownNode(Node& node);

	/**
	 * returns the ID of the Node, aka converts the Node IP to a String ID
	 */
	std::string getID();
private:
	// generates a random IP, also used as ID for the clients
	struct sockaddr_in generateRandomIP();

	// the IP of the Node, also used as an ID
	const struct sockaddr_in ipAddress;

	// The connected Nodes:
	std::vector<Node*> connectedNodes;

	// The known Nodes
	std::vector<Node*> knownNodes;

	// number of open connections
	int connectionsUsed;
};

#endif // NODE_H
