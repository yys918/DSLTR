// GroupAssignment - Cleaning.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <regex>

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
            // Handle the error by return default value
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
        // Clean the food name by removing leading and trailing double quotes
        if (tokens[0].front() == '"' && tokens[0].back() == '"') {
            info.food = tokens[0].substr(1, tokens[0].size() - 2);
        }
        else if (tokens[0].front() == '(' && tokens[0].back() == ')') {
            info.food = tokens[0].substr(1, tokens[0].size() - 2);
        }
        else {
            info.food = tokens[0];
        }

        info.measure = tokens[1];
        info.grams = cleanAndConvertToDouble(tokens[2]);
        info.calories = cleanAndConvertToDouble(tokens[3]);
        info.protein = cleanAndConvertToDouble(tokens[4]);
        info.fat = cleanAndConvertToDouble(tokens[5]);
        info.satFat = cleanAndConvertToDouble(tokens[6]);
        info.fiber = cleanAndConvertToDouble(tokens[7]);
        info.carbs = cleanAndConvertToDouble(tokens[8]);

        // Check if the category is surrounded by double quotes
        if (tokens[9].front() == '"' && tokens[9].back() == '"') {
            // Remove the double quotes from the category
            info.category = tokens[9].substr(1, tokens[9].size() - 2);
        }
        else {
            info.category = tokens[9];
        }
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
        if (!inQuotedString) {
            size_t quoteCount = count(token.begin(), token.end(), '"');
            if (quoteCount % 2 != 0) {
                // Odd number of quotes indicates the start of a quoted string
                inQuotedString = true;
                currentToken = token;
                continue;
            }
        }

        if (inQuotedString) {
            currentToken += (delimiter + token);

            // Check if the current token ends with a closing quote
            size_t lastQuotePos = currentToken.find_last_of('"');
            if (lastQuotePos != string::npos && lastQuotePos == currentToken.size() - 1) {
                inQuotedString = false;

                // Remove the surrounding quotes and handle escaped quotes
                regex pattern("\"\"");
                currentToken = regex_replace(currentToken.substr(1, currentToken.size() - 2), pattern, "\"");
                tokens.push_back(currentToken);
            }
        }
        else {
            tokens.push_back(token);
        }
    }
    return tokens;
}

// Function to perform data cleaning operations
void cleanData(vector<NutrientsInfo>& data) {
    const string WHITESPACE = " \t"; // Define a constant for whitespace characters

    // Remove leading and trailing spaces from all strings
    for (auto& entry : data) {
        // Format and trim spaces for food
        size_t foodStart = entry.food.find_first_not_of(WHITESPACE);
        size_t foodEnd = entry.food.find_last_not_of(WHITESPACE);
        entry.food = (foodStart != string::npos) ? entry.food.substr(foodStart, foodEnd - foodStart + 1) : "";

        // Format and trim spaces for measure
        size_t measureStart = entry.measure.find_first_not_of(WHITESPACE);
        size_t measureEnd = entry.measure.find_last_not_of(WHITESPACE);
        entry.measure = (measureStart != string::npos) ? entry.measure.substr(measureStart, measureEnd - measureStart + 1) : "";

        // Format and trim spaces for category
        size_t categoryStart = entry.category.find_first_not_of(WHITESPACE);
        size_t categoryEnd = entry.category.find_last_not_of(WHITESPACE);
        entry.category = (categoryStart != string::npos) ? entry.category.substr(categoryStart, categoryEnd - categoryStart + 1) : "";
    }

    // Remove rows with missing values (empty strings)
    data.erase(remove_if(data.begin(), data.end(),
        [](const NutrientsInfo& entry) {
            return (entry.food.empty() || entry.measure.empty() || entry.category.empty());
        }),
        data.end());
}

int main() {
    // Open the CSV file
    ifstream file("D:\\yyun\\Documents\\GitHub\\DSLTR\\GroupAssignment - Cleaning\\Nutrients_Info.csv");

    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return 1;
    }

    string line;
    vector<NutrientsInfo> data;
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
        // Read the entire line and then split it
        vector<string> tokens = splitCSV(line, ',');

        // Parse NutrientsInfo
        NutrientsInfo info = parseNutrientsInfo(tokens);

        data.push_back(info);
    }

    // Check for end-of-file or failure after the loop
    if (!file.eof() && file.fail()) {
        cout << "Error";
    }

    //Close the file
    file.close();

    // Perform data cleaning operations
    cleanData(data);

    // Print headers
    //cout << "Headers: ";
    int i = 0;
    for (const auto& header : headers) {
        if (i == 0) {
            cout << setw(45) << left << header;
            i++;
        }
        else if (i == 1) {
            cout << setw(15) << left << header;
            i++;
        }
        else {
            cout << setw(10) << left << header;
            i++;
        }

    }
    cout << endl;

    // Displaying the parsed data
    for (const auto& info : data) {
cout << setw(45) << left << info.food << setw(15) << left << info.measure << setw(10) << left << info.grams << setw(10) << left << info.calories << setw(10) << left << info.protein << setw(10) << left << info.fat << setw(10) << left << info.satFat << setw(10) << left << info.fiber << setw(10) << left << info.carbs << setw(10) << left << info.category << endl;

    }

    return 0;
}