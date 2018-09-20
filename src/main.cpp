#include "../include/minimazer.h"
#include "../include/prettyPrinter.h"

const int numberOfTests = 11;

int main(int argc, char* argv[]) {
    if (argc == 1){
        std::cout << "error no input file" << std::endl;
        return 1;
    }

    if (strcmp(argv[1], "-f") == 0) {
        //Just print graph by given description
        prettyPrinter(argv[2], argv[3]);
    } else
    if (strcmp(argv[1], "-r") == 0) {
        //Main way to use tool. You write in console your input file, tool generates minimum machine and
        //prints images if starting and ending machine
        new prettyPrinter(argv[2], "io/input.png");
        new Minimazer(argv[2], "io/output.txt");
        new prettyPrinter("io/output.txt", "io/output.png");
    } else
    if (strcmp(argv[1], "-t") == 0) {
        //Building tests. See tests folder
        for (int i = 1; i <= numberOfTests; i++) {
            char nn1[400] = "tests/", nn2[400] = "tests/", nn3[400] = "tests/", nn4[400] = "tests/";
            char tf[400];
            tf[0] = char('0' + i / 10), tf[1] = char('0' + i % 10);
            strcat(nn1, tf);
            strcat(nn2, tf);
            strcat(nn3, tf);
            strcat(nn4, tf);
            strcat(nn1, "/input.txt");
            strcat(nn2, "/input.png");
            strcat(nn3, "/output.txt");
            strcat(nn4, "/output.png");

            new prettyPrinter(nn1, nn2);
            new Minimazer(nn1, nn3);
            new prettyPrinter(nn3, nn4);

        }
    }
    return 0;
}
