//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Kileigh Adams
// Version     : 1.0
// Description : CS 300 Project Two
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>

using namespace std;

//============================================================================
// Constants
//============================================================================
const string COURSE_DATA_FILE = "CS 300 ABCU_Advising_Program_Input.csv";

//============================================================================
// Structures 
//============================================================================
struct Course { // Represents a course with its number, title, and prerequisites
    string number;
    string title;
    vector<string> prerequisites;
};

//============================================================================
// Function Prototypes
//============================================================================
void ToUpperInPlace(string& s); // Converts a string to uppercase in place
vector<string> SplitCSV(const string& line); // Splits a CSV line into parts
int FindCourseIndex(const vector<Course>& courses, const string& numberUpper); // Finds the index of a course by its number in uppercase
void SelectionSortByNumber(vector<Course>& courses); // Sorts courses by their number using selection sort
bool ValidatePrerequisites(const vector<Course>& courses, string& outError); // Validates that all prerequisites are defined as courses

bool LoadCoursesFromFile(const string& filename, vector<Course>& courses, string& outError); // Loads courses from a CSV file into a vector of Course structures
void DisplayCourse(const Course& c); // Displays a course's number and title
void PrintCourseList(vector<Course> courses); // Prints a list of courses in a sample schedule
void PrintCourseInfo_NumbersOnly(const vector<Course>& courses, const string& userInput); // Prints information about a course based on its number input by the user

void PrintMenu(); // Prints the main menu options

//============================================================================
// Utility Functions
//============================================================================
void ToUpperInPlace(string& s) { // Converts a string to uppercase in place
    for (int i = 0; i < (int)s.size(); i++) { 
		unsigned char ch = static_cast<unsigned char>(s[i]); // Ensure char is treated as unsigned to avoid issues with negative values
		s[i] = static_cast<char>(toupper(ch)); // Convert character to uppercase
    }
}

vector<string> SplitCSV(const string& line) { // Splits a CSV line into parts based on commas
	vector<string> parts; // Initialize a vector to hold the parts
	string item; // Temporary string to hold each part
	stringstream ss(line); // Create a stringstream from the line
	while (getline(ss, item, ',')) { // Read each part separated by commas
		parts.push_back(item); // Add the part to the vector
    }
	return parts; // Return the vector of parts
}

int FindCourseIndex(const vector<Course>& courses, const string& numberUpper) { // Finds the index of a course by its number in uppercase
	for (int i = 0; i < (int)courses.size(); i++) { // Iterate through the courses vector
		if (courses[i].number == numberUpper) { // If the course number matches the input
			return i; // Return the index of the course
        }
    }
	return -1; // If not found, return -1
}

void SelectionSortByNumber(vector<Course>& courses) { // Sorts courses by their number using selection sort
	for (int i = 0; i < (int)courses.size() - 1; i++) { // Iterate through the courses vector
		int minIdx = i; // Assume the current index is the minimum
		for (int j = i + 1; j < (int)courses.size(); j++) { // Compare with the rest of the vector
			if (courses[j].number < courses[minIdx].number) { // If a smaller number is found
				minIdx = j; // Update the minimum index
            }
        }
		if (minIdx != i) { // If the minimum index is not the current index
			Course tmp = courses[i]; // Swap the courses    
			courses[i] = courses[minIdx]; // Place the course with the smallest number at the current index
			courses[minIdx] = tmp; // Place the current course at the minimum index
        }
    }
}

bool ValidatePrerequisites(const vector<Course>& courses, string& outError) { // Validates that all prerequisites are defined as courses
	for (int i = 0; i < (int)courses.size(); i++) { // Iterate through each course
		for (int j = 0; j < (int)courses[i].prerequisites.size(); j++) { // Iterate through each prerequisite of the course
			string p = courses[i].prerequisites[j]; // Get the prerequisite number
			if (FindCourseIndex(courses, p) < 0) { // Check if the prerequisite is defined as a course
				outError = "Error: prerequisite \"" + p + "\" is not defined as a course."; // If not found, set the error message
				return false; // Return false indicating validation failed
            }
        }
    }
    return true;
}

//============================================================================
// Loading and Printing
//============================================================================
bool LoadCoursesFromFile(const string& filename, vector<Course>& courses, string& outError) { // Loads courses from a CSV file into a vector of Course structures
	courses.clear(); // Clear the courses vector to start fresh
	outError.clear(); // Clear any previous error message

	ifstream in(filename.c_str()); // Open the file for reading
	if (!in) { // Check if the file was opened successfully
		outError = "Error: could not open file \"" + filename + "\"."; // Set the error message if the file cannot be opened
		return false; // Return false indicating failure to load courses
    }

	struct Row { string number; string title; vector<string> prereqs; }; // Helper structure to hold course data temporarily
	vector<Row> rows; // Temporary vector to hold rows of course data

	string line; // Read each line from the file
	int lineNum = 0; // Initialize line number for error reporting

	while (getline(in, line)) { // Read each line from the file
		lineNum++; // Increment line number for error reporting
		if (line.size() == 0) { // Skip empty lines
			continue; // Continue to the next line if the current line is empty
        }

		vector<string> parts = SplitCSV(line); // Split the line into parts based on commas
		if ((int)parts.size() < 2) { // Check if there are at least two parts (course number and title)
			outError = "Error: line " + to_string(lineNum) + " must have at least a course number and title."; // Set error message if not enough parts
			return false; // Return false indicating failure to load courses
        }

		string num = parts[0]; // Get the course number from the first part
		ToUpperInPlace(num); // Convert the course number to uppercase

		string title = parts[1]; // Get the course title from the second part

		vector<string> prereqs; // Initialize a vector to hold prerequisites
		for (int i = 2; i < (int)parts.size(); i++) { // Iterate through the remaining parts for prerequisites
			string p = parts[i]; // Get the prerequisite number
			ToUpperInPlace(p); // Convert the prerequisite number to uppercase
			if (!p.empty()) { // Check if the prerequisite number is not empty
				prereqs.push_back(p); // Add the prerequisite to the vector
            }
        }

		for (int i = 0; i < (int)rows.size(); i++) { // Check for duplicate course numbers in the temporary rows vector
			if (rows[i].number == num) { // If a duplicate course number is found
				outError = "Error: duplicate course number \"" + num + "\"."; // Set error message for duplicate course number
				return false; // Return false indicating failure to load courses
            }
        }

		// Create a Row structure to hold the course data
        Row r;
        r.number = num;
        r.title = title;
        r.prereqs = prereqs;
		rows.push_back(r); // Add the Row to the temporary vector
    }

	for (int i = 0; i < (int)rows.size(); i++) { // Iterate through the temporary rows vector to create Course structures
		Course c; // Create a Course structure
        c.number = rows[i].number;
        c.title = rows[i].title;
        c.prerequisites = rows[i].prereqs;
        courses.push_back(c);
    }

	if (!ValidatePrerequisites(courses, outError)) { // Validate that all prerequisites are defined as courses
		courses.clear(); // Clear the courses vector if validation fails
        return false;
    }

    return true;
}

void DisplayCourse(const Course& c) { // Displays a course's number and title
	cout << c.number << ", " << c.title << "\n"; // Print the course number and title
}

void PrintCourseList(vector<Course> courses) { // Prints a list of courses in a sample schedule
	SelectionSortByNumber(courses); // Sort the courses by their number using selection sort
	cout << "Here is a sample schedule:\n"; // Print a message indicating a sample schedule
	for (int i = 0; i < (int)courses.size(); i++) { // Iterate through the sorted courses vector
        DisplayCourse(courses[i]);
    }
}
// Prints information about a course based on its number input by the user
void PrintCourseInfo_NumbersOnly(const vector<Course>& courses, const string& userInput) {
    string key = userInput;
    ToUpperInPlace(key);

	int idx = FindCourseIndex(courses, key); // Find the index of the course by its number in uppercase
    if (idx < 0) {
        cout << "Course \"" << key << "\" not found.\n";
        return;
    }

	const Course& c = courses[idx]; // Get the course from the courses vector using the found index
    DisplayCourse(c);

	if (c.prerequisites.empty()) { // If the course has no prerequisites
        cout << "Prerequisites: None\n";
        return;
    }

    cout << "Prerequisites: ";
	for (int i = 0; i < (int)c.prerequisites.size(); i++) { // Iterate through the prerequisites of the course
		cout << c.prerequisites[i]; // Print the prerequisite number
		if (i + 1 < (int)c.prerequisites.size()) { // If there are more prerequisites to print
            cout << ", ";
        }
    }
    cout << "\n";
}

//============================================================================
// Menu
//============================================================================
void PrintMenu() {
    cout << "1. Load Data Structure.\n";
    cout << "2. Print Course List.\n";
    cout << "3. Print Course.\n";
    cout << "9. Exit\n";
    cout << "What would you like to do? ";
}

//============================================================================
// Main
//============================================================================
int main() {
	vector<Course> courses; // Vector to hold the courses loaded from the file
	bool loaded = false; // Flag to indicate if courses have been loaded successfully

    cout << "Welcome to the course planner.\n";

	while (true) { // Print the main menu and prompt for user input
		PrintMenu(); // Print the menu options

        string line;
		if (!getline(cin, line)) { 
            cout << "Thank you for using the course planner!\n";
            break;
        }

        int choice = 0;
        {
			stringstream ss(line); // Create a stringstream from the input line
			ss >> choice; // Read the choice from the input
        }

        if (choice == 1) {
            string error;
            if (LoadCoursesFromFile(COURSE_DATA_FILE, courses, error)) {
                loaded = true;
            }
            else {
                loaded = false;
                cout << error << "\n";
            }

        }
        else if (choice == 2) {
            if (!loaded) {
                string error;
                if (LoadCoursesFromFile(COURSE_DATA_FILE, courses, error)) {
                    loaded = true;
                }
                else {
                    cout << error << "\n";
                }
            }
            if (loaded) {
                PrintCourseList(courses);
            }

        }
        else if (choice == 3) {
            if (!loaded) {
                string error;
                if (LoadCoursesFromFile(COURSE_DATA_FILE, courses, error)) {
                    loaded = true;
                }
                else {
                    cout << error << "\n";
                }
            }
            if (loaded) {
                cout << "What course do you want to know about? ";
                string key;
                if (getline(cin, key)) {
                    PrintCourseInfo_NumbersOnly(courses, key);
                }
            }

        }
        else if (choice == 9) {
            cout << "Thank you for using the course planner!\n";
            break;

        }
        else {
            cout << choice << " is not a valid option.\n";
        }
    }

    return 0;
}
