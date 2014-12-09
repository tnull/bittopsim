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
	 * @brief connect this Node to another Node
	 * @param node: the Node to connect to
	 */
	void connect(Node& node);

	/**
	 * @brief disconnect this Node from another Node
	 * @param node: the Node to disconnect from
	 */
	void disconnect(Node& node);

	/** 
	 * @brief add a Node to our knownNodes list
	 * @param node: the Node to add
	 */
	void addKnownNode(Node& node);

	/** 
	 * @brief remove a Node from our knownNodes list
	 * @param node: the Node to remove
	 */
	void removeKnownNode(Node& node);

	/**
	 * @brief receives an "addr" message
	 * This function receives "addr" messages from another Node. This will forward the received vAddr to _two_ connected Nodes and save them if the node doesn't have enough yet.
	 * @param node received from
	 * @param vector of addresses
	 */
	void recvAddrMsg(Node& senderNode, std::vector<Node*> vAddr);


	/**
	 * @brief receives an "getaddr" message
	 * @param node received from
	 * @return vector of addresses
	 */
	std::vector<Node*> recvGetaddrMsg();

	/**
	 * returns the ID of the Node, aka converts the Node IP to a String ID
	 */
	std::string getID();
private:
	/** 
	 * @brief sends an "getaddr" message to the node
	 * @param node to send to
	 * @return vector of addresses
	 */
	std::vector<Node*> sendGetaddrMsg(Node& receiverNode);

	/** 
	 * @brief sends an "addr" message to the node
	 * @param node to send to
	 * @param vector of addresses
	 */
	void sendAddrMsg(Node& receiverNode, std::vector<Node*> vAddr);

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

	// vector to save if we already got "addr" message from this node
	std::vector<Node *> gotAddrFromNode;
};

/**
 * @brief Represents an instance of the 'bitcoin-seeder'
 */
class DNSSeeder 
{
public:
	DNSSeeder();
	~DNSSeeder();

	/**
	 * @brief represents a query to the DNSSeeder
	 * @return list of Nodes for bootstrapping
	 */
	std::vector<Node*> queryDNS();
private:
	void cacheHit(bool force = false); // a Node's query came in, so hit the cache, maybe rebuild
	std::vector<Node*> knownNodes; // all Nodes known
	std::vector<Node*> goodNodes; // only the good Nodes
	std::vector<Node*> nodeCache; // current cache of nodes which will be delivered
	time_t cacheTime; // last time a cache was created
	int cacheHits; // number of cache hits for this cache
	Node* crawlerNode; // the Bitcoin Node of the seeder
};

#endif // NODE_H
