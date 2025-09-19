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

#define fast_io                  \
    ios::sync_with_stdio(false); \
    cin.tie(0);

const unsigned char MARKER = 0xFF;
const int MAX_CHAIN_LENGTH = 8;
const string HEADER = "O=======================|[ SQUEEZE! ]|=======================O";

struct VectorHash
{
    size_t operator()(const vector<unsigned char> &v) const
    {
        size_t hash = 0;
        for (auto b : v)
        {
            hash ^= std::hash<unsigned char>()(b) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

struct VectorEqual
{
    bool operator()(const vector<unsigned char> &a, const vector<unsigned char> &b) const
    {
        return a == b;
    }
};

void printLinePair(string key, string value)
{
    cout << "| " << key;

    int padSize = 4;
    int lnLength = HEADER.size() - key.size() - value.size() - padSize;

    for (int i = 0; i < lnLength; i++)
    {
        cout << " ";
    }

    cout << value << " |\n";
}

void printLine(string line)
{
    cout << "| " << line;

    int padSize = 4;
    int lnLength = HEADER.size() - line.size() - padSize;

    for (int i = 0; i < lnLength; i++)
    {
        cout << " ";
    }

    cout << " |\n";
}

void printBoxEnd()
{
    cout << "O";
    for (int i = 0; i < HEADER.size() - 2; i++)
    {
        cout << "=";
    }
    cout << "O\n";
}

void printBoxEndError()
{
    cout << "[!]";
    for (int i = 0; i < HEADER.size() - 6; i++)
    {
        cout << "=";
    }
    cout << "[!]\n";
}

void printLinePairError(string key, string value)
{

    cout << "\n";

    printBoxEndError();
    printLinePair("", "");
    cout << "| [!] " << key;

    int padSize = 12;
    int lnLength = HEADER.size() - key.size() - value.size() - padSize;

    for (int i = 0; i < lnLength; i++)
    {
        cout << " ";
    }

    cout << value << " [!] |\n";

    printLinePair("", "");
    printBoxEndError();
}

std::string formatSize(int size)
{
    std::string s = std::to_string(size);
    std::string formatted_s = "";
    int counter = 0;

    for (int i = s.length() - 1; i >= 0; --i)
    {
        formatted_s.push_back(s[i]);
        counter++;
        if (counter % 3 == 0 && i != 0)
        {
            formatted_s.push_back(',');
        }
    }
    std::reverse(formatted_s.begin(), formatted_s.end());

    formatted_s += " bytes";

    return formatted_s;
}

std::string getFileExtension(const std::string &filename)
{
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos)
    {
        return filename.substr(pos);
    }
    return "";
}

std::string removeExtension(const std::string &filename)
{
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos)
    {
        return "desqueezed-" + filename.substr(0, pos);
    }
    return "desqueezed-" + filename;
}

void displayBits(unsigned char byte)
{
    for (int bit = 7; bit >= 0; --bit)
    {
        std::cout << ((byte >> bit) & 1);
    }
    std::cout << " ";
}

void displayByte(string byte)
{
    for (unsigned char b : byte)
    {
        displayBits(b);
    }
    std::cout << "\n";
}

void displayByte(vector<unsigned char> byte)
{
    for (unsigned char b : byte)
    {
        displayBits(b);
    }
    std::cout << "\n";
}

bool encode(vector<unsigned char> &buffer)
{

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

    printLinePair("Input size: ", formatSize(buffer.size()));
    printLine("Encoding...");
    printBoxEnd();

    for (int i = 0; i < 256; i++)
    {
        dictionary[{static_cast<unsigned char>(i)}] = i;
    }

    int nextCode = 256;
    vector<unsigned char> w = {};

    for (int i = 0; i < buffer.size(); i++)
    {
        unsigned char c = buffer[i];
        vector<unsigned char> wc = w;
        wc.push_back(c);

        if (dictionary.find(wc) != dictionary.end())
        {
            w = wc;
        }
        else
        {
            dictionary[wc] = nextCode++;
            if (!w.empty())
            {
                output.push_back(dictionary[w]);
            }

            w = {c};
        }
    }

    if (!w.empty())
    {
        output.push_back(dictionary[w]);
    }

    printLine("SQUEEZED!");
    printLinePair("Output size: ", formatSize(output.size()));

    buffer.clear();
    for (int code : output)
    {
        buffer.push_back(code & 0xFF);
        buffer.push_back((code >> 8) & 0xFF);
    }

    printLinePair("New file size: ", formatSize(buffer.size()));
    printBoxEnd();

    return true;
}

bool decode(vector<unsigned char> &buffer)
{
    unordered_map<int, vector<unsigned char>> dictionary;
    vector<unsigned char> output;

    printLinePair("Input size: ", formatSize(buffer.size()));
    printLine("Decoding...");
    printBoxEnd();

    for (int i = 0; i < 256; i++)
    {
        dictionary[i] = {static_cast<unsigned char>(i)};
    }

    int currSizeDict = 256;

    vector<int> codes;
    if (buffer.size() % 2 != 0)
    {
        printLinePairError("ERROR: ", "invalid buffer size for 16-bit codes");
        return false;
    }

    for (int i = 0; i < buffer.size(); i += 2)
    {
        int code = buffer[i] | (buffer[i + 1] << 8);
        codes.push_back(code);
    }

    if (codes.empty())
    {
        printLinePairError("ERROR: ", "no codes to decode");
        return false;
    }

    int k = codes[0];
    cout << k;
    if (k >= 256)
    {
        printLinePairError("ERROR: ", "first code must be single character: " + to_string(k));
        return false;
    }

    vector<unsigned char> w = dictionary[k];
    output.insert(output.end(), w.begin(), w.end());

    for (int i = 1; i < codes.size(); i++)
    {
        k = codes[i];
        vector<unsigned char> entry;

        if (dictionary.find(k) != dictionary.end())
        {
            entry = dictionary[k];
        }
        else if (k == currSizeDict)
        {
            entry = w;
            entry.push_back(w[0]);
        }
        else
        {
            printLinePairError("ERROR: ", "invalid code encountered");
            return false;
        }

        output.insert(output.end(), entry.begin(), entry.end());

        vector<unsigned char> newEntry = w;
        newEntry.push_back(entry[0]);
        dictionary[currSizeDict] = newEntry;
        currSizeDict++;

        w = entry;
    }

    buffer.swap(output);
    return true;
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        std::cerr << "  - For compression: input any file, output will be .sqzd\n";
        std::cerr << "  - For decompression: input .sqzd file, output will be original extension\n";
        return 1;
    }

    cout << "\n"
         << HEADER << "\n";

    const char *inputFilename = argv[1];
    std::string inputStr(inputFilename);
    std::string inputExt = getFileExtension(inputStr);

    std::string outputFilename;
    bool isDecompression = (inputExt == ".sqzd");

    if (argc >= 3)
    {
        outputFilename = argv[2];
    }
    else
    {
        if (isDecompression)
        {
            outputFilename = removeExtension(inputStr);
        }
        else
        {
            outputFilename = inputStr + ".sqzd";
        }
    }

    printLinePair("", "");
    printLinePair((isDecompression ? "DECOMPRESSING: " : "COMPRESSING: "), inputFilename);
    printLinePair("OUTPUT: ", outputFilename);
    printLinePair("", "");
    printBoxEnd();

    std::ifstream input(inputFilename, std::ios::binary);
    if (!input)
    {
        printLinePairError("ERROR opening input file: ", inputFilename);
        return 1;
    }

    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(input)),
                                      std::istreambuf_iterator<char>());
    printLine("Buffer read complete");

    bool success = false;

    if (isDecompression)
    {
        success = decode(buffer);
    }
    else
    {
        success = encode(buffer);
    }

    if (!success)
        return 1;

    std::ofstream output(outputFilename, std::ios::binary);
    if (!output)
    {
        std::cerr << "Error opening output file: " << outputFilename << "\n";
        return 1;
    }

    output.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());

    cout << "\n";
    printBoxEnd();
    printLine("Write complete!");
    printLinePair("OUTPUT SAVED TO: ", outputFilename);
    printBoxEnd();
    cout << "\n";

    return 0;
}