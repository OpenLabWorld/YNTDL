#ifndef __NETWORK_H_INCLUDED__
#define __NETWORK_H_INCLUDED__

// forward declared dependencies

// include dependencies
#include <string>
#include <vector>
#include <memory>
#include <cstdint>

#include "position.h"
#include "iface.h"
#include "application.h"
#include "node.h"
#include "link.h"
#include "ipaddr.h"
#include "nameable.h"

namespace ns3lxc {

// declarations
class Topology : public Positionable, public IfaceProvider, public IfaceAccepter, virtual public Nameable {
public:
	int runTime = 60;
	std::string name = "";

	IpAddr *ip;
	IpAddr *subnetMask;

	std::vector<std::shared_ptr<Topology> > subTopologies;
	std::vector<std::shared_ptr<Node> > nodes;
	std::vector<std::shared_ptr<Link> > links;
	std::vector<std::shared_ptr<Application> > applications;
	
	std::map<std::string, std::shared_ptr<Topology> > topMap;
	std::map<std::string, std::shared_ptr<Node> > nodeMap;
	std::map<std::string, std::shared_ptr<Link> > linkMap;
	
	Topology();
	Topology(std::string name): name(name) {};
	Topology(std::shared_ptr<Topology> temp, std::string newName);
	Topology(const Topology&);
};

}

#endif