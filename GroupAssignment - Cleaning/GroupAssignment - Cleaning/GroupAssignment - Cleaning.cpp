// GroupAssignment - Cleaning.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>

using namespace std; // Adding the 'using namespace std;'

struct NutrientsInfo
{
    string food;
    string measure;
    double grams;
    double calories;
    double protein;
    double fat;
    double satFat;
    double fiber;
    double carbs;
    string category;
};

double cleanAndConvertToDouble(const string& str) {
    if (str.empty() || str == "t" || str == "a")
    {
        return 0.0; // Handle empty or special cases
    }
    else if (str.find('/') != string::npos || isalpha(str[0])) { // Check for '/' or an alphabetical character
        return -1.0; // Return a default value for dates or non-numeric values
    }
    else {
        try {
            string cleanedStr = str;
            cleanedStr.erase(remove(cleanedStr.begin(), cleanedStr.end(), ','), cleanedStr.end());
            cleanedStr.erase(remove(cleanedStr.begin(), cleanedStr.end(), '"'), cleanedStr.end()); // cus got "1,419"
            return stod(cleanedStr);
        }
        catch (const invalid_argument& e) {
            cerr << "Invalid argument: " << e.what() << '\n';
            // Handle the error, e.g., return a default value or rethrow
            return 0.0;
        }
        catch (const out_of_range& e) {
            cerr << "Out of range: " << e.what() << '\n';
            // Handle the error
            return 0.0;
        }
    }
}

NutrientsInfo parseNutrientsInfo(const vector<string>& tokens) {
    NutrientsInfo info;
    if (tokens.size() >= 10) {
        info.food = tokens[0];
        info.measure = tokens[1];
        info.grams = cleanAndConvertToDouble(tokens[2]);
        info.calories = cleanAndConvertToDouble(tokens[3]);
        info.protein = cleanAndConvertToDouble(tokens[4]);
        info.fat = cleanAndConvertToDouble(tokens[5]);
        info.satFat = cleanAndConvertToDouble(tokens[6]);
        info.fiber = cleanAndConvertToDouble(tokens[7]);
        info.carbs = cleanAndConvertToDouble(tokens[8]);
        info.category = tokens[9];
    }
    return info;
}

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
void cleanData(vector<NutrientsInfo>& data) {
    // Remove leading and trailing spaces from all strings
    for (auto& entry : data) {
        entry.food.erase(remove(entry.food.begin(), entry.food.end(), ' '), entry.food.end());
        entry.measure.erase(remove(entry.measure.begin(), entry.measure.end(), ' '), entry.measure.end());
        entry.category.erase(remove(entry.category.begin(), entry.category.end(), ' '), entry.category.end());
    }

    // Remove rows with missing values (empty strings)
    data.erase(remove_if(data.begin(), data.end(),
        [](const NutrientsInfo& entry) {
            return (entry.food.empty() || entry.measure.empty() || entry.category.empty());
        }),
        data.end());

    // Convert all text to lowercase
    for (auto& entry : data) {
        transform(entry.food.begin(), entry.food.end(), entry.food.begin(), ::tolower);
        transform(entry.measure.begin(), entry.measure.end(), entry.measure.begin(), ::tolower);
        transform(entry.category.begin(), entry.category.end(), entry.category.begin(), ::tolower);
    }
}

// Function to check if a row already exists in the dataset
bool isDuplicate(const vector<NutrientsInfo>& data, const NutrientsInfo& info) {
    return find_if(data.begin(), data.end(),
        [&info](const NutrientsInfo& entry) {
            return (entry.food == info.food && entry.measure == info.measure);
        }) != data.end();
}

int main() {
    // Open the CSV file
    ifstream file("D:\\yyun\\Documents\\GitHub\\DSLTR\\GroupAssignment - Cleaning\\Nutrients_Info.csv");

    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return 1;
    }

    string line;
    vector<NutrientsInfo> originalData;
    vector<NutrientsInfo> uniqueData;
    vector<string> headers; // Store headers separately

    // Read the headers
    if (getline(file, line)) {
        stringstream ss(line);
        string header;

        while (getline(ss, header, ',')) {
            headers.push_back(header);
        }
    }
    else {
        cout << "Empty file or unable to read headers!" << endl;
        return 1;
    }

    while (getline(file, line)) {
        
        vector<string> tokens = splitCSV(line, ',');

        // Parse NutrientsInfo
        NutrientsInfo info = parseNutrientsInfo(tokens);

        // Check if the row is unique before adding it to uniqueData
        if (!isDuplicate(originalData, info)) {
            uniqueData.push_back(info);
        }

        // Always add the row to originalData
        originalData.push_back(info);
    }

    // Check for end-of-file or failure after the loop
    if (!file.eof() && file.fail()) {
        cout << "Error"; 
    }

    //Close the file
    file.close();

    // Perform data cleaning operations
    cleanData(uniqueData);

    // Print headers
    cout << "Headers: ";
    for (const auto& header : headers) {
        cout << header << " \t";
    }
    cout << endl;

    // Displaying the parsed data
    for (const auto& info : uniqueData) {
        //cout << "Food: " << info.food << ", Measure: " << info.measure << ", Grams: " << info.grams << ", Calories: " << info.calories << ", Protein: " << info.protein << ", Fat: " << info.fat << ", SatFat: " << info.satFat << ", Fiber: " << info.fiber << ", Carbs: " << info.carbs << ", Category: " << info.category << endl;
        cout << info.food << "\t" << info.measure << "\t" << info.grams << "\t" << info.calories << "\t" << info.protein << "\t" << info.fat << "\t" << info.satFat << "\t" << info.fiber << "\t" << info.carbs << "\t" << info.category << endl;

    }

    return 0;
}

void sortAlphabitically(vector<vector<string>> data) {
    //Sort uniqueData alphabetically (A-Z)
    sort(data.begin(), data.end());
}