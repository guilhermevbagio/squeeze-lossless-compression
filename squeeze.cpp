#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>
using namespace std;

#define fast_io ios::sync_with_stdio(false); cin.tie(0);

const unsigned char MARKER = 0xFF;
const int MAX_CHAIN_LENGTH = 8;

//TA LENTO Q SO A PORRA

void displayBits(unsigned char byte) {
    for (int bit = 7; bit >= 0; --bit) {
        std::cout << ((byte >> bit) & 1);
    }
    std::cout << " ";
}

void displayByte(string byte){
    for (unsigned char b : byte) {
        displayBits(b);
    }
    std::cout << "\n";
}

void encode(vector<unsigned char>& buffer) {

    cout << "Encoding..." << "\n";
   
    
}

int main(int argc, char* argv[]) {
    cout << "SQUEEZE!" << "\n";

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
        return 1;
    }

    const char* inputFilename = argv[1];
    const char* outputFilename = argv[2];

    std::ifstream input(inputFilename, std::ios::binary);
    if (!input) {
        std::cerr << "Error opening input file: " << inputFilename << "\n";
        return 1;
    }

    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(input)),
                                   std::istreambuf_iterator<char>());

    cout << "Buffer read complete" << "\n";

    encode(buffer);

    std::ofstream output(outputFilename, std::ios::binary);
    if (!output) {
        std::cerr << "Error opening output file: " << outputFilename << "\n";
        return 1;
    }

    output.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    std::cout << "Binary copy complete.\n";

    return 0;
}