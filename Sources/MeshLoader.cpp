// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#include "MeshLoader.h" 

#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>
#include <ios>

#include "Console.h"

using namespace std;

void MeshLoader::loadOFF (const std::string & filename, std::shared_ptr<Mesh> meshPtr) {
	Console::print ("Start loading mesh <" + filename + ">");
    meshPtr->clear ();
	ifstream in (filename.c_str ());
    if (!in) 
        throw std::ios_base::failure ("[Mesh Loader][loadOFF] Cannot open " + filename);
	string offString;
    unsigned int sizeV, sizeT, tmp;
    in >> offString >> sizeV >> sizeT >> tmp;
    auto & P = meshPtr->vertexPositions ();
    auto & T = meshPtr->triangleIndices ();
    P.resize (sizeV);
    T.resize (sizeT);
    size_t tracker = std::max((sizeV + sizeT)/20, (unsigned)1);
    Console::print (" > [", false);
    for (unsigned int i = 0; i < sizeV; i++) {
    	if (i % tracker == 0)
    		Console::print ("-",false);
        in >> P[i][0] >> P[i][1] >> P[i][2];
    }
    int s;
    for (unsigned int i = 0; i < sizeT; i++) {
    	if ((sizeV + i) % tracker == 0)
    		Console::print ("-",false);
        in >> s;
        for (unsigned int j = 0; j < 3; j++) 
            in >> T[i][j];
    }
    Console::print ("]\n", false);
    in.close ();
    meshPtr->vertexNormals ().resize (P.size (), glm::vec3 (0.f, 0.f, 1.f));
    meshPtr->recomputePerVertexNormals ();
    Console::print ("Mesh <" + filename + "> loaded");
}


void MeshLoader::loadOBJ (const std::string & path, std::shared_ptr<Mesh> meshPtr) {
    meshPtr->clear ();
    if (path.size() < 4) {
		throw std::invalid_argument("invalid filename");
	}
	size_t length = path.size();
	std::string format = path.substr(length - 4, 4);
	if (format != ".obj") {
		throw std::invalid_argument("format not supported");
	}
	std::ifstream in(path);
	std::string line;

	int surfaces;
    std::vector<glm::vec3> vertexPositions;
    std::vector<glm::uvec3> triangleIndices;
	
	while (getline(in, line)) {
		if (line.empty()) {
			continue;
		}
		if (line[0] == '#') {
			continue;
		}
		std::istringstream iss(line);
        std::string type;
        iss >> type;
        if (type != "v" && type != "f") {
            continue;
        }
        if (type == "v") {
            double a, b, c;
            iss >> a >> b >> c;
            vertexPositions.emplace_back(glm::vec3{a, b, c});
        } else {
            std::string token;
    	 	std::vector<int> indices;
            while (iss >> token) {
                std::istringstream t1(token);
                int a;
                t1 >> a;
                indices.emplace_back(a - 1);
            }
            if (indices.size() < 3) {
                continue;
            }
            for (int j = 1; j + 1 < indices.size(); j++) {
                triangleIndices.emplace_back(glm::uvec3{indices[0], indices[j], indices[j+1]});
            }
            triangleIndices.emplace_back(glm::uvec3{indices[0], indices[1], indices[(int)indices.size()-1]});
        }
    }

	in.close();
    meshPtr->vertexPositions() = vertexPositions;
    meshPtr->triangleIndices() = triangleIndices;
    meshPtr->vertexNormals ().resize (vertexPositions.size (), glm::vec3 (0.f, 0.f, 1.f));
    meshPtr->recomputePerVertexNormals ();
}