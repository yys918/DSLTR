// GroupAssignment - Cleaning.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>

using namespace std; // Adding the 'using namespace std;'

// Function to split a string based on a delimiter, handling quoted strings
vector<string> splitCSV(const string& s, char delimiter) {
    vector<string> tokens;
    stringstream ss(s);
    string token;
    bool inQuotedString = false;
    string currentToken;

    while (getline(ss, token, delimiter)) {
        // Check if the token contains an opening quote
        if (!token.empty() && token.front() == '"') {
            inQuotedString = true;
            currentToken = token;
            continue;
        }

        // If we're in a quoted string, append the token until we find the closing quote
        if (inQuotedString) {
            currentToken += (delimiter + token);

            // Check if the current token ends with a closing quote
            if (token.back() == '"') {
                inQuotedString = false;
                tokens.push_back(currentToken);
            }
        }
        else {
            tokens.push_back(token);
        }
    }

    return tokens;
}

// Function to split a string based on a delimiter
/*vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    stringstream ss(s);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}*/

// Function to remove leading and trailing spaces from strings
void trimSpaces(vector<string>& tokens) {
    for (auto& token : tokens) {
        size_t start = token.find_first_not_of(" \t");
        size_t end = token.find_last_not_of(" \t");
        if (start != string::npos && end != string::npos) {
            token = token.substr(start, end - start + 1);
        }
        else {
            token.clear();
        }
    }
}

// Function to perform data cleaning operations
void cleanData(vector<vector<string>>& data) {
    // Remove leading and trailing spaces from all strings
    for (auto& row : data) {
        for (auto& cell : row) { //not call function cuz different argument,this is string
            size_t start = cell.find_first_not_of(" \t");
            size_t end = cell.find_last_not_of(" \t");
            if (start != string::npos && end != string::npos) {
                cell = cell.substr(start, end - start + 1);
            }
            else {
                cell.clear(); // Remove the entire string if it only contains spaces
            }
        }
    }

    // Remove rows with missing values (empty strings)
    data.erase(remove_if(data.begin(), data.end(),
        [](const vector<string>& row) {
            return any_of(row.begin(), row.end(), [](const string& cell) {
                return cell.empty();
                });
        }),
        data.end());

    // Convert all text to lowercase
    for (auto& row : data) {
        for (auto& cell : row) {
            transform(cell.begin(), cell.end(), cell.begin(), ::tolower);
        }
    }
}

// Function to check if a row already exists in the dataset
bool isDuplicate(const vector<vector<string>>& data, const vector<string>& row) {
    return find(data.begin(), data.end(), row) != data.end();
}

int main() {
    // Open the CSV file
    ifstream file("C:\\Users\\yyun\\OneDrive - Asia Pacific University\\Documents\\Year 2\\Data Structures\\Assignments\\GroupAssignment - Cleaning\\Nutrients_Info.csv");

    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return 1;
    }

    string line;
    vector<vector<string>> originalData;
    vector<vector<string>> uniqueData;
    vector<string> headers; // Store headers separately

    // Read the headers
    /*if (getline(file, line)) {
        headers = split(line, ',');
        trimSpaces(headers);
    }*/

    while (getline(file, line)) {
        vector<string> tokens = splitCSV(line, ',');

        // Check if the row is unique before adding it to uniqueData
        if (!isDuplicate(uniqueData, tokens)) {
            uniqueData.push_back(tokens);
        }

        // Always add the row to originalData
        originalData.push_back(tokens);
    }

    // Close the file
    file.close();

    // Perform data cleaning operations
    cleanData(uniqueData);

    // Sort uniqueData alphabetically (A-Z)
    // sort(uniqueData.begin(), uniqueData.end());
    // sort(originalData.begin(), originalData.end());

    // Get the total number of lines in uniqueData
    cout << "Total number of lines in uniqueData: " << uniqueData.size() << endl;

    // Printing each line stored as an array
    for (const auto& lineArray : originalData) {
        int i = 0;
        for (const auto& token : lineArray) {
            cout << token << " ";
            cout << i << " ";
            i++;
            // Here, you can process the data or store it in arrays as needed
        }
        cout << endl;
    }

    return 0;
}