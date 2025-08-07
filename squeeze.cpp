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
    std::unordered_map<string, int> freq;
    std::unordered_map<string, int> commonBytes;

    //constructs frequency map
    for (int i = 0; i < buffer.size(); i++) {
        for(int j = 1; j <= MAX_CHAIN_LENGTH; ++j) {
            if (i + j <= buffer.size()) {
                string bytes(buffer.begin() + i, buffer.begin() + i + j);

                int amt = ++freq[bytes];

                if(amt > 2 && bytes.size() >= 3){
                    commonBytes[bytes] = amt;
                } 
            }
        }
    }

    int avg, sum = 0;
    //evaluates freqs
    for (auto& pair : freq) {
        string bytes = pair.first;
        std::string bin;

        sum += pair.second;
    }
    avg = sum / freq.size();

    cout << "Average frequency: \n" << avg;
    cout << "From " << freq.size() << " total distinct bytes \n";


    cout << "Clearing key duplicates" << "\n";
    vector<string> keys;
    for(const auto& [k, _] : commonBytes)
        keys.push_back(k);

    sort(keys.begin(), keys.end(), [](const string& a, const string& b) {
         return a.length() < b.length();
    });

    unordered_map<string, int> clearFreq;

    for (int i = 0; i < keys.size(); ++i) {
        bool shouldRemove = false;

        for (int j = i + 1; j < keys.size(); ++j) {

            if(keys[i].size() >= keys[j].size()) {
                continue;
            }

            if (keys[j].find(keys[i]) != string::npos) {
                int score_i = (freq[keys[i]] - 1) * (keys[i].size() - 2);
                int score_j = (freq[keys[j]] - 1) * (keys[j].size() - 2);

                if (score_i <= score_j) {
                    shouldRemove = true;
                    break;
                }
            }
        }

        if (!shouldRemove) {
            clearFreq[keys[i]] = commonBytes[keys[i]];
        }
    }

    
    cout << "Generating aliases" << "\n";
    unordered_map<string, unsigned char> byteAliases;
    unsigned char start = 0x01;
    for(const auto& [k, _] : clearFreq) {
        byteAliases[k] = start++;
    }

    cout << byteAliases.size() << " bytes selected for substitution\n";

    for(auto& [k, _] : clearFreq)
        displayByte(k);

    int total_savings = 0;
    for (const auto& [pattern, freq] : clearFreq) {
        int saving = (freq - 1) * (pattern.size() - 2);
        if (saving > 0) {
            total_savings += saving;
        }
    }
    std::cout << "Estimated total savings: " << total_savings << " bytes\n";

    cout << "Substituting..." << "\n";
    vector<unsigned char> resultBuffer;
    
    resultBuffer.reserve(buffer.size());
    size_t last_percent = -1;
    unordered_map<unsigned char, vector<pair<string, unsigned char>>> fastAliasMap;
    unordered_map<string, int> actualSubs;
    
    for (const auto& [pattern, alias] : byteAliases) {
        fastAliasMap[(unsigned char)pattern[0]].emplace_back(pattern, alias);
    }

    for (int i = 0; i < buffer.size();) {
        bool subs = false;

        auto it = fastAliasMap.find(buffer[i]);
        if (it != fastAliasMap.end()) {
            for (const auto& [pattern, alias] : it->second) {
                if (i + pattern.size() <= buffer.size() &&
                    memcmp(&buffer[i], pattern.data(), pattern.size()) == 0) {

                    resultBuffer.push_back(MARKER);
                    resultBuffer.push_back(alias);
                    i += pattern.size(); 
                    actualSubs[pattern]++;
                    subs = true;
                    break;
                }
            }
        }

        if (!subs) {
            resultBuffer.push_back(buffer[i]);
            ++i;
        }

        size_t percent = i * 100 / buffer.size();
        if (percent > last_percent + 5) {
            std::cout << percent << "% (" << i << "/" << buffer.size() << ")\n";
            last_percent = percent;
        }
    }

    int actualSavings = 0;
    for (const auto& [pattern, count] : actualSubs) {
        if (count > 0) {
            actualSavings += count * ((int)pattern.size() - 2);
        }
    }
    std::cout << "Actual savings: " << actualSavings << " bytes\n";

    buffer.clear();
    buffer = move(resultBuffer);
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