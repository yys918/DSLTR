// GroupAssignment - LinkedList.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

using namespace std; // Adding the 'using namespace std;'
using namespace chrono;

struct NutrientsInfo {
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
    NutrientsInfo* next; // Pointer to the next node in the linked list
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

NutrientsInfo* parseNutrientsInfo(const vector<string>& tokens) {
    NutrientsInfo* info = nullptr;

    if (tokens.size() >= 10) {
        info = new NutrientsInfo();

        // Clean the food name by removing leading and trailing double quotes
        if (tokens[0].front() == '"' && tokens[0].back() == '"') {
            info->food = tokens[0].substr(1, tokens[0].size() - 2);
        }
        else if (tokens[0].front() == '(' && tokens[0].back() == ')') {
            info->food = tokens[0].substr(1, tokens[0].size() - 2);
        }
        else {
            info->food = tokens[0];
        }

        info->measure = tokens[1];
        info->grams = cleanAndConvertToDouble(tokens[2]);
        info->calories = cleanAndConvertToDouble(tokens[3]);
        info->protein = cleanAndConvertToDouble(tokens[4]);
        info->fat = cleanAndConvertToDouble(tokens[5]);
        info->satFat = cleanAndConvertToDouble(tokens[6]);
        info->fiber = cleanAndConvertToDouble(tokens[7]);
        info->carbs = cleanAndConvertToDouble(tokens[8]);

        // Check if the category is surrounded by double quotes
        if (tokens[9].front() == '"' && tokens[9].back() == '"') {
            // Remove the double quotes from the category
            info->category = tokens[9].substr(1, tokens[9].size() - 2);
        }
        else {
            info->category = tokens[9];
        }

        info->next = nullptr; // Ensure the next pointer is initialized for the linked list
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
void cleanData(NutrientsInfo*& head) {
    const string WHITESPACE = " \t"; // Define a constant for whitespace characters

    NutrientsInfo* current = head;

    while (current != nullptr) {
        // Format and trim spaces for food
        size_t foodStart = current->food.find_first_not_of(WHITESPACE);
        size_t foodEnd = current->food.find_last_not_of(WHITESPACE);
        current->food = (foodStart != string::npos) ? current->food.substr(foodStart, foodEnd - foodStart + 1) : "";

        // Format and trim spaces for measure
        size_t measureStart = current->measure.find_first_not_of(WHITESPACE);
        size_t measureEnd = current->measure.find_last_not_of(WHITESPACE);
        current->measure = (measureStart != string::npos) ? current->measure.substr(measureStart, measureEnd - measureStart + 1) : "";

        // Format and trim spaces for category
        size_t categoryStart = current->category.find_first_not_of(WHITESPACE);
        size_t categoryEnd = current->category.find_last_not_of(WHITESPACE);
        current->category = (categoryStart != string::npos) ? current->category.substr(categoryStart, categoryEnd - categoryStart + 1) : "";

        // Move to the next node in the linked list
        current = current->next;
    }

    // Remove nodes with missing values (empty strings)
    NutrientsInfo* prev = nullptr;
    current = head;

    while (current != nullptr) {
        if (current->food.empty() || current->measure.empty() || current->category.empty()) {
            // Node has missing values, remove it from the linked list
            NutrientsInfo* temp = current;

            if (prev != nullptr) {
                prev->next = current->next;
                current = current->next;
            }
            else {
                head = current->next;
                current = head;
            }

            delete temp;
        }
        else {
            prev = current;
            current = current->next;
        }
    }
}

void displayData(const vector<string>& headers, NutrientsInfo* head) {
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

    // Displaying the parsed data from the linked list
    NutrientsInfo* current = head;
    while (current != nullptr) {
        cout << setw(45) << left << current->food
            << setw(15) << left << current->measure
            << setw(10) << left << current->grams
            << setw(10) << left << current->calories
            << setw(10) << left << current->protein
            << setw(10) << left << current->fat
            << setw(10) << left << current->satFat
            << setw(10) << left << current->fiber
            << setw(10) << left << current->carbs
            << setw(10) << left << current->category << endl;

        // Move to the next node in the linked list
        current = current->next;
    }
}

// Function to perform selection sort on the linked list
void selectionSort(NutrientsInfo*& head, bool ascendingOrder, int sortColumn) {
    NutrientsInfo* current = head;

    while (current) {
        NutrientsInfo* minMaxNode = current;
        NutrientsInfo* temp = current->next;

        // Find the minimum or maximum element in the remaining unsorted portion
        while (temp) {
            switch (sortColumn) {
            case 1: // Food
                if ((ascendingOrder && temp->food < minMaxNode->food) || (!ascendingOrder && temp->food > minMaxNode->food)) {
                    minMaxNode = temp;
                }
                break;
            case 2: // Measure
                if ((ascendingOrder && temp->measure < minMaxNode->measure) || (!ascendingOrder && temp->measure > minMaxNode->measure)) {
                    minMaxNode = temp;
                }
                break;
            case 3: // Grams
                if ((ascendingOrder && temp->grams < minMaxNode->grams) || (!ascendingOrder && temp->grams > minMaxNode->grams)) {
                    minMaxNode = temp;
                }
                break;
            case 4: // Calories
                if ((ascendingOrder && temp->calories < minMaxNode->calories) || (!ascendingOrder && temp->calories > minMaxNode->calories)) {
                    minMaxNode = temp;
                }
                break;
            case 5: // Protein
                if ((ascendingOrder && temp->protein < minMaxNode->protein) || (!ascendingOrder && temp->protein > minMaxNode->protein)) {
                    minMaxNode = temp;
                }
                break;
            case 6: // Fat
                if ((ascendingOrder && temp->fat < minMaxNode->fat) || (!ascendingOrder && temp->fat > minMaxNode->fat)) {
                    minMaxNode = temp;
                }
                break;
            case 7: // Sat.Fat
                if ((ascendingOrder && temp->satFat < minMaxNode->satFat) || (!ascendingOrder && temp->satFat > minMaxNode->satFat)) {
                    minMaxNode = temp;
                }
                break;
            case 8: // Fiber
                if ((ascendingOrder && temp->fiber < minMaxNode->fiber) || (!ascendingOrder && temp->fiber > minMaxNode->fiber)) {
                    minMaxNode = temp;
                }
                break;
            case 9: // Carbs
                if ((ascendingOrder && temp->carbs < minMaxNode->carbs) || (!ascendingOrder && temp->carbs > minMaxNode->carbs)) {
                    minMaxNode = temp;
                }
                break;
            case 10: // Category
                if ((ascendingOrder && temp->category < minMaxNode->category) || (!ascendingOrder && temp->category > minMaxNode->category)) {
                    minMaxNode = temp;
                }
                break;
            }

            temp = temp->next;
        }

        // Swap the found minimum or maximum element with the current node
        swap(current->food, minMaxNode->food);
        swap(current->measure, minMaxNode->measure);
        swap(current->grams, minMaxNode->grams);
        swap(current->calories, minMaxNode->calories);
        swap(current->protein, minMaxNode->protein);
        swap(current->fat, minMaxNode->fat);
        swap(current->satFat, minMaxNode->satFat);
        swap(current->fiber, minMaxNode->fiber);
        swap(current->carbs, minMaxNode->carbs);
        swap(current->category, minMaxNode->category);

        current = current->next;
    }
}

// Function to swap two nodes in the linked list
NutrientsInfo* swap(NutrientsInfo* node1, NutrientsInfo* node2) {
    node1->next = node2->next;
    node2->next = node1;

    return node2;
}

void bubbleSort(NutrientsInfo*& head, bool ascendingOrder, int sortColumn) {
    int count = 0;
    NutrientsInfo* current = head;

    // Count the number of nodes in the linked list
    while (current) {
        count++;
        current = current->next;
    }

    for (int i = 0; i < count; i++) {
        NutrientsInfo** h = &head;
        int swapped = 0;

        for (int j = 0; j < count - i - 1; j++) {
            NutrientsInfo* p1 = *h;
            NutrientsInfo* p2 = p1->next;

            switch (sortColumn) {
            case 1: // Food
                if ((ascendingOrder && p1->food > p2->food) || (!ascendingOrder && p1->food < p2->food)) {
                    *h = swap(p1, p2);
                    swapped = 1;
                }
                break;
            case 2: // Measure
                if ((ascendingOrder && p1->measure > p2->measure) || (!ascendingOrder && p1->measure < p2->measure)) {
                    *h = swap(p1, p2);
                    swapped = 1;
                }
                break;
            case 3: // Grams
                if ((ascendingOrder && p1->grams > p2->grams) || (!ascendingOrder && p1->grams < p2->grams)) {
                    *h = swap(p1, p2);
                    swapped = 1;
                }
                break;
            case 4: // Calories
                if ((ascendingOrder && p1->calories > p2->calories) || (!ascendingOrder && p1->calories < p2->calories)) {
                    *h = swap(p1, p2);
                    swapped = 1;
                }
                break;
            case 5: // Protein
                if ((ascendingOrder && p1->protein > p2->protein) || (!ascendingOrder && p1->protein < p2->protein)) {
                    *h = swap(p1, p2);
                    swapped = 1;
                }
                break;
            case 6: // Fat
                if ((ascendingOrder && p1->fat > p2->fat) || (!ascendingOrder && p1->fat < p2->fat)) {
                    *h = swap(p1, p2);
                    swapped = 1;
                }
                break;
            case 7: // Sat.Fat
                if ((ascendingOrder && p1->satFat > p2->satFat) || (!ascendingOrder && p1->satFat < p2->satFat)) {
                    *h = swap(p1, p2);
                    swapped = 1;
                }
                break;
            case 8: // Fiber
                if ((ascendingOrder && p1->fiber > p2->fiber) || (!ascendingOrder && p1->fiber < p2->fiber)) {
                    *h = swap(p1, p2);
                    swapped = 1;
                }
                break;
            case 9: // Carbs
                if ((ascendingOrder && p1->carbs > p2->carbs) || (!ascendingOrder && p1->carbs < p2->carbs)) {
                    *h = swap(p1, p2);
                    swapped = 1;
                }
                break;
            case 10: // Category
                if ((ascendingOrder && p1->category > p2->category) || (!ascendingOrder && p1->category < p2->category)) {
                    *h = swap(p1, p2);
                    swapped = 1;
                }
                break;
            default:
                break;
                // Add cases for other columns as needed

            }

            h = &(*h)->next;
        }

        if (swapped == 0)
            break;
    }
}



int main() {
    ifstream file("C:\\Users\\Aaren Choo\\Documents\\Year 2\\Sem 2\\DSTR\\Assignment\\Nutrients_Info 1.csv");

    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return 1;
    }

    string line;
    NutrientsInfo* head = nullptr;
    vector<string> headers;

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
        NutrientsInfo* info = parseNutrientsInfo(tokens);

        info->next = head;
        head = info;
    }

    if (!file.eof() && file.fail()) {
        cout << "Error" << endl;
    }

    file.close();

    cleanData(head);

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
        cout << "Please enter a valid choice." << endl;
        return 1;
    }

    if (num == 2) {
        auto start = high_resolution_clock::now();
        selectionSort(head, ascending, column);
        displayData(headers, head);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout << "Sorting time: " << duration.count() << " milliseconds" << endl;
    }
    // Add similar blocks for other sorting algorithms
    else if (num == 3) {
        auto start = high_resolution_clock::now();
        bubbleSort(head, ascending, column);
        displayData(headers, head);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        cout << "Sorting time: " << duration.count() << " milliseconds" << endl;
    }
    // Release memory allocated for the linked list
    while (head != nullptr) {
        NutrientsInfo* temp = head;
        head = head->next;
        delete temp;
    }

    return 0;
}