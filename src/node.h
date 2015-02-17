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
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/clustering_coefficient.hpp> // for clustering
#include <boost/graph/exterior_property.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include <boost/graph/geodesic_distance.hpp>

class BTCTopologySimulation;
class DNSSeeder;

/** 
 * @brief represents a node of the bitcoin network
 */
class Node : public std::enable_shared_from_this<Node>
{

public:
	typedef std::shared_ptr<Node> ptr;
	typedef std::vector<std::shared_ptr<Node>> vector;
	typedef std::unordered_map<std::string, std::shared_ptr<Node>> map;

	/**
	 * @brief initialize the Node
	 */
	Node(BTCTopologySimulation* simCTX, bool acceptInboundConnections = true);
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

	/**
	 * @brief returns if the node is reachable
	 */
	bool isReachable();

	inline bool operator==(const Node& n){return (this->getID() == n.getID());}
	inline bool operator!=(const Node& n){return !(*this == n);}

	/**
	 * @brief returns connections, just for the graph
	 * @return vector of connected nodes
	 */
	Node::vector getConnections();

	/**
	 * @brief returns inbound connections, just for the graph
	 * @return vector of connected nodes
	 */
	Node::vector getInboundConnections();
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
	 * @brief sends an "addr" message to the node, with up to 1000 knownNodes
	 * @param node to send to
	 */
	void sendAddrMsg(Node::ptr receiverNode);
	
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

/****************************************************
 * Utility functions related to the node class
****************************************************/
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
 * @brief returns a random Node out of a vector
 * @param vector to look in
 * @return random node
 */
Node::ptr randomNodeOfVector(Node::vector& v);

/**
 * @brief returns a random Node out of a map
 * @param map to look in
 * @return random node
 */
Node::ptr randomNodeOfMap(Node::map& m);



typedef struct VertexProperty {
   std::string ID;
 } VertexProperty;

typedef struct EdgeProperty {
	float probability = 1.0;
 } EdgeProperty;
// TODO sollte der graph wirklich undirected sein? in einem netzwerk ist das ok?
typedef boost::labeled_graph<boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, VertexProperty, EdgeProperty>, std::string> Graph;

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
/**
 * @brief generates a boost Graph from a node vector
 * @param node vector to use
 * @return graph
 */
void nodeVectorToGraph(Node::vector& nodes, Graph& g);

#endif // NODE_H
