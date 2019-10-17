#include <iostream>
#include <bits/stdc++.h>
#include <string>

using namespace std;

// Contains the name of the ticket; the price of the ticket;
// time of validity.
using ticket = pair<string, pair<long long, int>>;

// Contains the name of the stop; time in which the bus will
// be on this stop, in minutes from the 00:00
using stop = pair<string, int>;

// Route: Set of the stops ordered by their times (**Unique stops).
// Each next one should have time bigger then previous.
using route = set<stop>;

// The first parameter is a pair <sum_of_times_of_tickets, price_of_tickets>.
// The second parameter is a vector of size up to three, containing the positions
// of chosen tickets is schedule.
using setOfTickets = pair<pair<int, long long>, vector<int> >;

/**
 * Schedule contains all possible tickets a customer can buy.
 */
vector<ticket> schedule;

// map of all added routs. the key is the ID of given route.
map<long long, route> allRouts;

setOfTickets allSetsOfTickets[2000][4];

// Contains the number of tickets that were print out during the program.
unsigned long long countTickets = 0;

const int max_time = 1500;

/******************Function Headers*******************/
void addRoute(long long idRoute, const route &r);

void addTicket(ticket &t);

string querySetOfTickets(vector<pair<string, long long>> &query);

/*****************************************************/


string createQuery(const string &line) {
    vector<string> results;
    istringstream iss(line);

    for (string s; iss >> s;)
        results.push_back(s);

    // results[0] is ?, so we omit that.
    vector<pair<string, long long>> query;

    // Do not iterate on the last string, which is the last stop in the query.
    for (int i = 1; i < (int) results.size() - 1; i += 2) {
        string stop = results[i];
        int stopNumber = stoi(results[i + 1], nullptr, 10);
        query.push_back(make_pair(stop, (long long) stopNumber));
    }

    // Add also the last stop with some value for route number which is unnecessary.
    query.push_back(make_pair(results[results.size() - 1], -27));

    return querySetOfTickets(query);
}

/**
 * Regex checks whether the request for a ticket is valid or no.
 */
const regex ticketRequestRegex("\\? [a-zA-Z_^]+( [0-9]+ [a-zA-Z_^]+)*");

string checkTicketRequest(const string &line) {
    if (regex_match(line, ticketRequestRegex)) {
        return createQuery(line);
    }

    return "";
}

void createTicket(const string &line) {
    // Get the name until first digit.

    string helperLine = line;
    string name;
    for (unsigned i = 0; i < line.length(); i++) {
        if (line[i] >= '0' && line[i] <= '9') {
            name = helperLine.substr(0, i - 1);
            helperLine.erase(0, i);
            break;
        }
    }

    vector<string> results;
    istringstream iss(helperLine);

    for (string s; iss >> s;)
        results.push_back(s);

    double cost = stod(results[0], nullptr);
    int validity = stoi(results[1], nullptr, 10);

    // Creates the ticket.
    ticket newTicket = make_pair(name, make_pair((long long) cost, validity));

    // Add the ticket.
    addTicket(newTicket);
}

bool checkCostInInput(const string &costStr) {
    return costStr != "0.00";
}

/**
 * Regex checks when new ticket is to be added in schedule.
 * Does not check the case when cost is "0.00".
 */
const regex scheduleRegex("[a-z A-Z]+ ([0-9]+.[0-9]{2}) [1-9][0-9]*");

/**
 * Checks the command and if it is valid adds ticket to the schedule.
 * @param line - Certain line taken from the input stream.
 * @return true if command is valid, otherwise false. Errors will be written in another function.
 */
bool checkTicketAddCommand(const string &line) {
    smatch match;
    if (regex_match(line, match, scheduleRegex)) {
        if (checkCostInInput(match.str(1))) {
            // Function parses the line and adds the ticket in schedule.
            createTicket(line);
            return true;
        }
    }

    return false;
}

int convertToMinutes(const string &time) {
    string delim = ":";
    string hour = time.substr(0, time.find(delim));
    string minutes = time.substr(time.find(delim) + 1);

    // Returns time in minutes from 0:00.
    return stoi(hour, nullptr, 10) * 60 + stoi(minutes, nullptr, 10);
}

bool createRoute(const string &line) {
    vector<string> results;
    istringstream iss(line);

    for (string s; iss >> s;)
        results.push_back(s);

    // Check if given id is free.
    long long id = stoi(results[0], nullptr, 10);
    // If map has route with id 'id' return false.
    if (allRouts.find(id) != allRouts.end())
        return false;

    route newRoute;
    // resultSize is odd, but we start from 1, so cycling will be even times.
    for (long long i = 1; i < (int) results.size(); i += 2) {
        int minutes = convertToMinutes(results[i]);
        stop stopTime = make_pair(results[i + 1], minutes);
        newRoute.insert(stopTime);
    }

    // Everything is fine add the route with given id in map.
    addRoute(id, newRoute);
    return true;
}

bool checkUniqueStops(const string &line) {
    // Check if route stops are unique.
    smatch match;
    const regex stopRegex("([a-zA-Z^_]+)");

    string::const_iterator searchStart(line.cbegin());
    vector<string> stopNames;

    while (regex_search(searchStart, line.cend(), match, stopRegex)) {
        if (find(stopNames.begin(), stopNames.end(), match[1]) != stopNames.end()) {
            // Stop can be reached maximum once in the route.
            return false;
        } else {
            // Add to vector to be checked later.
            stopNames.push_back(match[1]);
        }

        searchStart = match.suffix().first;
    }

    return true;
}

/**
 * Check in the line all time matches and check for time range.
 * @param line - proper line. Needs to be checked for time range.
 * @return true if time range is true for all tickets, otherwise false.
 */
bool checkTimeInInput(const string &line) {
    smatch match;
    const regex timeRegex("([5-9]|1[0-9]|2[0-1]):([0-5][0-9])");

    string::const_iterator searchStart(line.cbegin());
    int prevHours = 0;
    int prevMinutes = 0;

    while (regex_search(searchStart, line.cend(), match, timeRegex)) {
        int hours = stoi(match[1], nullptr, 10);
        int minutes = stoi(match[2], nullptr, 10);

        if ((hours < prevHours) || (hours == prevHours && minutes <= prevMinutes))
            return false;

        if (match[1] == "21")
            if (minutes > 21)
                return false;

        if (match[1] == "5")
            if (minutes < 55)
                return false;

        // To compare with the next time.
        prevHours = hours;
        prevMinutes = minutes;

        searchStart = match.suffix().first;
    }

    return true;
}

/**
 * Regex checks everything besides the minute range for 5:55 and 21:21.
 * Checks existence of leading 0 in first part(Hours) of the time.
 */
const regex routeRegex("[0-9]+( ([5-9]|1[0-9]|2[0-1]):([0-5][0-9]) ([a-zA-Z^_]+))+");

/**
 * Reads the line and parses the command to be executed,
 * if the line is matched with routeRegex.
 * @return true if command is valid, otherwise false. The errors will be written in another function.
 */
bool checkRouteAddCommand(const string &line) {
    if (!regex_match(line, routeRegex))
        return false;

    // Check time range and unique stop criteria are satisfied.
    if (!checkTimeInInput(line) || !checkUniqueStops(line))
        return false;

    // If gets here, means input is correct.
    // Parses the line and creates route with id and stops, then adds in set.
    return createRoute(line);
}

/*
ALGORITHMIC PART
*/

/*
TODO:
can query consist of only one stop?
*/

/*
INNER FUNCTIONS
*/

// Creates a new setOfTickets, by adding a new ticket to the existing
// setOfTickets
setOfTickets addTicketToSet(setOfTickets sot, const ticket &t, int idOfTicket) {
    sot.second.push_back(idOfTicket);
    sot.first.first += t.second.second;
    sot.first.second += t.second.first;
    sot.first.first = min(sot.first.first, max_time);
    return sot;
}

// Adds a setOfTickets to allSetsOfTickets, if it is better. setOfTickets A is better
// than setOfTickets of B if they have the same sum of times and number of tickets,
// and the cost of A is smaller then cost of B.
void addSetOfTickets(const setOfTickets &t) {
    if (t.second.size() > 3) return;

    int id_time = t.first.first;
    int id_ticket = t.second.size();

    if (allSetsOfTickets[id_time][id_ticket].first.first == -1) {
        allSetsOfTickets[id_time][id_ticket] = t;
    } else if (allSetsOfTickets[id_time][id_ticket].first.second > t.first.second) {
        allSetsOfTickets[id_time][id_ticket] = t;
    }
}

// Returns the time in which the bus with given id of route will
// be on given stop. The time is given in minutes from the 00:00.
int getTime(long long idOfRoute, const string &nameOfStop) {
    return (*allRouts[idOfRoute].lower_bound({nameOfStop, -1})).second;
}

/*
EXTERNAL FUNCTIONS
*/

void addRoute(long long idRoute, const route &r) {
    allRouts[idRoute] = r;
}

// Adds to the allSetsOfTickets all possible sets of tickets with given ticket
// that are better than previous. setOfTickets A is better than setOfTickets of B
// if they have the same sum of times and number of tickets, and the cost of A is
// smaller then cost of B.
void addTicket(ticket &t) {
    schedule.push_back(t);
    int idOfTicket = schedule.size() - 1;

    for (int i = 0; i <= max_time; i++) {
        for (int j = 0; j < 3; j++) {
            if (allSetsOfTickets[i][j].first.first == -1) continue;
            setOfTickets sot = addTicketToSet(allSetsOfTickets[i][j], t, idOfTicket);
            addSetOfTickets(sot);
        }
    }
}

// Returns the answer for given query. The answer is given in one of three formats:
// :-|
// :-( name_of_the_stop_on_which_we_wait
// ! name_of_ticket; name_of_ticket_2; ...; name_of_ticket_n
// Function assumes that the given query is correct.
// Query is a vector pair<string, long long> where string is the name of the stop
// and long long is the number of route which we take to get to this stop. The value
// of the last pair does not matter.
string querySetOfTickets(vector<pair<string, long long>> &query) {
    int n = query.size();
    assert(n > 1);

    for (int i = 0; i < n - 2; i++) {
        int endOfDrive = getTime(query[i].second, query[i + 1].first);
        int startOfDrive = getTime(query[i + 1].second, query[i + 1].first);

        if (endOfDrive != startOfDrive) {
            return ":-( " + query[i + 1].first;
        }
    }

    int start = getTime(query[0].second, query[0].first);
    int finish = getTime(query[n - 2].second, query[n - 1].first);
    int lengthOfTrip = finish - start + 1;

    int id_time = -1;
    int id_ticket = -1;

    for (int i = lengthOfTrip; i <= max_time; i++) {
        for (int j = 1; j <= 3; j++) {
            if (allSetsOfTickets[i][j].first.first == -1) continue;

            if (id_time == -1) {
                id_time = i;
                id_ticket = j;
            }
            if (allSetsOfTickets[i][j].first.second < allSetsOfTickets[id_time][id_ticket].first.second) {
                id_time = i;
                id_ticket = j;
            }
        }
    }

    if (id_time == -1) return ":-|";
    string answer = "!";

    for (auto x : allSetsOfTickets[id_time][id_ticket].second) {
        countTickets++;
        answer.push_back(' ');
        answer = answer + schedule[x].first;
        answer.push_back(';');
    }

    answer.pop_back();
    return answer;
}

void preprocAllSetsOfTickets() {
    for (int i = 0; i <= max_time; i++) {
        for (int j = 0; j <= 3; j++) {
            allSetsOfTickets[i][j] = {{-1, -1}, vector<int>()};
        }
    }

    allSetsOfTickets[0][0] = {{0, 0}, vector<int>()};
}

/*
END OF ALGORITHMIC PART
*/

int main() {
    preprocAllSetsOfTickets();
    string line;
    int lineNumber = 0;
    while (getline(cin, line)) {
        // Ignore empty lines
        if (!line.empty()) {
            // Do checking staff...if wrong print on the stderr.
            string query;
            if (!checkRouteAddCommand(line) && !checkTicketAddCommand(line)) {
                if (!(query = checkTicketRequest(line)).empty()) {
                    cout << query << endl;
                    lineNumber++;
                } else {
                    cerr << "Error in line " << ++lineNumber << ": " << line << endl;
                }
            } else {
                lineNumber++;
            }
        } else {
            lineNumber++;
        }
    }

    cout << countTickets << endl;
    /****** Assumption that below code will not be reached because of input. ******/
//    // Checking the input reading status.
//    if (cin.bad()) {
//        // IO error
//    } else if (!cin.eof()) {
//        // format error (not possible with getline)
//    }

    return 0;
}