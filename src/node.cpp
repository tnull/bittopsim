#include "node.h"
#include "constants.h"
#include "btctopologysim.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>

Node::Node(BTCTopologySimulation *simCTX, bool acceptInboundConnections, bool online) : simCTX(simCTX), acceptInboundConnections(acceptInboundConnections), online(online), identifier(generateRandomIP()), sendAddrNodesLastFill(0) {}

Node::~Node() {}

bool Node::inboundConnect(Node::ptr originNode) 
{
	if(!acceptInboundConnections) return false;
	if(*originNode == *this) return false;
	if(connections.size() + inboundConnections.size() >= MAXCONNECTEDPEERS) return false;

	if(!nodeInVector(originNode, connections)) {
		if(!nodeInVector(originNode, connections) && connections.size() < MAXOUTBOUNDPEERS) {
			connections.push_back(originNode);
		} else if(!nodeInVector(originNode, inboundConnections)) {
			inboundConnections.push_back(originNode);
		} else {
			LOG("\tNode " << getID() << "is already connected to Node " << originNode->getID() << ".");
		}
	} 
	addKnownNode(originNode);
	return true;
}


void Node::inboundDisconnect(Node::ptr originNode)
{
	if(nodeInVector(originNode, connections)) {
		connections.erase(std::remove(std::begin(connections), std::end(connections), originNode));
	}

	if(nodeInVector(originNode, inboundConnections)) {
		inboundConnections.erase(std::remove(std::begin(inboundConnections), std::end(inboundConnections), originNode));
	}
}

bool Node::connect(Node::ptr destNode, bool fOneShot)
{
	// we can't reach the receiver
	if(!destNode->isReachable()) return false;

	// don't connect to self
	if (*destNode == *this) return false;

	// don't connect if we have enough peers
	if(connections.size() >= MAXOUTBOUNDPEERS || connections.size() + inboundConnections.size() >= MAXCONNECTEDPEERS) return false;

	// don't connect to already connected Node...
	if (nodeInVector(destNode, connections)) return false;
	
	// establish connection
	bool connection = destNode->inboundConnect(shared_from_this());

	if (!connection) {
		LOG("\tNode " << getID() << "could not connect to Node " << destNode->getID() << ".");
		return false;
	}
	// if we already have an inbound connection, change it to outbound...
	if(nodeInVector(destNode, inboundConnections)) {
		inboundConnections.erase(std::remove(std::begin(inboundConnections), std::end(inboundConnections), destNode));
	}

	if(!nodeInVector(destNode, connections)) {
			connections.push_back(destNode);
	}

	LOG("\tNode " << getID() << " --> " << destNode->getID() << "[" << connections.size() << "/" << MAXOUTBOUNDPEERS << "]");
	sendVersionMsg(destNode);
	
	if(fOneShot) {
		disconnect(destNode);
	}

	return connection;
}


void Node::disconnect(Node::ptr destNode)
{
	destNode->inboundDisconnect(shared_from_this());

	if(nodeInVector(destNode, connections)) {
		connections.erase(std::remove(std::begin(connections), std::end(connections), destNode));
	}

	if(nodeInVector(destNode, inboundConnections)) {
		inboundConnections.erase(std::remove(std::begin(inboundConnections), std::end(inboundConnections), destNode));
	}
}

void Node::sendVersionMsg(Node::ptr receiverNode)
{
	receiverNode -> recvVersionMsg(shared_from_this());
}

void Node::recvVersionMsg(Node::ptr senderNode)
{
	if(nodeInVector(senderNode, inboundConnections)) {
		sendVersionMsg(senderNode);
	} else {
		Node::vector addr;
		addr.push_back(shared_from_this());
		sendAddrMsg(senderNode, addr);
		Node::vector vNodes = sendGetaddrMsg(senderNode);
		addKnownNodes(vNodes);
	}
}

void Node::addKnownNode(Node::ptr node)
{
	// don't add unreachable nodes
	if (!node->isReachable()) return;
	// don't add self
	if (*node == *this) return;

	// if node is not in known Nodes, add it
	auto it = knownNodes.find(node->getID());
	if(it == knownNodes.end()) {
		knownNodes[node->getID()] = node;
	}
}

void Node::addKnownNodes(Node::vector& nodes)
{
	for (Node::ptr n : nodes) {
		addKnownNode(n);
	}
}

void Node::removeKnownNode(Node::ptr node)
{
	// if node is in known Nodes, remove it
	auto it = knownNodes.find(node->getID());
	if ( it != knownNodes.end()) {
		knownNodes.erase(it);
	}
}

void Node::sendAddrMsg(Node::ptr receiverNode)
{
	// default for sending addr messages
	unsigned int size = knownNodes.size(); 
	unsigned int maxaddrs = size <= 1000 ? size : 1000;
	Node::vector vAddr;
	unsigned int count = 1; // start at 1 because we don't want to send the receiverNode to itself
	while(count < maxaddrs) {
		// fill vector with random known nodes, which aren't ourselves and aren't already in there
		Node::ptr n = randomNodeOfMap(knownNodes);
		while(n.get() == receiverNode.get() || nodeInVector(n, vAddr)) {
			n = randomNodeOfMap(knownNodes);
		}
		vAddr.push_back(n);
		count++;
	}
	if(!vAddr.empty()) {
		sendAddrMsg(receiverNode, vAddr);
	}
}

void Node::sendAddrMsg(Node::ptr receiverNode, Node::vector& vAddr) 
{
	receiverNode->recvAddrMsg(shared_from_this(), vAddr);
}

void Node::recvAddrMsg(Node::ptr senderNode, Node::vector& vAddr)
{
	//! \todo: implement and use timestamps. "lastSeen" for Nodes. We should only forward addrs younger than 10 minutes here.	
	//! \todo: also, this right now is an infinite loop, how do we schedule the addr messages?
	
	time_t now = BTCTopologySimulation::getSimClock();

	// if we send for the first time, or we sent for 24h to the same nodes, get new random nodes.
	if(sendAddrNodes.size() == 0 || sendAddrNodesLastFill + 86400 < now) {
		sendAddrNodesLastFill = now;
		// send to two random connected Nodes
		switch(connections.size()) {
			case 0: 
				break;
			case 1:
				sendAddrNodes.push_back(connections.at(0));
				break;
			case 2:
				sendAddrNodes.push_back(connections.at(0));
				sendAddrNodes.push_back(connections.at(1));
				break;
			default:
				for (short i = 0; i < 2; ++i) {
					Node::ptr n = randomNodeOfVector(sendAddrNodes);
					while(n != nullptr && nodeInVector(n, sendAddrNodes)) {
						// avoid duplicates
						n = randomNodeOfVector(sendAddrNodes);
					}
					if(n != nullptr) {
						sendAddrNodes.push_back(n);
					}
				}
		}
	}
	for(Node::ptr n : sendAddrNodes) {
		sendAddrMsg(n, vAddr);
	}

	//! \constraint We don't check whether a node is in reachable nets, and hence are always forwarding the "addr" messages to two nodes.
	addKnownNodes(vAddr);
}

Node::vector Node::sendGetaddrMsg(Node::ptr receiverNode)
{
	return receiverNode->recvGetaddrMsg();
}

Node::vector Node::recvGetaddrMsg() {
	Node::vector result;
	int max = 0.23 * knownNodes.size() < 2500 ? 0.23 * knownNodes.size() : 2500; // return 2500 addresses at maximum, else 23% of knownNodes
	for (int i = 0; i < max; ++i) {
		Node::ptr n = randomNodeOfMap(knownNodes);
		result.push_back(n);
	}
	return result;
}

std::string Node::getID() const
{
	return identifier;
}

void Node::start()
{
	LOG("Starting Node " << getID() << ".");
	online = true;
	fillConnections();
	if(connections.size() >= 2) {
		LOG("\tEnough P2P peers available, skipping DNS seeding");
		return;
	}
	DNSSeeder::ptr seed = simCTX->getDNSSeeder();
	connect(seed->getCrawlerNode(), true);
	Node::vector nodesFromSeeds = seed->queryDNS();
	addKnownNodes(nodesFromSeeds);
	fillConnections();
}

void Node::stop() 
{
	LOG("Stopping Node " << getID() << ".");
	online = false;
	for(Node::ptr n : connections) {
		disconnect(n);
	}

	for(Node::ptr n : inboundConnections) {
		disconnect(n);
	}
	connections.clear();
	inboundConnections.clear();
}

void Node::maintenance()
{
	// check if all our connections are still online
	for (Node::ptr node : connections) {
		if(!node->isReachable()) {
			connections.erase(std::remove(std::begin(connections), std::end(connections), node), std::end(connections));
		}
	}

	fillConnections();
}

void Node::fillConnections()
{
	if(knownNodes.empty()) return;
	// get Minimum of MAXOUTBOUNDPEERS and knownNodes.size() to determine to how many nodes we can connect
	unsigned int numberOfConnections = MAXOUTBOUNDPEERS < inboundConnections.size() ? MAXOUTBOUNDPEERS : inboundConnections.size();
	
	// first fill with inbound connections
	while (connections.size() < numberOfConnections && !inboundConnections.empty()) {
		Node::ptr n = randomNodeOfVector(inboundConnections);
		while(!n->isReachable() || nodeInVector(n, connections)) {
			n = randomNodeOfVector(inboundConnections);
		}
		connect(n);
	}

	numberOfConnections = MAXOUTBOUNDPEERS < knownNodes.size() ? MAXOUTBOUNDPEERS : knownNodes.size();
	// Choose random Nodes of knownNodes
	while (connections.size() < numberOfConnections) {

		// randomly choose nodes until we have a distinct, reachable set
		Node::ptr n = randomNodeOfMap(knownNodes);
		while(!n->isReachable() || nodeInVector(n, connections)) {
			n = randomNodeOfMap(knownNodes);
		}
		connect(n);
	}
}

bool Node::isReachable()
{
	return online && acceptInboundConnections;
}


Node::vector Node::getConnections()
{
	return connections;
}

Node::vector Node::getInboundConnections()
{
	return inboundConnections;
}

std::string Node::generateRandomIP()
{
	//! \constraint The program doesn't check for IP/ID collisions, as these are 32 bit values, this shouldn't be a problem.
	struct sockaddr_in ip;
	unsigned char *p_ip;
	unsigned long ul_dst;

	p_ip = (unsigned char*) &ul_dst;
	for(unsigned int i=0; i < sizeof(unsigned long); i++) {
		*p_ip++ = rand()%255;
	}
	ip.sin_addr.s_addr = ul_dst;
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ip.sin_addr), str, INET_ADDRSTRLEN);
	std::string result(str);
	return result;
}


CrawlerNode::CrawlerNode(BTCTopologySimulation* simCTX) : Node(simCTX, true, true)
{
	// fill our goodNodes with all reachable nodes for bootstrap.
	//! \constraint We assume that bootstrapping by iterating over all nodes is ok.
	Node::vector nodes = simCTX -> getAllNodes();
	for(Node::ptr node : nodes) {
		if(node->isReachable()) goodNodes.push_back(node);
	}
}
CrawlerNode::~CrawlerNode() {}
Node::vector CrawlerNode::getGoodNodes() 
{
	return goodNodes;
}

DNSSeeder::DNSSeeder(BTCTopologySimulation* simCTX) : cacheHits(0), crawlerNode(std::make_shared<CrawlerNode>(simCTX)), simCTX(simCTX) 
{
	// force building the cache after starting
	cacheHit(true);
}

Node::vector DNSSeeder::queryDNS()
{
	cacheHit();
	return nodeCache;
}

CrawlerNode::ptr DNSSeeder::getCrawlerNode() 
{
	return crawlerNode;
}

void DNSSeeder::cacheHit(bool force)
{
	Node::vector goodNodes = crawlerNode->getGoodNodes();
	int cacheSize = nodeCache.size();
	time_t now = BTCTopologySimulation::getSimClock();
	cacheHits++;
	if (force || cacheHits > (cacheSize * cacheSize) / 400 || ((cacheHits * cacheHits) > cacheSize / 20 && now - cacheTime > 5)) {
		LOG("\tDNSSeeder is rebuilding nodeCache - force: " << std::boolalpha << force << " cacheHits: " << cacheHits << " cacheSize: " << cacheSize);
		nodeCache.clear();
		cacheHits = 0;
		cacheTime = now;
		// if we have good nodes, add 1/2 * |goodNodes| random nodes
		if (goodNodes.size() > 0) {
			unsigned int size = goodNodes.size();

			for(unsigned int i = 0; i < size / 2 && i < 1000; i++) {
				Node::ptr n = randomNodeOfVector(goodNodes);
				nodeCache.push_back(n);
			}
		}
	}
}

DNSSeeder::~DNSSeeder() {}

Node::vector::iterator findNodeInVector(Node::ptr node, Node::vector& vector) 
{
	auto it = std::find_if(vector.begin(), vector.end(), [node](Node::ptr const p) {
	    return (*p == *node);
	});
	return it;
}

bool nodeInVector(Node::ptr node, Node::vector& vector) 
{
	return findNodeInVector(node, vector) != vector.end();
}

Node::ptr randomNodeOfVector(Node::vector& v)
{
	if(v.empty()) return nullptr;
	unsigned int randomIndex = rand() % v.size();
	return v.at(randomIndex);
}

Node::ptr randomNodeOfMap(Node::map& m)
{
	if(m.empty()) return nullptr;
	unsigned int randomIndex = rand() % m.size();
	auto it = m.begin();
	std::advance(it, randomIndex);
	return it -> second;
}

void nodeVectorToGraph(Node::vector& nodes, Graph& g)
{
	for(unsigned int index = 0; index < nodes.size(); ++index) {
		//add vertex
		Vertex u = boost::vertex(index, g);

		// for outgoing connections
		for(Node::ptr to : nodes.at(index)->getConnections()) {
			// find the node in allnodes
			auto pos = findNodeInVector(to, nodes);
			unsigned int toIndex = std::distance(nodes.begin(), pos);

			// add to the graph
			Vertex v = boost::vertex(toIndex, g);

			// check if edge doesn't exist yet, then add it
			if(!boost::edge(u,v,g).second) {
				boost::add_edge(u, v, g);
			}
		}

		// for inbound connections
		for(Node::ptr from : nodes.at(index)->getInboundConnections()) {
			// find the node in allnodes
			auto pos = findNodeInVector(from, nodes);
			unsigned int fromIndex = std::distance(nodes.begin(), pos);

			// add to the graph
			Vertex v = boost::vertex(fromIndex, g);

			// check if edge doesn't exist yet, then add it
			if(!boost::edge(v, u, g).second) {
				boost::add_edge(v, u, g);
			}
		}
	}
}
