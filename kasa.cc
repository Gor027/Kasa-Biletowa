#include <iostream>
#include <bits/stdc++.h>

using namespace std;

// Nazwa biletu, cena, długość działąnia.
using ticket = pair<string, pair<int, int>>;

// Nazwa przystanku, czas zatrzymania.
using stop = pair<string, int>;

// Route: Set of the stops ordered by their times (**Unique stops).
// Each next one should have time bigger then previous.
using route = set<stop>;

/**
 * Schedule contains all possible tickets a customer can buy.
 */
using schedule = vector<ticket>;

/***************TODO***************/
/**
 * Regex for the third command
 * Check functions
 * Add functions
 */
/**********************************/

/**
 * Regex checks when new ticket is to be added in schedule.
 */
const regex scheduleRegex("[a-z A-Z]* [0-9]+.[0-9]{2} [1-9][0-9]*");

bool checkTicketAddCommand(const string &line) {
    if (regex_match(line, scheduleRegex)) {
        if (checkCostInInput(line)) {
            // Function parses the line and adds the ticket in schedule.
            addTicketToSchedule(line);
            return true;
        }
    }

    return false;
}

/**
 * Regex checks everything besides the first two digits of time.
 * It does not check when numbers until ':' are more than 2. E.g. 555:12
 * Also does not check time between 5:55 and 21:21.
 * Checks existence of leading 0 in first part(Hours) of the time.
 */
const regex routeRegex("[0-9]+( [1-9]{1}[0-9]*:[0-9]{2} [a-zA-Z^_]+)+");

/**
 * Reads the line and parses the command to be executed,
 * if the line is matched with routeRegex.
 * @return true if command is valid, otherwise false. The errors will be written in another function.
 */
bool checkRouteAddCommand(const string &line) {
    if (regex_match(line, routeRegex)) {
        if (checkTimeInInput(line)) {
            addRoute(line); // Parses the line and creates route with id and stops then adds in set.
            return true;
        }
    }

    return false;
}

int main() {
    string line;
    int lineNumber = 1;
    while (getline(cin, line)) {
        // Ignore empty lines
        if (line.length() != 0) {
            // Do checking staff...if wrong print on the stderr.
        }
    }

    // Checking the input reading status.
    if (cin.bad()) {
        // IO error
    } else if (!cin.eof()) {
        // format error (not possible with getline)
    } else {
        // format error (not possible with getline)
        // or end of file
    }

    return 0;
}