/**
 * @brief Node represents a bitcoin peer/node
 */

#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <memory>
#include <netinet/in.h>

class BTCTopologySimulation;

class Node : public std::enable_shared_from_this<Node>
{

public:
	typedef std::shared_ptr<Node> ptr;
	typedef std::vector<std::shared_ptr<Node>> vector;
	/**
	 * @brief initialize the Node
	 */
	Node();
	~Node();

	/**
	 * @brief connect this Node to another Node
	 * @param node: the Node to connect to
	 */
	void connect(Node::ptr node);

	/**
	 * @brief disconnect this Node from another Node
	 * @param node: the Node to disconnect from
	 */
	void disconnect(Node::ptr node);

	/** 
	 * @brief add a Node to our knownNodes list
	 * @param node: the Node to add
	 */
	void addKnownNode(Node::ptr node);

	/** 
	 * @brief remove a Node from our knownNodes list
	 * @param node: the Node to remove
	 */
	void removeKnownNode(Node::ptr node);

	/**
	 * @brief receives an "addr" message
	 * This function receives "addr" messages from another Node. This will forward the received vAddr to _two_ connected Nodes and save them if the node doesn't have enough yet.
	 * @param node received from
	 * @param vector of addresses
	 */
	void recvAddrMsg(Node::ptr senderNode, Node::vector& vAddr);


	/**
	 * @brief receives an "getaddr" message
	 * @param node received from
	 * @return vector of addresses
	 */
	Node::vector recvGetaddrMsg();

	/**
	 * returns the ID of the Node, aka converts the Node IP to a String ID
	 */
	std::string getID() const;

	inline bool operator==(const Node& n){return (this->getID() == n.getID());}
	inline bool operator!=(const Node& n){return !(*this == n);}
private:
	/** 
	 * @brief sends an "getaddr" message to the node
	 * @param node to send to
	 * @return vector of addresses
	 */
	Node::vector sendGetaddrMsg(Node::ptr receiverNode);

	/** 
	 * @brief sends an "addr" message to the node
	 * @param node to send to
	 * @param vector of addresses
	 */
	void sendAddrMsg(Node::ptr receiverNode, Node::vector& vAddr);

	// generates a random IP, also used as ID for the clients
	std::string generateRandomIP();

	// the IP of the Node, also used as an ID
	std::string identifier;

	// The connected Nodes:
	Node::vector connectedNodes;

	// The known Nodes
	Node::vector knownNodes;

	// number of open connections
	int connectionsUsed;

	// vector to save if we already got "addr" message from this node
	Node::vector gotAddrFromNode;
};

/*
 * Utility functions related to the node class
 */
/** 
 * @brief Checks if the node is in the given vector
 * @param node to search
 * @param vector to look in
 */
bool nodeInVector(Node::ptr node, Node::vector& vector);
/**
 * @brief Finds a node in the given vector
 * @param node to find
 * @param vector to look in
 */
Node::vector::iterator findNodeInVector(Node::ptr node, Node::vector& vector);


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
	Node::vector queryDNS();
private:
	void cacheHit(bool force = false); // a Node's query came in, so hit the cache, maybe rebuild
	Node::vector knownNodes; // all Nodes known
	Node::vector goodNodes; // only the good Nodes
	Node::vector nodeCache; // current cache of nodes which will be delivered
	time_t cacheTime; // last time a cache was created
	int cacheHits; // number of cache hits for this cache
	Node::ptr crawlerNode; // the Bitcoin Node of the seeder
};

#endif // NODE_H
