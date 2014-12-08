#include "node.h"
#include "constants.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>

Node::Node() : ipAddress(generateRandomIP())
{
	connectionsUsed = 0;
	getID();
}

Node::~Node()
{
}

void Node::connect(Node& node)
{
	// add to known Nodes
	addKnownNode(node);

	// if our connection count is full, do nothing further
	if(connectionsUsed >= MAXCONNECTEDPEERS) return;

	// if node is not in connected Nodes, add it
	if (std::find(connectedNodes.begin(), connectedNodes.end(), &node) == connectedNodes.end()) {
		connectedNodes.push_back(&node);
		connectionsUsed++;
		std::cout << "Node " << this -> getID() << " --> " << node.getID() << "[" << connectionsUsed << "/" << MAXCONNECTEDPEERS << "]" << std::endl;
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

DNSSeed::DNSSeed()
{
}
