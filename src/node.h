/*!
 * \brief Node represents a bitcoin peer/node
 */

#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <netinet/in.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/clustering_coefficient.hpp> // for clustering
#include <boost/graph/exterior_property.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include <boost/graph/geodesic_distance.hpp>

class Simulation;
class DNSSeeder;

/*! 
 * \brief represents a node of the bitcoin network
 */
class Node : public std::enable_shared_from_this<Node>
{

public:
	typedef std::shared_ptr<Node> ptr; //!< a shared_ptr of type Node.
	typedef std::vector<std::shared_ptr<Node>> vector; //!< a vector of Nodes. 
	typedef std::deque<std::shared_ptr<Node>> deque; //!< a deque of Nodes. 
	typedef std::unordered_map<std::string, std::shared_ptr<Node>> map; //!< a map which maps strings to Nodes (meant to use the Node-IDs).

	/*!
	 * \brief initialize the Node
	 * \param simCTX: a pointer to the simulation this Node belongs to.
	 * \param acceptInboundConnections: decides if the Node will be a client or a server, defaults to server.
	 * \param online decides if the Node will be online at creation. Only viable for CrawlerNodes (so far)
	 */
	Node(Simulation* simCTX, bool acceptInboundConnections = true, bool online = false);
	~Node();

	/*! 
	 * \brief add a Node to our knownNodes list
	 * \param node: the Node to add
	 */
	void addKnownNode(Node::ptr node);

	/*! 
	 * \brief add  Node::vector to our knownNodes list
	 * \param nodes: the Node::vector to add
	 */
	void addKnownNodes(Node::vector& nodes);

	/*! 
	 * \brief remove a Node from our knownNodes list
	 * \param node: the Node to remove
	 */
	void removeKnownNode(Node::ptr node);

	/*!
	 * @brief Is called by an other node's connect function to initiate a connection to this node
	 * @param originNode is the Node the connection was made from.
	 * @return true if connection was successful, else false.
	 */
	bool inboundConnect(Node::ptr originNode);

	/*!
	 * @brief Is called by an other node's disconnect function to end a connection to this node
	 * @param originNode is the Node the connection was made from.
	 */
	void inboundDisconnect(Node::ptr originNode);

	/*!
	 * \brief receives an "version" message
	 */
	void recvVersionMsg(Node::ptr senderNode);

	/*!
	 * \brief receives an "addr" message
	 * This function receives "addr" messages from another Node. This will forward the received vAddr to _two_ connected Nodes and save them if the node doesn't have enough yet.
	 * \param originNode the Node we got the addr message from.
	 * \param vAddr is a Node::vector of addresses
	 */
	void recvAddrMsg(Node::ptr originNode, Node::vector& vAddr);


	/*!
	 * \brief receives an "getaddr" message
	 */
	void recvGetaddrMsg(Node::ptr senderNode);

	/*!
	 * \brief returns the ID of the Node, aka converts the Node IP to a String ID
	 * \return ID of Node
	 */
	std::string getID() const;

	/*!
	 * \brief bootstrap &  start this Node
	 */
	void start();

	/*!
	 * \brief stop this Node, it keeps knownNodes, but forgets all connections.
	 */
	void stop();

	/*!
	 * \brief maintenance function which gets called regularly to clean up, refill connections etc.
	 */
	void maintenance();

	/*!
	 * \brief try to connect until we have
	 */
	void fillConnections(bool fOneShot = false);

	/*!
	 * \brief returns if the node is reachable
	 */
	bool isReachable();

	inline bool operator==(const Node& n){return (this->getID() == n.getID());} //!< checks if Nodes are the same by comparing their IDs.
	inline bool operator!=(const Node& n){return !(*this == n);} //!< checks if Nodes are not the same by negating the result of the == op. 

	/*!
	 * \brief returns connections, just for the graph
	 * \return vector of connected nodes
	 */
	Node::vector getConnections();

	/*!
	 * \brief returns inbound connections, just for the graph
	 * \return vector of connected nodes
	 */
	Node::vector getInboundConnections();
protected:

	Node::map knownNodes; //!< The known Nodes of this Node
	Simulation* simCTX; //!< the simulation the DNSSeeder belongs to
	bool acceptInboundConnections; //!< does this node accept inbound connections?
	bool online; //!< is this node online?

	/*!
	 * @brief connects to an other node.
	 * @param destNode is the Node the connection will be made to. 
	 * @param fOneShot determines if we disconnect after successful retrieval of addrs.
	 * @return true if the connection could be established, else false.
	 */
	bool connect(Node::ptr destNode, bool fOneShot = false);

	void checkConnections();
	void runDisconnect();
	void trickle();
private:
	void disconnect(Node::ptr destNode);
	
	/*!
	 * \brief sends an "version" message
	 * \param receiverNode is the Node the message will be sent to
	 */
	void sendVersionMsg(Node::ptr receiverNode);

	/*! 
	 * \brief sends an "getaddr" message to the node
	 * \param receiverNode is the Node the message will be sent to
	 */
	void sendGetaddrMsg(Node::ptr receiverNode);

	/*! 
	 * \brief sends an "addr" message to the node
	 * \param receiverNode is the Node the message will be sent to.
	 * \param vAddr is the Node::vector of addresses which will be sent to receiverNode.
	 */
	void sendAddrMsg(Node::ptr receiverNode, Node::vector& vAddr);

	/*! 
	 * \brief schedules "addr" messages to a node, which will be sent at next maintenance
	 * \param receiverNode is the Node the message will be sent to.
	 * \param vAddr is the Node::vector of addresses which will be sent to receiverNode.
	 */
	void scheduleAddrMsg(Node::ptr receiverNode, Node::vector& vAddr);

	/*! 
	 * \brief schedules a disconnect next tick, this is important for fOneShot
	 * \param node is the node to disconnect from
	 */
	void scheduleDisconnect(Node::ptr node);

	/*!
	 * \brief generates a random IP, also used as ID for the clients
	 * \return std::string with the ID
	 */
	std::string generateRandomIP(); 


	std::string identifier; //!< the IP of the Node, also used as an ID
	Node::vector connections; //!< The connected (outbound) Nodes
	Node::vector inboundConnections; //!< open inbound connections
	unsigned int nOutboundConnections;
	unsigned int nInboundConnections;
	Node::vector sendAddrNodes; //!< these nodes will be used to send addrs to for 24h, then there will be new ones.
	Node::vector relayedAddrFrom; //!< saves the nodes we already relayed an addr message from
	unsigned long sendAddrNodesLastFill; //!< Last time we filled the sendAddrNodes.
	std::unordered_map<std::string, Node::vector> addrMessagesToSend; //! The vector of addr messages to send next tick.
	Node::vector disconnectSchedule; //!< Saves the node to disconnect from next tick
};

/*! 
 * \brief represents a crawler node for the DNSSeeder
 */
class CrawlerNode : public Node {

public:
	typedef std::shared_ptr<CrawlerNode> ptr; //!< a shared_ptr of type CrawlerNode.

	/*!
	 * \brief Constructor for the CrawlerNode
	 * \param simCTX: a pointer to the simulation which spawned it.
	 */
	CrawlerNode(Simulation *simCTX);
	~CrawlerNode();

	/*!
	 * \brief Accessor to the goodNodes
	 */
	Node::vector getGoodNodes();

	void maintenance();

protected:

	bool connect(Node::ptr destNode, bool fOneShot = true);
private:
	Node::vector goodNodes; //!< only the good Nodes \todo implement goodNodes!
};

/*!
 * \brief Represents an instance of the 'bitcoin-seeder'
 */
class DNSSeeder 
{
public:
	typedef std::shared_ptr<DNSSeeder> ptr; //!< A shared_ptr to the DNSSeeder
	typedef std::vector<std::shared_ptr<DNSSeeder>> vector; //!< A vector of shared_ptrs to the DNSSeeder

	/*!
	 * \brief Constructor of the DNSSeeder
	 * \param simCTX: a pointer to the simulation the seeder belongs to.
	 */
	DNSSeeder(Simulation* simCTX);
	~DNSSeeder();

	/*!
	 * \brief represents a query to the DNSSeeder
	 * \return list of Nodes for bootstrapping
	 */
	Node::vector queryDNS();

	/*!
	 * \brief Returns the crawler node
	 * \return crawler node
	 */
	CrawlerNode::ptr getCrawlerNode();
private:
	void cacheHit(bool force = false); //!< a Node's query came in, so hit the cache, maybe rebuild
	Node::vector nodeCache; //!< current cache of nodes which will be delivered
	unsigned long cacheTime; //!< last time a cache was created
	int cacheHits; //!< number of cache hits for this cache
	CrawlerNode::ptr crawlerNode; //!< the Bitcoin Node of the seeder
	Simulation* simCTX; //!< the simulation the DNSSeeder belongs to
};

/*! 
 * \brief Checks if the node is in the given vector
 * \param node to search
 * \param vector to look in
 */
bool nodeInVector(Node::ptr node, Node::vector& vector);

/*!
 * \brief Finds a node in the given vector
 * \param node to find
 * \param vector to look in
 */
Node::vector::iterator findNodeInVector(Node::ptr node, Node::vector& vector);
Node::vector::iterator findNodeInVector(std::string nodeID, Node::vector& vector);

/*!
 * \brief returns a random Node out of a vector
 * \param vector to look in
 * \return random node
 */
Node::ptr randomNodeOfVector(Node::vector& v);

/*!
 * \brief returns a random Node out of a map
 * \param map to look in
 * \return random node
 */
Node::ptr randomNodeOfMap(Node::map& m);



/*!
 * \brief Properties which represent an vertex of the graph.
 */
typedef struct VertexProperty {
 } VertexProperty;

/*!
 * \brief Properties which represent an edge of the graph.
 */
typedef struct EdgeProperty {
	float probability = 1.0; //!< propability to choose this edge (for the weight map / floyd-warshall).
 } EdgeProperty;

typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS, VertexProperty, EdgeProperty> Graph;

typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;

// for the clustering coefs
typedef boost::exterior_vertex_property<Graph, float> ClusteringProperty;
typedef ClusteringProperty::container_type ClusteringContainer;
typedef ClusteringProperty::map_type ClusteringMap;

// for geodesic mean path
typedef boost::exterior_vertex_property<Graph, float> DistanceProperty;
typedef DistanceProperty::matrix_type DistanceMatrix;
typedef DistanceProperty::matrix_map_type DistanceMatrixMap;
typedef boost::exterior_vertex_property<Graph, float> GeodesicProperty;
typedef GeodesicProperty::container_type GeodesicContainer;
typedef GeodesicProperty::map_type GeodesicMap;
typedef boost::property_map<Graph, float EdgeProperty::*>::type WeightMap;
/*!
 * \brief generates a boost Graph from a node vector
 * \param node vector to use
 * \return graph
 */
void nodeVectorToGraph(Node::vector& nodes, Graph& g);

#endif // NODE_H
