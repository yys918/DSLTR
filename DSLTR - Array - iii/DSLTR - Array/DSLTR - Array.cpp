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
#include <chrono>    

using namespace std;
using namespace chrono; // For calculation of excution time

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

    // Convert all text to lowercase
    for (auto& entry : data) {
        transform(entry.food.begin(), entry.food.end(), entry.food.begin(), ::tolower);
        transform(entry.measure.begin(), entry.measure.end(), entry.measure.begin(), ::tolower);
        transform(entry.category.begin(), entry.category.end(), entry.category.begin(), ::tolower);
    }
}

void displayData(const vector<string>& headers, const vector<NutrientsInfo>& data) {
    // Print headers
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
        //cout << "Food: " << info.food << ", Measure: " << info.measure << ", Grams: " << info.grams << ", Calories: " << info.calories << ", Protein: " << info.protein << ", Fat: " << info.fat << ", SatFat: " << info.satFat << ", Fiber: " << info.fiber << ", Carbs: " << info.carbs << ", Category: " << info.category << endl;
        cout << setw(40) << left << info.food << setw(15) << left << info.measure << setw(10) << left << info.grams << setw(10) << left << info.calories << setw(10) << left << info.protein << setw(10) << left << info.fat << setw(10) << left << info.satFat << setw(10) << left << info.fiber << setw(10) << left << info.carbs << setw(10) << left << info.category << endl;

    }
}

// Function to perform selection sort on the vector of NutrientsInfo based on a specified column
void selectionSort(vector<NutrientsInfo>& data, bool ascending, int column) {
    int n = data.size();

    for (int i = 0; i < n - 1; i++) {
        int indexToCompare = i;
        for (int j = i + 1; j < n; j++) {
            // Compare based on the specified column
            switch (column) {
            case 1:  // Sorting based on food name
                if ((ascending && data[j].food < data[indexToCompare].food) ||
                    (!ascending && data[j].food > data[indexToCompare].food)) {
                    indexToCompare = j;
                }
                break;
            case 2:  // Sorting based on measure
                if ((ascending && data[j].measure < data[indexToCompare].measure) ||
                    (!ascending && data[j].measure > data[indexToCompare].measure)) {
                    indexToCompare = j;
                }
                break;
            case 3:  // Sorting based on grams
                if ((ascending && data[j].grams < data[indexToCompare].grams) ||
                    (!ascending && data[j].grams > data[indexToCompare].grams)) {
                    indexToCompare = j;
                }
                break;
            case 4:  // Sorting based on calories
                if ((ascending && data[j].calories < data[indexToCompare].calories) ||
                    (!ascending && data[j].calories > data[indexToCompare].calories)) {
                    indexToCompare = j;
                }
                break;
            case 5:  // Sorting based on protein
                if ((ascending && data[j].protein < data[indexToCompare].protein) ||
                    (!ascending && data[j].protein > data[indexToCompare].protein)) {
                    indexToCompare = j;
                }
                break;
            case 6:  // Sorting based on fat
                if ((ascending && data[j].fat < data[indexToCompare].fat) ||
                    (!ascending && data[j].fat > data[indexToCompare].fat)) {
                    indexToCompare = j;
                }
                break;
            case 7:  // Sorting based on saturated fat
                if ((ascending && data[j].satFat < data[indexToCompare].satFat) ||
                    (!ascending && data[j].satFat > data[indexToCompare].satFat)) {
                    indexToCompare = j;
                }
                break;
            case 8:  // Sorting based on fiber
                if ((ascending && data[j].fiber < data[indexToCompare].fiber) ||
                    (!ascending && data[j].fiber > data[indexToCompare].fiber)) {
                    indexToCompare = j;
                }
                break;
            case 9:  // Sorting based on carbs
                if ((ascending && data[j].carbs < data[indexToCompare].carbs) ||
                    (!ascending && data[j].carbs > data[indexToCompare].carbs)) {
                    indexToCompare = j;
                }
                break;
            case 10: // Sorting based on category
                if ((ascending && data[j].category < data[indexToCompare].category) ||
                    (!ascending && data[j].category > data[indexToCompare].category)) {
                    indexToCompare = j;
                }
                break;
            }
        }

        // Swap the found element with the first element
        swap(data[indexToCompare], data[i]);
    }
}

// Function to perform insertion sort on the vector of NutrientsInfo based on a specified column
void insertionSort(vector<NutrientsInfo>& data, bool ascending, int column) {
    int n = data.size();
    int i, j;
    string key;
    
    for (int i = 1; i < n; i++) {
        NutrientsInfo key = data[i];
        int j = i - 1;

        switch (column) {
        case 1: //food name
            
            while (j >= 0 && ((ascending && data[j].food > key.food) || (!ascending && data[j].food < key.food))) {
                data[j + 1] = data[j];
                j = j - 1;
            }
            break;

        case 2: //measure

            while (j >= 0 && ((ascending && data[j].measure > key.measure) || (!ascending && data[j].measure < key.measure))) {
                data[j + 1] = data[j];
                j = j - 1;
            }
            break;

        case 3: //grams
            
            while (j >= 0 && ((ascending && data[j].grams > key.grams) || (!ascending && data[j].grams < key.grams))) {
                data[j + 1] = data[j];
                j = j - 1;
            }
            break;

        case 4: //calories

            while (j >= 0 && ((ascending && data[j].calories > key.calories) || (!ascending && data[j].calories < key.calories))) {
                data[j + 1] = data[j];
                j = j - 1;
            }
            break;

        case 5: //protein

            while (j >= 0 && ((ascending && data[j].protein > key.protein) || (!ascending && data[j].protein < key.protein))) {
                data[j + 1] = data[j];
                j = j - 1;
            }
            break;

        case 6: //fat

            while (j >= 0 && ((ascending && data[j].fat > key.fat) || (!ascending && data[j].fat < key.fat))) {
                data[j + 1] = data[j];
                j = j - 1;
            }
            break;

        case 7: //satFat

            while (j >= 0 && ((ascending && data[j].satFat > key.satFat) || (!ascending && data[j].satFat < key.satFat))) {
                data[j + 1] = data[j];
                j = j - 1;
            }
            break;

        case 8: //fiber

            while (j >= 0 && ((ascending && data[j].fiber > key.fiber) || (!ascending && data[j].fiber < key.fiber))) {
                data[j + 1] = data[j];
                j = j - 1;
            }
            break;

        case 9: //carbs

            while (j >= 0 && ((ascending && data[j].carbs > key.carbs) || (!ascending && data[j].carbs < key.carbs))) {
                data[j + 1] = data[j];
                j = j - 1;
            }
            break;

        case 10: //category

            while (j >= 0 && ((ascending && data[j].category > key.category) || (!ascending && data[j].category < key.category))) {
                data[j + 1] = data[j];
                j = j - 1;
            }
            break;

        default:
            cerr << "Invalid column number" << endl;
        }
        
        data[j + 1] = key;
    }
}

int main() {
    // Open the CSV file
    ifstream file("C:\\Users\\Natalie\\OneDrive - Asia Pacific University\\Documents\\APU\\Y2S2\\DSTR (C++)\\ASMT\\Nutrients_Info.csv");

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

    while (true) {

        int num, column, choice;
        bool ascending;

        cout << "What sorting algorithm do you want to use?\n"
            << "1. Insertion Sort\n2. Selection Sort\n3. Bubble Sort\nChoice:  ";
        cin >> num;

        cout << "Enter the column number to sort (1-9)\n1. Food Name\n2. Measure\n3. Grams\n4. Calories\n5. Protein\n6. Fat\n7. Sat. fat\n8. Fiber\n9. Carbs\n10. Category\nChoice: ";
        cin >> column;
        column = column;

        cout << "Enter sorting order\n1. Descending\n2. Ascending\nChoice: ";
        cin >> choice;
        if (choice == 1) {
            ascending = false;
        }
        else if (choice == 2) {
            ascending = true;
        }
        else {
            cout << "Please enter a valid chioce." << endl;
            return 1;
        }
        cout << ascending << endl;

        if (num == 1) {
            auto start = high_resolution_clock::now();

            insertionSort(data, ascending, column);
            displayData(headers, data);

            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            cout << "Sorting time: " << duration.count() << " milliseconds" << endl;

        }

        else if (num == 2) {
            auto start = high_resolution_clock::now();

            selectionSort(data, ascending, column);
            displayData(headers, data);

            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            cout << "Sorting time: " << duration.count() << " milliseconds" << endl;
        }

        else if (num == 3) {
            auto start = high_resolution_clock::now();

            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(stop - start);
            cout << "Sorting time: " << duration.count() << " milliseconds" << endl;
        }

        else {
            cout << "Please enter a valid input";
        }
    }

    return 0;
}

