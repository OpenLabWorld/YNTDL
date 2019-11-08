#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

#include "yntdl.h"

using namespace yntdl;

void yntdl::Node::reRefIfaces(yntdl::Node *node){
    for(auto it = node->ifaces.begin(); it != node->ifaces.end(); it++){
        it->second.node = node;
    }
}

yntdl::Node::Node(yntdl::Node& temp, std::string nodeName, std::string origName): Positionable(temp), Nameable(nodeName, origName), AdditionalTags(temp), IfaceProvider() {
    ifaces = temp.ifaces;
    reRefIfaces(this);
    requiresReRef = true;
    type = temp.type;
    nodeNum = temp.nodeNum;
    for(auto cmdPair : temp.commands){
        if(cmdPair.second){
            commands.push_back(cmdPair);
        }
        // else {
        //     std::cout << "Filtering command " + cmdPair.first + " for node " + name << std::endl;
        // }
    }
    for(Application app : temp.applications){
        if(app.inherit){
            applications.push_back(Application(&app));
        }
        // else {
            // std::cout << "Filtering out app " + app.name + " on node " + name << std::endl;
        // }
    }
}

yntdl::Node::Node(const yntdl::Node &temp): Positionable(temp), Nameable(temp), AdditionalTags(temp), IfaceProvider() {
    ifaces = temp.ifaces;
    reRefIfaces(this);
    requiresReRef = true;
    type = temp.type;
    applications = temp.applications;
    nodeNum = temp.nodeNum;
    commands = temp.commands;
}

void yntdl::Node::addCommand(std::string cmd, bool inherit){
    commands.push_back(std::pair<std::string, bool>(cmd, inherit));
}

Iface *yntdl::Node::getIface(std::string ifaceName){
    if(requiresReRef){
        reRefIfaces(this);
        requiresReRef = false;
    }
    if(ifaces.count(ifaceName) > 0){
        return &ifaces[ifaceName];
    }
    throw yntdl::YntdlException(yntdl::ErrorCode::IFACE_NOT_FOUND, ifaceName + " on Node " + name);
}

std::ostream& std::operator<<(std::ostream &out, const yntdl::Node &node){
    out << "Node " + node.name + " of type " + (node.type == "" ? "No Type" : node.type);
    return out;
}