// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#include "Console.h"

#include <iostream>

using namespace std;

bool Console::sm_verbose = true;

std::ostream * Console::sm_output = &std::cout;

void Console::toggleVerbose (bool verbose) {
    sm_verbose = verbose;
}

bool Console::isVerbose () {
    return sm_verbose;
}

void Console::print (const std::string & message, bool prefix) {
    if (Console::isVerbose ()) {
        if (prefix)
            *sm_output << "[MyRenderer] ";
        *sm_output << message; 
        if (prefix)
            *sm_output << std::endl;
        *sm_output << std::flush;
    }
}

void Console::clear () {
    sm_output->clear ();
}

void Console::setStream (std::ostream * stream) {
    if (stream == nullptr)
        Console::sm_output = &std::cout;
    else
        Console::sm_output = stream;
}
