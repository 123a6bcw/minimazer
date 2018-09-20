#include "../include/minimazer.h"

Minimazer::Node::Node(uint32_t num, uint32_t sz): number(num), isTerminal(false)
{
   connectedNodes.resize(sz);
}
Minimazer::Node::Node(uint32_t num, uint32_t sz, bool a): number(num), isTerminal(a)
{
   connectedNodes.resize(sz);
}

//Reads machine from input, construct minimum length machine and print it to output
Minimazer::Minimazer(const char* input, const char* op): outputFile(op) {
    inputGraph = readGraph(input);
    if (inputGraph.empty()) {
        return;
    }

    createOutputGraph();
    printGraph(outputFile, outputGraph);
}

/*
 Reading graph from input. Format:
 First line contains two integer n, m - number of vertices in graph and alphabet size (symbols has numbers from 0 to m-1
  vertices from 0 to n-1)
 Next line containt integer number t
 Next line contains t integers - numbers of terminal verticies
 then n blocks. Block number i starts with integer ki - number of edges from this verticies,
 in the next ki lines describing this edges - number of end verticies and symbol's number written on this edge

 First vertex is the starting vertex!
*/

std::vector<Minimazer::Node*> Minimazer::readGraph(const char* input) {
    std::vector<Node*> graph;

    std::fstream in;
    in.open(input, std::fstream::in);
    if (!in.is_open()) {
        std::cout << "Can't open this file or file is empty" << std::endl;
        return graph;
    }

    uint32_t n, m;
    in >> n >> m;
    alphabetSize = m;

    for (uint32_t i = 0; i < n; i++) {
        graph.push_back(new Node(i, alphabetSize));
    }

    uint32_t t;
    in >> t;
    for (uint32_t i = 0; i < t; i++) {
        uint32_t k;
        in >> k;
        graph[k]->isTerminal = true;
    }

    for (uint32_t i = 0; i < n; i++) {
        uint32_t k;
        in >> k;
        for (uint32_t j = 0; j < k; j++) {
            uint32_t to, w;
            in >> to >> w;
            graph[i]->connectedNodes[w] = graph[to];
        }
    }

    return graph;
}

//Prints graph in the given format to the output
void const Minimazer::printGraph(const char* output, std::vector<Node*> graph) {
    std::fstream out;
    out.open(output, std::fstream::out);

    out << graph.size() << " " << alphabetSize << "\n";

    std::vector<uint32_t> terminalVertices;
    for (auto v : graph) {
        if (v->isTerminal) {
            terminalVertices.push_back(v->number);
        }
    }
    out << terminalVertices.size() << "\n";
    for (auto v : terminalVertices) {
        out << v << " ";
    }
    out << "\n";

    for (auto v : graph) {
        uint32_t k = 0;
        //I create a specific devil vertex that should be deleted than algorithm is ednded. e->number = graph.size()
        //means it's edge to the devil vertex.
        for (auto e : v->connectedNodes) {
            if (e != NULL && e->number < graph.size()) {
                k++;
            }
        }

        out << k <<  "\n";

        for (uint32_t j = 0; j < v->connectedNodes.size(); j++) {
            if (v->connectedNodes[j] != NULL && v->connectedNodes[j]->number < graph.size()) {
                out << v->connectedNodes[j]->number << " ";
                out << j << "\n";
            }
        }
    }
}

void Minimazer::createOutputGraph() {
    outputGraph = inputGraph;

    addDevilNode(outputGraph);
    deleteUnreachableNodes(outputGraph);
    buildNodeEquivalenceMatrix(outputGraph);
    mergeEquivalentNodes(outputGraph, _nodeEquivalenceMatrix);
}

//Iterating over graph in order to find unreachable nodes from the starting one
void Minimazer::dfs(Node* v, bool* used) {
    used[v->number] = true;

    for (auto to : v->connectedNodes) {
        if (!used[to->number]) {
            dfs(to, used);
        }
    }

    return;
}

void Minimazer::deleteUnreachableNodes(std::vector<Node*>& graph) {
    bool* used = new bool[graph.size()];
    for (uint32_t i = 0; i < graph.size(); i++) {
        used[i] = false;
    }

    std::vector<Node*> newGraph;

    dfs(graph[0], used);

    //After deleting node number of vertices should match it's position in graph vector, so we need to
    //reassign numbers
    uint32_t numberOfDeletedVertices = 0;

    for (uint32_t i = 0; i < graph.size(); i++) {
        if (!used[i]) {
            numberOfDeletedVertices++;
            if (i + 1 == graph.size()) {
                devilNodeExists = false;
            }
        } else {
            graph[i]->number = i - numberOfDeletedVertices;
        }
    }

    std::vector<Node*> graphWithoudDeletedVertices;
    for (uint32_t i = 0; i < graph.size(); i++) {
        if (used[i]) {
            graphWithoudDeletedVertices.push_back(graph[i]);
        }
    }

    swap(graph, graphWithoudDeletedVertices);
    delete[] used;
}

/*
Adds a devil vertex, for each vertex that does not have edge by some symbol, adds corresponding edge to the
devil vertex. Therefore, machine becomes complete.
*/
void Minimazer::addDevilNode(std::vector<Node*>& graph) {
    Node* devilNode = new Node(graph.size(), alphabetSize);
    graph.push_back(devilNode);
    devilNodeExists = true;

    for (auto v : graph) {
        for (auto& to : v->connectedNodes) {
            if (to == NULL) {
                to = devilNode;
            }
        }
    }
}

/*
Creates Matrix: a[i, j] = 1 iff i is equivalent to j. a[i, i] is undefined and meaningless
*/

std::vector < std::vector< std::vector<uint32_t> > > Minimazer::createRevGraph(std::vector<Node*> graph) {
    std::vector < std::vector< std::vector<uint32_t> > > reversedGraph(graph.size());
    for (uint32_t i = 0; i < graph.size(); i++) {
        reversedGraph[i].resize(alphabetSize);
    }

    for (auto v : graph) {
        for (uint32_t j = 0; j < v->connectedNodes.size(); j++) {
            uint32_t to = v->connectedNodes[j]->number;
            reversedGraph[to][j].push_back(v->number);
        }
    }

    return reversedGraph;
}

void Minimazer::buildNodeEquivalenceMatrix(std::vector<Node*> graph) {
    std::vector < std::vector<  std::vector<uint32_t> > > revGraph = createRevGraph(graph);

    std::queue< std::pair<uint32_t, uint32_t> > o;

    _nodeEquivalenceMatrix = new bool* [graph.size()];
    for (uint32_t i = 0; i < graph.size(); i++) {
        _nodeEquivalenceMatrix[i] = new bool [graph.size()];
        for (uint32_t j = 0; j < graph.size(); j++) {
            _nodeEquivalenceMatrix[i][j] = 0;
        }
    }

    for (uint32_t i = 0; i < graph.size(); i++) {
        for (uint32_t j = i + 1; j < graph.size(); j++) {
            if ((graph[i]->isTerminal) ^ (graph[j]->isTerminal)) {
                o.push({i, j});
            }
        }
    }

    while (!o.empty()) {
        std::pair<uint32_t, uint32_t> top = o.front();
        uint32_t v1 = top.first;
        uint32_t v2 = top.second;
        _nodeEquivalenceMatrix[v1][v2] = true;
        _nodeEquivalenceMatrix[v2][v1] = true;

        o.pop();

        for (uint32_t c = 0; c < alphabetSize; c++) {

            for (auto to1 : revGraph[v1][c]) {
                for (auto to2 : revGraph[v2][c]) {

                    if (!_nodeEquivalenceMatrix[to1][to2]) {
                        _nodeEquivalenceMatrix[to1][to2] = true;
                        _nodeEquivalenceMatrix[to2][to1] = true;

                        o.push({to1, to2});
                    }
                }
            }
        }
    }

    /*
    For now, a[i, j] means i is not! equivalent to j. We should invert it
    */

    for (uint32_t i = 0; i < graph.size(); i++) {
        for (uint32_t j = 0; j < graph.size(); j++) {
            _nodeEquivalenceMatrix[i][j] = !_nodeEquivalenceMatrix[i][j];
        }
    }
}

//Creates graph on ñlasses of equivalence from starting one

void Minimazer::mergeEquivalentNodes(std::vector<Node*>& graph, bool** nodeEquivalenceMatrix) {
    uint32_t* equivalenceClass = new uint32_t [graph.size()];
    bool* isClassTerminal = new bool [graph.size()];
    for (uint32_t i = 0; i < graph.size(); i++) {
        equivalenceClass[i] = graph.size();
        isClassTerminal[i] = false;
    }

    uint32_t numberOfClasses = 0;

    for (uint32_t i = 0; i + devilNodeExists < graph.size(); i++) {
        if (equivalenceClass[i] == graph.size()) {
            equivalenceClass[i] = numberOfClasses;
            if (graph[i]->isTerminal) {
                isClassTerminal[equivalenceClass[i]] = true;
            }
            numberOfClasses++;

            for (uint32_t j = 0; j + devilNodeExists < graph.size(); j++) {
                if (nodeEquivalenceMatrix[i][j]) {
                    equivalenceClass[j] = equivalenceClass[i];
                }
            }
        }
    }

    std::vector<Node*> mergedGraph;
    for (uint32_t i = 0; i < numberOfClasses; i++) {
        mergedGraph.push_back(new Node(i, alphabetSize, isClassTerminal[i]));
    }

    for (uint32_t i = 0; i + devilNodeExists < graph.size(); i++) {
        Node* v = graph[i];
        for (uint32_t j = 0; j < v->connectedNodes.size(); j++) {
            Node* to = v->connectedNodes[j];
            if (!devilNodeExists || to->number + devilNodeExists != graph.size())
                mergedGraph[equivalenceClass[i]]->connectedNodes[j] = mergedGraph[equivalenceClass[to->number]];
        }
    }

    swap(mergedGraph, graph);
    delete[] equivalenceClass;
    delete[] isClassTerminal;
}


