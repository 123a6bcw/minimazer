#pragma once

//There are some commentaries in minimazer.cpp (most of them) and here
//Not much though, hope you enjoy my perfect English!

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <queue>

class Minimazer {
private:
    struct Node;

public:
    Minimazer(const char*, const char*);

    void createOutputGraph();

    void deleteUnreachableNodes(std::vector<Node*>&);
    void addDevilNode(std::vector<Node*>&);
    void buildNodeEquivalenceMatrix(std::vector<Node*>);
    void mergeEquivalentNodes(std::vector<Node*>&, bool**);

    std::vector < std::vector< std::vector<uint32_t> > > createRevGraph(std::vector<Node*>);

    std::vector<Node*> readGraph(const char*);
    void const printGraph(const char*, std::vector<Node*>);
private:
    void dfs(Node*,bool*);

    struct Node {
        Node(uint32_t, uint32_t);
        Node(uint32_t, uint32_t, bool);

        uint32_t number;
        bool isTerminal;
        std::vector <Node*> connectedNodes;
    };

    std::vector<Node*> inputGraph;
    std::vector<Node*> outputGraph;
    const char* outputFile;
    uint32_t alphabetSize;
    bool** _nodeEquivalenceMatrix;
    bool devilNodeExists = false; //I add special node to graph. Then I should carefully delete in order to not make
                                  //machine larger. But it could be deleted in deleteUnreachableNodes, so I need to handle it
};
