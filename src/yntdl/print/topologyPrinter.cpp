#include <iostream>
#include <string>
#include "yntdl.h"

using namespace std;

void yntdl::printLink(std::ostream &out, yntdl::Link *linkPtr, int indent, bool ifaces, std::string ifaceName){
    string ind(indent, '\t');
    string ind2(indent + 1, '\t');
    if(ifaceName == ""){
        out << ind << " - " << *linkPtr << '\n';
    }
    if(ifaces && linkPtr->ifaces.size() > 0){
        if(ifaceName == ""){
            //print all ifaces
            out << ind2 << "Connected Interfaces:\n";
            for(auto ifacePtr : linkPtr->ifaces){
                out << ind2 << " - " << *ifacePtr << '\n';
            }
        } else {
            //print specific iface
            for(auto ifacePtr : linkPtr->ifaces){
                if(ifacePtr->name.find(ifaceName) != std::string::npos){
                    out << ind << " - " << *linkPtr << '\n';
                    out << ind2 << "Connected Interface:\n";
                    out << ind2 << " - " << *ifacePtr << '\n';
                }
            }
        }
    }
}

void yntdl::printTopology(std::ostream &out, yntdl::Topology *top, int indent, bool nodes, string nodeName, bool links,\
    string linkName, bool apps, string appName, bool ifaces, string ifaName, bool positions, double time, bool commands){
    string ind(indent, '\t');
    string nind = ind;

    int newIndent = indent;

    if(((linkName == "" && nodeName == "") || (nodes && links)) && appName == "" && ifaName == ""){
        newIndent = indent + 1;
        nind = string(newIndent, '\t');
        out << ind << "Topology " << top->name << '\n';
        if(top->ip != nullptr){
            out << nind << "Ip: " << *top->ip << '\n';
        }
        if(top->subnetMask != nullptr){
            out << nind << "Subnet Mask: " << *top->subnetMask << '\n';
        }
        if(positions){
            if(time < 0){
                if(top->positions.size() > 0){
                    out << nind << "Positions:\n";
                    for(auto pos : top->positions){
                        out << nind << " - " << pos << '\n';
                    }
                }
            } else {
                out << nind << top->getPosition(time) << '\n';
            }
        }
    }   
    if(links){
        if(linkName == ""){
            if(top->links.size() > 0){
                out << nind << "Links:\n";
                for(auto linkPtr : top->links){
                    yntdl::printLink(out, linkPtr.get(), newIndent, ifaces, ifaName);
                }
            }
        } else if(top->links.size() > 0){
            if(nodes && nodeName == ""){
                out << nind << "Links:\n";
            }
            for(auto linkPtr : top->links){
                if(linkPtr->name.find(linkName) != std::string::npos){
                    yntdl::printLink(out, linkPtr.get(), newIndent, ifaces, ifaName);
                }
            }
        }
    }
    if(positions || nodes || apps || (ifaces && !links)){
        if(nodeName == ""){
            if(top->nodes.size() > 0){
                if(ifaName == "" && appName == ""){
                    out << nind << "Nodes:\n";
                }
                for(auto nodePtr : top->nodes){
                    yntdl::printNode(out, nodePtr.get(), newIndent, apps, appName, ifaces, ifaName, positions, time, commands);
                }
            }
        } else if(top->nodes.size() > 0){
            if(links && linkName == ""){
                out << nind << "Nodes:\n";
            }
            for(auto nodePtr : top->nodes){
                if(nodePtr->name.find(nodeName) != std::string::npos){
                    yntdl::printNode(out, nodePtr.get(), newIndent, apps, appName, ifaces, ifaName, positions, time, commands);
                }
            }
        }
    }
    for(auto topPtr : top->subTopologies){
        yntdl::printTopology(out, topPtr.get(), indent, nodes, nodeName, links, linkName, apps, appName, ifaces, ifaName, positions, time, commands);
    }
}

//FIXME: Add Back in
/*void yntdl::printTopology(std::ostream &out, ParsedTopology *parsedTop){
    out << "Parsed Topology " << parsedtop->topology.name << '\n';
    out << '\t' << "Included Topologies:" << '\n';
    for(auto incTopPair : parsedtop->includedTopologies){
        out << "\t\t" << incTopPair.first << '\n';
    }
    out << '\t' << "Included Nodes:" << '\n';
    for(auto incNodePair : parsedtop->nodes){
        out << "\t\t" << incNodePair.first << '\n';
    }
    out << '\t' << "Included Links:" << '\n';
    for(auto incLinkPair : parsedtop->links){
        out << "\t\t" << incLinkPair.first << '\n';
    }
    printTopology(out, parsedtop->topology);
}*/