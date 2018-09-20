#pragma once

#include <fstream>
#include <cstring> 

class prettyPrinter {
public:
    prettyPrinter(const char*, const char*);
    
    void createDotFile(const char*, const char*);
    int createImage(const char*, const char*);
    
    const char* outputFile = "bin/output.dot";
};
