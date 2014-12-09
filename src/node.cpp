#include "node.h"
#include "constants.h"
#include "btctopologysim.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>

Node::Node() : ipAddress(generateRandomIP()), connectionsUsed(0)
{
}

Node::~Node()
{
}

void Node::connect(Node& node)
{
	// don't connect to self
	if (&node == this) return;

	// add to known Nodes
	addKnownNode(node);

	// if our connection count is full, do nothing further
	if(connectionsUsed >= MAXCONNECTEDPEERS) return;

	// if node is not in connected Nodes, add it
	if (std::find(connectedNodes.begin(), connectedNodes.end(), &node) == connectedNodes.end()) {
		connectedNodes.push_back(&node);
		connectionsUsed++;
		LOG("Node " << this -> getID() << " --> " << node.getID() << "[" << connectionsUsed << "/" << MAXCONNECTEDPEERS << "]");
		node.connect(*this);
	}
}

void Node::disconnect(Node& node)
{
	// if node is not in known Nodes, delete it
	removeKnownNode(node);

	// if node is not in connected Nodes, delete it
	std::vector<Node*>::iterator it = std::find(connectedNodes.begin(), connectedNodes.end(), &node);
	if ( it != connectedNodes.end()) {
		connectedNodes.erase(it);
		connectionsUsed--;
		node.disconnect(*this);
	}
}

void Node::addKnownNode(Node& node)
{
	// don't add self
	if (&node == this) return;

	// TODO: introduce maximum for known Nodes?
	
	// if node is not in known Nodes, add it
	if (std::find(knownNodes.begin(), knownNodes.end(), &node) == knownNodes.end()) {
		knownNodes.push_back(&node);
	}
}

void Node::removeKnownNode(Node& node)
{
	// if node is in known Nodes, remove it
	std::vector<Node*>::iterator it = std::find(knownNodes.begin(), knownNodes.end(), &node);
	if ( it != knownNodes.end()) {
		knownNodes.erase(it);
	}
}

void Node::sendAddrMsg(Node& receiverNode, std::vector<Node*> vAddr)
{
	receiverNode.recvAddrMsg(*this, vAddr);
}

void Node::recvAddrMsg(Node& senderNode, std::vector<Node*> vAddr)
{
	//TODO: implement and use timestamps. "lastSeen" for Nodes. We should only forward addrs younger than 10 minutes here.	
	//TODO: send to the same nodes for 24h
	
	// if we haven't got an "addr" message from this node, do something
	if(std::find(gotAddrFromNode.begin(), gotAddrFromNode.end(), &senderNode) == gotAddrFromNode.end()) {
		// send to two random connected Nodes
		for (short i = 0; i < 2; ++i) {
			unsigned int index = rand() % connectedNodes.size();
			connectedNodes.at(index) -> sendAddrMsg(*this, vAddr);
		}

		// TODO: maybe check if in our nets at some point?
		for(Node* n : vAddr) {
			addKnownNode(*n);
		}
		
		gotAddrFromNode.push_back(&senderNode);
	}
}

std::vector<Node*> Node::sendGetaddrMsg(Node& receiverNode)
{
	if(std::find(gotAddrFromNode.begin(), gotAddrFromNode.end(), &receiverNode) != gotAddrFromNode.end()) {
		gotAddrFromNode.push_back(&receiverNode);
	}
	return receiverNode.recvGetaddrMsg();
}

std::vector<Node*> Node::recvGetaddrMsg() {
	std::vector<Node*> result;
	int max = 0.23 * knownNodes.size() < 2500 ? 0.23 * knownNodes.size() : 2500; // return 2500 addresses at maximum, else 23% of knownNodes
	unsigned int randomIndex;
	for (int i = 0; i < max; ++i) {
		randomIndex = rand() % knownNodes.size();
		result.push_back(knownNodes.at(randomIndex));
	}
	return result;
}

std::string Node::getID() 
{
	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ipAddress.sin_addr), str, INET_ADDRSTRLEN);
	std::string result(str);
	return result;
}

struct sockaddr_in Node::generateRandomIP()
{
	struct sockaddr_in ip;
	unsigned char *p_ip;
	unsigned long ul_dst;

	p_ip = (unsigned char*) &ul_dst;
	for(unsigned int i=0; i < sizeof(unsigned long); i++) {
		*p_ip++ = rand()%255;
	}
	ip.sin_addr.s_addr = ul_dst;
	return ip;
}

DNSSeeder::DNSSeeder()
{
	crawlerNode = new Node();
}

std::vector<Node*> DNSSeeder::queryDNS()
{
	cacheHit();
	return nodeCache;
}

void DNSSeeder::cacheHit(bool force)
{
	int cacheSize = nodeCache.size();
	time_t now = BTCTopologySimulation::getSimClock();
	cacheHits++;
	if (force || cacheHits > (cacheSize * cacheSize) / 400 || ((cacheHits * cacheHits) > cacheSize / 20 && now - cacheTime > 5)) {
		nodeCache.clear();
		cacheHits = 0;
		cacheTime = now;
		// if we have good nodes, add 1/2 * |goodNodes| random nodes
		if (goodNodes.size() > 0) {
			unsigned int size = goodNodes.size();
			for(unsigned int i = 0; i < size / 2; i++) {
				unsigned int index = rand() % knownNodes.size();
				nodeCache.push_back(goodNodes.at(index));
			}
		}
		else {
			unsigned int index = rand() % knownNodes.size();
			nodeCache.push_back(knownNodes.at(index));
		}	
	}
}

DNSSeeder::~DNSSeeder()
{
	delete crawlerNode;
}
