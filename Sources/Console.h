// ----------------------------------------------
// Polytechnique - INF584 "Image Synthesis"
//
// Base code for practical assignments.
//
// Copyright (C) 2022 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------
#pragma once

#include <string>
#include <iostream>

/// Provides a basic TTI service. Fire message on the standard output by default.
class Console {
public:

    /// Print a message on output stream.
    static void print (const std::string & message, bool prefix = true);

    /// Toggle message firing. On by default.
    static void toggleVerbose (bool verbose);

    /// Indicate whether message are effectively fired
    static bool isVerbose ();

    /// Erase all previous messages
    static void clear ();

    /// Change the output stream of the console. Reset to standard output if stream is null.
    static void setStream (std::ostream * stream);

private:
    static bool sm_verbose;
    static std::ostream * sm_output;
};

