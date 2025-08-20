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

struct VectorHash {
    size_t operator()(const vector<unsigned char>& v) const {
        size_t hash = 0;
        for (auto b : v) {
            hash ^= std::hash<unsigned char>()(b) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

struct VectorEqual {
    bool operator()(const vector<unsigned char>& a, const vector<unsigned char>& b) const {
        return a == b;
    }
};

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

    unordered_map<vector<unsigned char>, int, VectorHash, VectorEqual> dictionary;


    cout << "Encoding..." << "\n";
   
    vector<unsigned char> currentString = {};  

    int counter = 0;

    for(int i = 0; i < buffer.size(); i++){
        unsigned char currentChar = buffer[i];

        currentString.push_back(currentChar);

        if(dictionary.find(currentString) == dictionary.end()) {
            dictionary[currentString] = counter++;
        }
    }
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