#ifndef __LINK_TYPE_MAP_H_INCLUDED__
#define __LINK_TYPE_MAP_H_INCLUDED__

#include <map>
#include <string>

#include "linkType.h"
#include "csma.h"

static std::map<std::string, linkType> linkTypeMap = {"csma", CSMA()};

#endif