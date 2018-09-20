#include "../include/prettyPrinter.h"

prettyPrinter::prettyPrinter(const char* input, const char* output) {
    createDotFile(input, outputFile);
    createImage(outputFile, output);
}

//From file in my own syntax creates file in DOT syntax in order to print it with special tool

void prettyPrinter::createDotFile(const char* input, const char* output) {
    std::fstream in, out;
    in.open(input, std::fstream::in);
    out.open(output, std::fstream::out);

    out << "digraph machine {" << "\n";

    int n;
    in >> n;

    bool* isTerminal = new bool [n];
    for (int i = 0; i < n; i++) {
        isTerminal[i] = false;
    }

    int m;
    in >> m;
    in >> m;
    for (int i = 0; i < m; i++) {
        int k;
        in >> k;
        isTerminal[k] = true;
    }

    for (int i = 0; i < n; i++) {
        out << i;
        if (isTerminal[i]) {
           out << " [shape=doublecircle]";
        }
        out << ";\n";
    }

    for (int i = 0; i < n; i++) {
        int k;
        in >> k;
        for (int j = 0; j < k; j++) {
            int to, w;
            in >> to >> w;

            out << i << " -> " << to << " [label=" << w << "];\n";
        }
    }

    out << "}";
}

int prettyPrinter::createImage(const char* input, const char* output) {
    int v;
    char nn[100] = "dot -Tpng bin/output.dot -o";
    v = system(strcat(nn, output));
    return v;
}
