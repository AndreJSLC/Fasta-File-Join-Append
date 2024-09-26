#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Function to read FASTA file and determine the maximum characters per line
pair<vector<pair<string, string>>, int> read_fasta(const string& filepath) {
    vector<pair<string, string>> sequences;
    ifstream infile(filepath);
    string line, sequence, description;
    int max_chars_per_line = 0;

    while (getline(infile, line)) {
        if (line[0] == '>') {
            if (!sequence.empty()) {
                sequences.push_back({ description, sequence });
                sequence.clear();
            }
            description = line.substr(1);
        }
        else {
            sequence += line;
            if (max_chars_per_line == 0) {
                max_chars_per_line = line.length();
            }
        }
    }
    if (!sequence.empty()) {
        sequences.push_back({ description, sequence });
    }

    return { sequences, max_chars_per_line };
}

int main() {
    string firstFastaFile;
    cout << "Enter the path to the first FASTA file: ";
    getline(cin, firstFastaFile);

    string secondFastaFile;
    cout << "Enter the path to the second FASTA file: ";
    getline(cin, secondFastaFile);

    string outputFilePath;
    cout << "Enter the output path for the combined FASTA file: ";
    getline(cin, outputFilePath);

    auto firstResult = read_fasta(firstFastaFile);
    vector<pair<string, string>> firstFastaFileSequences = firstResult.first;
    int max_chars_per_line_forward = firstResult.second;

    auto secondResult = read_fasta(secondFastaFile);
    vector<pair<string, string>> secondFastaFileSequences = secondResult.first;
    int max_chars_per_line_reverse = secondResult.second;

    // Remove the key-value pairs where the key contains the substring "mitochondrion"
    firstFastaFileSequences.erase(remove_if(firstFastaFileSequences.begin(), firstFastaFileSequences.end(),
        [](const pair<string, string>& p) {
            return p.first.find("mitochondrion") != string::npos;
        }),
        firstFastaFileSequences.end());

    // Convert the sequences to uppercase
    for (auto& entry : firstFastaFileSequences) {
        transform(entry.second.begin(), entry.second.end(), entry.second.begin(), ::toupper);
    }

    ofstream outputFile(outputFilePath);
    if (!outputFile) {
        cerr << "Failed to create the output file." << endl;
        return 1;
    }

    // Write the first fasta file sequences to the output file
    for (const auto& entry : firstFastaFileSequences) {
        outputFile << ">" << entry.first << "\n";
        for (size_t i = 0; i < entry.second.length(); i += max_chars_per_line_forward) {
            outputFile << entry.second.substr(i, max_chars_per_line_forward) << "\n";
        }
    }

    // Append the second fasta file sequences to the output file
    for (const auto& entry : secondFastaFileSequences) {
        outputFile << ">" << entry.first << "\n";
        for (size_t i = 0; i < entry.second.length(); i += max_chars_per_line_reverse) {
            outputFile << entry.second.substr(i, max_chars_per_line_reverse) << "\n";
        }
    }

    outputFile.close();

    cout << "Combined FASTA file generated successfully." << endl;
    cout << "Output file: " << outputFilePath << endl;

    return 0;
}
