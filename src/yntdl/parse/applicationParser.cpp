#include <string>
#include <iostream>
#include <memory>
#include <algorithm>

#include "yaml-cpp/yaml.h"

#include "yntdl.h"

using namespace std;

static void addAppToAllNodes(yntdl::Application *app, yntdl::Topology *top){
    for(auto subTopPtr : top->subTopologies){
        addAppToAllNodes(app, subTopPtr.get());
    }
    for(auto nodePtr : top->nodes){
        nodePtr->applications.push_back(*app);
    }
}

static void addAppToNode(yntdl::Application *application, std::string nodeName, yntdl::ParsedTopology *parsedTop){
    cout << "adding application: " + application->name + " to node " + nodeName << endl;
    for(auto cmdPair : application->commands){
        cout << "\tcmd: " + cmdPair.first << endl;
    }
    vector<string> findMe = yntdl::splitString(nodeName);
    shared_ptr<yntdl::Node> nodePtr = yntdl::findNode(findMe, &parsedTop->topology);
    bool hasApp = false;
    yntdl::Application *appPtr;
    for(size_t i = 0; i < nodePtr->applications.size(); ++i){
        yntdl::Application *app = &nodePtr->applications[i];
        if(app->name == application->name){
            hasApp = true;
            appPtr = app;
            break;
        }
    }
    if(!hasApp){
        nodePtr->applications.push_back(application);
    } else {
        // Merge pre-existing app with new one
        for(auto cmdPair : application->commands){
            appPtr->commands.push_back(cmdPair);
        }
        if(application->path != "" && application->path != appPtr->path){
            appPtr->path = application->path;
        }
    }
}

static void parseMappedApplication(YAML::Node mapNode, string appName, yntdl::ParsedTopology *parsedTop){
    vector<string> recognizedTags;
    if(mapNode[TAG_ALL]){
        recognizedTags.push_back(TAG_ALL);
        yntdl::Application app(appName);
        if(mapNode[TAG_ALL].Type() != YAML::NodeType::Null){
            //default args are present
            app.addCommand(mapNode[TAG_ALL].as<string>());
        }
        addAppToAllNodes(&app, &parsedTop->topology);
    }
    if(!mapNode[TAG_NODE] && !mapNode[yntdl::pluralize(TAG_NODE)] && !mapNode[TAG_ALL]){
        throw yntdl::YntdlException(yntdl::ErrorCode::NODE_NOT_SPECIFIED, appName);
    }
    bool inherit = true;
    if(mapNode[TAG_INHERIT]){
        recognizedTags.push_back(TAG_INHERIT);
        inherit = mapNode[TAG_INHERIT].as<bool>();
    } else if(mapNode[yntdl::pluralize(TAG_INHERIT)]){
        recognizedTags.push_back(yntdl::pluralize(TAG_INHERIT));
        inherit = mapNode[yntdl::pluralize(TAG_INHERIT)].as<bool>();
    }
    yntdl::Application app(appName, inherit);
    YAML::Node cmdTag;
    if(mapNode[TAG_COMMAND]){
        recognizedTags.push_back(TAG_COMMAND);
        cmdTag = mapNode[TAG_COMMAND];
    } else if(mapNode[yntdl::pluralize(TAG_COMMAND)]){
        recognizedTags.push_back(yntdl::pluralize(TAG_COMMAND));
        cmdTag = mapNode[yntdl::pluralize(TAG_COMMAND)];
    }
    if(cmdTag.Type() == YAML::NodeType::Scalar){
        app.addCommand(cmdTag.as<string>(), inherit);
    } else if(cmdTag.Type() == YAML::NodeType::Sequence){
        for(auto cmd : cmdTag){
            app.addCommand(cmd.as<string>(), inherit);
        }
    }

    YAML::Node nodeTag;
    if(mapNode[TAG_NODE]){
        recognizedTags.push_back(TAG_NODE);
        nodeTag = mapNode[TAG_NODE];
    } else if (mapNode[yntdl::pluralize(TAG_NODE)]) {
        recognizedTags.push_back(yntdl::pluralize(TAG_NODE));
        nodeTag = mapNode[yntdl::pluralize(TAG_NODE)];
    }
    if(nodeTag.Type() != YAML::NodeType::Null){
        for(auto node : nodeTag){
            if(node.Type() == YAML::NodeType::Scalar){
                //Just a node name
                addAppToNode(&app, node.as<string>(), parsedTop);
            } else if(node.Type() == YAML::NodeType::Map){
                //Map of node names to commands
                string nodeName = node.begin()->first.as<string>();
                string cmd = node.begin()->second.as<string>();
                yntdl::Application nodeApp(app);
                nodeApp.addCommand(cmd);
                addAppToNode(&nodeApp, nodeName, parsedTop);
            }
        }
    }
}

void yntdl::parseApplications(YAML::Node apps, yntdl::ParsedTopology *parsedTop){
    for(size_t i = 0; i < apps.size(); ++i){
        string appName = apps[i].begin()->first.as<string>();
        switch(apps[i].begin()->second.Type()){
            case YAML::NodeType::Scalar:
                if(apps[i].begin()->second.as<string>() == "all"){
                    yntdl::Application app(appName);
                    addAppToAllNodes(&app, &parsedTop->topology);
                } else {
                    yntdl::Application app(appName);
                    addAppToNode(&app, apps[i].begin()->second.as<string>(), parsedTop);
                }
                break;
            case(YAML::NodeType::Sequence):
                for(auto nodeName : apps[i].begin()->second){
                        yntdl::Application app(appName);
                    if(nodeName.Type() == YAML::NodeType::Scalar){
                        addAppToNode(&app, nodeName.as<string>(), parsedTop);
                    } else if(nodeName.Type() == YAML::NodeType::Map){
                        app.addCommand(nodeName.begin()->second.as<string>());
                        addAppToNode(&app, nodeName.begin()->first.as<string>(), parsedTop);
                    }
                }
                break;
            case(YAML::NodeType::Map):
            {
                parseMappedApplication(apps[i].begin()->second, appName, parsedTop);
                break;
            }
        }
    }
}