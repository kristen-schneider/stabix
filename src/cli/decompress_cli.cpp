#include <iostream>
#include <ostream>
#include "decompress_main.h"
using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        // prevent seg faults
        cout << "1 argument required: config_path" << endl;
        return -1;
    }

    // 0. Read Config Options
    // open file, exit
    string config_file = argv[1];
    decompress_main(config_file);
}
