#ifndef __CONTAINER_H_INCLUDED__
#define __CONTAINER_H_INCLUDED__

// forward declared dependencies

// include dependencies
#include <string>
#include <map>
#include <vector>
#include <memory>

#include "application.h"
#include "position.h"
#include "iface.h"
#include "nameable.h"

namespace ns3lxc {

// declarations
class Node : public Positionable, public IfaceProvider, virtual public Nameable {
public:
    std::string name;

	std::map<std::string, std::shared_ptr<Iface> > ifaces;
	std::vector<Application> applications;
	
	std::weak_ptr<Iface> getIface(std::string ifaceName); // OVERRIDE IfaceProvider

    Node(): Positionable(), IfaceProvider() {};
    Node(std::string name):  name(name), Positionable(), IfaceProvider() {};
    Node(ns3lxc::Node temp, std::string nodeName);
    Node(const ns3lxc::Node &temp);
};

}
#endif