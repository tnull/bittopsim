/**
 * @brief Node represents a bitcoin peer/node
 */

#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <netinet/in.h>

class BTCTopologySimulation;
class DNSSeeder;

class Node : public std::enable_shared_from_this<Node>
{

public:
	typedef std::shared_ptr<Node> ptr;
	typedef std::vector<std::shared_ptr<Node>> vector;
	typedef std::unordered_map<std::string, std::shared_ptr<Node>> map;

	/**
	 * @brief initialize the Node
	 */
	Node(BTCTopologySimulation* simCTX);
	~Node();

	/** 
	 * @brief add a Node to our knownNodes list
	 * @param node: the Node to add
	 */
	void addKnownNode(Node::ptr node);

	/** 
	 * @brief add  Node::vector to our knownNodes list
	 * @param nodes: the Node::vector to add
	 */
	void addKnownNodes(Node::vector& nodes);

	/** 
	 * @brief remove a Node from our knownNodes list
	 * @param node: the Node to remove
	 */
	void removeKnownNode(Node::ptr node);


	/**
	 * @brief receives an "version" message
	 * @param node received from
	 */
	void recvVersionMsg(Node::ptr senderNode);

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
	 * @brief returns the ID of the Node, aka converts the Node IP to a String ID
	 * @return ID of Node
	 */
	std::string getID() const;

	/**
	 * @brief bootstrap this Node
	 */
	void bootstrap();

	/**
	 * @brief try to connect until we have
	 */
	void fillConnections();

	inline bool operator==(const Node& n){return (this->getID() == n.getID());}
	inline bool operator!=(const Node& n){return !(*this == n);}
protected:

	Node::map knownNodes; // The known Nodes
	BTCTopologySimulation* simCTX; // the simulation the DNSSeeder belongs to
private:
	/**
	 * @brief sends an "version" message
	 * @param node sent from
	 */
	void sendVersionMsg(Node::ptr receiverNode, bool fOneShot = false);
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

	std::string generateRandomIP(); // generates a random IP, also used as ID for the clients
	std::string identifier; // the IP of the Node, also used as an ID
	Node::vector connections; // The connected Nodes:
	Node::vector inboundConnections; // open inbound connections
	Node::vector gotAddrFromNode; // vector to save if we already got "addr" message from this node
	bool acceptInboundConnections; // does this node accept inbound connections?
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
 * @brief represents a crawler node for the DNSSeeder
 */
class CrawlerNode : public Node {

public:
	typedef std::shared_ptr<CrawlerNode> ptr;
	CrawlerNode(BTCTopologySimulation *simCTX);
	~CrawlerNode();

private:
	Node::vector goodNodes; // only the good Nodes
};

/**
 * @brief Represents an instance of the 'bitcoin-seeder'
 */
class DNSSeeder 
{
public:
	typedef std::shared_ptr<DNSSeeder> ptr;
	typedef std::vector<std::shared_ptr<DNSSeeder>> vector;

	DNSSeeder(BTCTopologySimulation* simCTX);
	~DNSSeeder();

	/**
	 * @brief represents a query to the DNSSeeder
	 * @return list of Nodes for bootstrapping
	 */
	Node::vector queryDNS();

	/**
	 * @brief Returns the crawler node
	 * @return crawler node
	 */
	CrawlerNode::ptr getCrawlerNode();
private:
	void cacheHit(bool force = false); // a Node's query came in, so hit the cache, maybe rebuild
	Node::vector nodeCache; // current cache of nodes which will be delivered
	time_t cacheTime; // last time a cache was created
	int cacheHits; // number of cache hits for this cache
	CrawlerNode::ptr crawlerNode; // the Bitcoin Node of the seeder
	BTCTopologySimulation* simCTX; // the simulation the DNSSeeder belongs to
};

#endif // NODE_H
