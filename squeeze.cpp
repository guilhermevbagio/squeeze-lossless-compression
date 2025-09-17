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

std::string getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        return filename.substr(pos);
    }
    return "";
}

std::string removeExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        return filename.substr(0, pos);
    }
    return filename;
}

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

void displayByte(vector<unsigned char> byte){
    for (unsigned char b : byte) {
        displayBits(b);
    }
    std::cout << "\n";
}

void encode(vector<unsigned char>& buffer) {

    /*
    w \= NIL;
    add all possible charcodes to the dictionary
    for (every character c in the uncompressed data) do
        if ((w + c) exists in the dictionary) then
           w = w + c;
        else 
           add (w + c) to the dictionary;
           add the dictionary code for w to output;
           w = c;
        endif
    done
    add the dictionary code for w to output;
    display output;
   */


    unordered_map<vector<unsigned char>, int, VectorHash, VectorEqual> dictionary;
    vector<int> output;
    
    cout << "Encoding..." << "\n";
    cout << "Buffer size: " << buffer.size() << "\n";
    
    for (int i = 0; i < 256; i++) {
        dictionary[{static_cast<unsigned char>(i)}] = i;
    }
    
    int nextCode = 256; 
    vector<unsigned char> w = {};
    
    for (int i = 0; i < buffer.size(); i++) {
        unsigned char c = buffer[i];
        vector<unsigned char> wc = w;
        wc.push_back(c);
        
        if (dictionary.find(wc) != dictionary.end()) {
            w = wc;
        } else {
            dictionary[wc] = nextCode++;
            
            if (!w.empty()) { 
                output.push_back(dictionary[w]);
            }
            
            w = {c};
        }
    }
    
    if (!w.empty()) {
        output.push_back(dictionary[w]);
    }
    
    cout << "Compression complete. Output size: " << output.size() << " codes\n";
    
    buffer.clear();
        for (int code : output) {
        if (code < 256) {
            buffer.push_back(static_cast<unsigned char>(code));
        } else if (code < 65536) {
            buffer.push_back(0xFF);
            buffer.push_back(code & 0xFF);
            buffer.push_back((code >> 8) & 0xFF);
        } else {
            buffer.push_back(0xFE);
            buffer.push_back(code & 0xFF);
            buffer.push_back((code >> 8) & 0xFF);
            buffer.push_back((code >> 16) & 0xFF);
        }
    }
    
    cout << "New buffer size: " << buffer.size() << " bytes\n";
}

void decode(vector<unsigned char>& buffer){
    /*
    read a char k;
    output k;
    w = k;
    while (read a char k) do
        if (index k exists in dictionary) then
            entry = dictionary entry for k;
        else if (k == currSizeDict)
            entry = w + w[0];
        else
            signal invalid code;
        endif
        output entry;
        add w+entry[0] to the dictionary;
        w = entry;
    done
    */


    unordered_map<vector<unsigned char>, int, VectorHash, VectorEqual> dictionary;
    vector<int> output;
    
    cout << "Decoding..." << "\n";
    cout << "Buffer size: " << buffer.size() << "\n";
}

int main(int argc, char* argv[]) {
    cout << "SQUEEZE!" << "\n";
    
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        std::cerr << "  - For compression: input any file, output will be .sqzd\n";
        std::cerr << "  - For decompression: input .sqzd file, output will be original extension\n";
        return 1;
    }
    
    const char* inputFilename = argv[1];
    std::string inputStr(inputFilename);
    std::string inputExt = getFileExtension(inputStr);
    
    std::string outputFilename;
    bool isDecompression = (inputExt == ".sqzd");
    
    if (argc >= 3) {
        outputFilename = argv[2];
    } else {
        if (isDecompression) {
            outputFilename = removeExtension(inputStr);
        } else {
            outputFilename = inputStr + ".sqzd";
        }
    }
    
    cout << (isDecompression ? "DECOMPRESSING: " : "COMPRESSING: ") << inputFilename << "\n";
    cout << "Output: " << outputFilename << "\n";
    
    std::ifstream input(inputFilename, std::ios::binary);
    if (!input) {
        std::cerr << "Error opening input file: " << inputFilename << "\n";
        return 1;
    }
    
    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(input)),
                                       std::istreambuf_iterator<char>());
    cout << "Buffer read complete" << "\n";
    
    if (isDecompression) {
        decode(buffer);
    } else {
        encode(buffer);
    }
    
    std::ofstream output(outputFilename, std::ios::binary);
    if (!output) {
        std::cerr << "Error opening output file: " << outputFilename << "\n";
        return 1;
    }
    
    output.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    std::cout << "Operation complete: " << outputFilename << "\n";
    
    return 0;
}