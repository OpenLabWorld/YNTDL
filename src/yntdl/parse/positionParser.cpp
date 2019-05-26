#include <string>
#include <vector>
#include <iostream>

#include "yaml-cpp/yaml.h"

#include "yntdl.h"

using namespace std;

void parsePositions(YAML::Node posNode, std::shared_ptr<yntdl::Node> nodePtr){
    if(nodePtr->positions.size() > 0){
        nodePtr->positions.clear();
    }
    if(posNode.Type() == YAML::NodeType::Scalar){
        vector<string> xyz = splitString(posNode.as<string>());
        nodePtr->positions.push_back(yntdl::Position(0, stod(xyz[0]), stod(xyz[1]), stod(xyz[2])));
    } else if(posNode.begin()->second.Type() == YAML::NodeType::Scalar){
        for(auto iter : posNode){
            double time = iter.first.as<double>();
            vector<string> xyz = splitString(iter.second.as<string>());
            nodePtr->positions.push_back(yntdl::Position(time, stod(xyz[0]), stod(xyz[1]), stod(xyz[2])));
        }
    }
}

void parsePositions(YAML::Node posNode, yntdl::Topology *topPtr){
    if(topPtr->positions.size() > 0){
        topPtr->positions.clear();
    }
    if(posNode.Type() == YAML::NodeType::Scalar){
        vector<string> xyz = splitString(posNode.as<string>());
        topPtr->positions.push_back(yntdl::Position(0, stod(xyz[0]), stod(xyz[1]), stod(xyz[2])));
        // cout << "\tAdded " + posNode.as<string>() << endl;
    } else if(posNode.begin()->second.Type() == YAML::NodeType::Scalar){
        for(auto iter : posNode){
            double time = iter.first.as<double>();
            vector<string> xyz = splitString(iter.second.as<string>());
            topPtr->positions.push_back(yntdl::Position(time, stod(xyz[0]), stod(xyz[1]), stod(xyz[2])));
            // cout << "\tAdded " + iter.second.as<string>() << endl;
        }
    }
    computeAbsolutePositions(topPtr);
}

void computeAbsolutePositions(yntdl::Topology *top){
    for(auto topPtr : top->subTopologies){
        topPtr->centerPositionsAroundParent(top);
        computeAbsolutePositions(topPtr.get());
    }
    for(auto nodePtr : top->nodes){
        nodePtr->centerPositionsAroundParent(top);
    }
}

static void applyRotation2(int rotation,  yntdl::Topology *topPtr){
    topPtr->rotatePositions(rotation);
    for(auto nodePtr : topPtr->nodes){
        nodePtr->rotatePositions(rotation);
    }
    for(auto subTopPtr : topPtr->subTopologies){
        applyRotation2(rotation, subTopPtr.get());
    }
}

void applyRotation(int rotation, yntdl::Topology *topPtr){
    for(auto nodePtr : topPtr->nodes){
        nodePtr->rotatePositions(rotation);
    }
    for(auto subTopPtr : topPtr->subTopologies){
        applyRotation2(rotation, subTopPtr.get());
    }
}