#include "node.h"
#include "constants.h"
#include "btctopologysim.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>

Node::Node(BTCTopologySimulation *simCTX) : simCTX(simCTX), identifier(generateRandomIP()), acceptInboundConnections(true) {}

Node::~Node() {}

void Node::sendVersionMsg(Node::ptr receiverNode, bool fOneShot)
{
	// don't connect to self
	if (*receiverNode == *this) return;

	// don't connect if we have enough peers
	if(connections.size() >= MAXOUTBOUNDPEERS || connections.size() + inboundConnections.size() >= MAXCONNECTEDPEERS) return;

	// don't connect to already connected Node...
	if (nodeInVector(receiverNode, connections)) return;

	if (!fOneShot) {

		// if we already have an inbound connection, change it to outbound...
		auto it = findNodeInVector(receiverNode, inboundConnections);
		if (it != inboundConnections.end()) {
			inboundConnections.erase(it);
		}
		connections.push_back(receiverNode);

		LOG("\tNode " << this -> getID() << " --> " << receiverNode -> getID() << "[" << connections.size() << "/" << MAXOUTBOUNDPEERS << "]");
	}

	receiverNode -> recvVersionMsg(shared_from_this());
	sendAddrMsg(receiverNode, knownNodes);
	Node::vector vNodes = sendGetaddrMsg(receiverNode);
	addKnownNodes(vNodes);
}

void Node::recvVersionMsg(Node::ptr senderNode)
{
	if(!nodeInVector(senderNode, connections)) {
		if(acceptInboundConnections && connections.size() + inboundConnections.size() < MAXCONNECTEDPEERS && !nodeInVector(senderNode, inboundConnections)) {
			inboundConnections.push_back(senderNode);
		}
	} 
	addKnownNode(senderNode);
	//TODO: schedule here?
}

void Node::addKnownNode(Node::ptr node)
{
	// don't add self
	if (*node == *this) return;

	// TODO: introduce maximum for known Nodes?
	
	// if node is not in known Nodes, add it
	if(!nodeInVector(node, knownNodes)) knownNodes.push_back(node);
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
	auto it = findNodeInVector(node, knownNodes);
	if ( it != knownNodes.end()) {
		knownNodes.erase(it);
	}
}

void Node::sendAddrMsg(Node::ptr receiverNode, Node::vector& vAddr)
{
	receiverNode->recvAddrMsg(shared_from_this(), vAddr);
}

void Node::recvAddrMsg(Node::ptr senderNode, Node::vector& vAddr)
{
	//TODO: implement and use timestamps. "lastSeen" for Nodes. We should only forward addrs younger than 10 minutes here.	
	//TODO: send to the same nodes for 24h
	
	// if we haven't got an "addr" message from this node, do something
	if(!nodeInVector(senderNode, gotAddrFromNode)) {
		gotAddrFromNode.push_back(senderNode);

		// send to two random connected Nodes
		if(connections.size() > 0) {
			//TODO do they check for duplicates?
			for (short i = 0; i < 2; ++i) {
				unsigned int index = rand() % connections.size();
				sendAddrMsg(connections.at(index), vAddr);
			}
		}

		// TODO: maybe check if in our nets at some point?
		addKnownNodes(vAddr);
	}
}

Node::vector Node::sendGetaddrMsg(Node::ptr receiverNode)
{
	Node::vector result;
	// check if we already got addr message from the receiverNode
	// TODO: check if this is correct, return empty vector if we already asked this node?
	if(!nodeInVector(receiverNode, gotAddrFromNode)) {
		gotAddrFromNode.push_back(receiverNode);
		result = receiverNode -> recvGetaddrMsg();
	}
	return result;
}

Node::vector Node::recvGetaddrMsg() {
	Node::vector result;
	int max = 0.23 * knownNodes.size() < 2500 ? 0.23 * knownNodes.size() : 2500; // return 2500 addresses at maximum, else 23% of knownNodes
	unsigned int randomIndex;
	for (int i = 0; i < max; ++i) {
		randomIndex = rand() % knownNodes.size();
		result.push_back(knownNodes.at(randomIndex));
	}
	return result;
}

std::string Node::getID() const
{
	return identifier;
}

void Node::bootstrap()
{
	LOG("Bootstrapping Node " << getID() << ".");
	DNSSeeder::ptr seed = simCTX->getDNSSeeder();
	sendVersionMsg(seed->getCrawlerNode(), true);
	//TODO: when exactly do nodes send addr/getaddr?
	Node::vector nodesFromSeeds = seed->queryDNS();
	addKnownNodes(nodesFromSeeds);
	fillConnections();
}

void Node::fillConnections()
{
	if(knownNodes.empty()) return;
	// get Minimum of MAXOUTBOUNDPEERS and knownNodes.size() to determine to how many nodes we can connect
	unsigned int numberOfConnections = MAXOUTBOUNDPEERS < knownNodes.size() ? MAXOUTBOUNDPEERS : knownNodes.size();
	
	// Choose random Nodes of knownNodes
	int randomIndex;
	while (connections.size() < numberOfConnections) {
		randomIndex = rand() % knownNodes.size();
		Node::ptr n = knownNodes.at(randomIndex);
		sendVersionMsg(n);
	}
}

std::string Node::generateRandomIP()
{
	// TODO: handle or avoid collisions of IDs/IPs
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

CrawlerNode::CrawlerNode(BTCTopologySimulation* simCTX) : Node(simCTX) {}
CrawlerNode::~CrawlerNode() {}
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
	Node::vector nodes = simCTX -> getAllNodes();
	int cacheSize = nodeCache.size();
	time_t now = BTCTopologySimulation::getSimClock();
	cacheHits++;
	if (force || cacheHits > (cacheSize * cacheSize) / 400 || ((cacheHits * cacheHits) > cacheSize / 20 && now - cacheTime > 5)) {
		LOG("\tDNSSeeder is rebuilding nodeCache - force: " << std::boolalpha << force << " cacheHits: " << cacheHits << " cacheSize: " << cacheSize);
		nodeCache.clear();
		cacheHits = 0;
		cacheTime = now;
		// if we have good nodes, add 1/2 * |goodNodes| random nodes
		if (nodes.size() > 0) {
			unsigned int size = nodes.size();
			for(unsigned int i = 0; i < size / 2; i++) {
				unsigned int index = rand() % size;
				nodeCache.push_back(nodes.at(index));
			}
		}
	}
}

DNSSeeder::~DNSSeeder() {}
