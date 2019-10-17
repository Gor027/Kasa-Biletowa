#include <iostream>
#include <bits/stdc++.h>

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

// Contains all possible sets of tickets up to three tickets that are relevant.
// That is when we take all possible sets of tickets, some set of tickets is
// relevant if there are no other set of tickets that is cheaper and lasts for
// a longer period of time.
set<setOfTickets> allSetsOfTickets;

// Contains the number of tickets that were print out during the program.
unsigned long long countTickets;

/******************Function Headers*******************/
void addRoute(long long idRoute, const route &r);

void addTicket(ticket &t);

string querySetOfTickets(vector<pair<string, long long>> &query);

/*****************************************************/

/***************TODO***************/
/**
 * Regex for the third command - (DONE)
 * Check functions             - (DONE)
 * Add functions               - (DONE)
 */
/***********COMPLETED************/


void createQuery(const string &line) {
    vector<string> results;
    istringstream iss(line);

    for (string s; iss >> s;)
        results.push_back(s);

    // results[0] is ?, so we omit that.
    vector<pair<string, long long>> query;

    // Do not iterate on the last string, which is the last stop in the query.
    for (unsigned long i = 1; i < results.size() - 1; i += 2) {
        string stop = results[i];
        int stopNumber = stoi(results[i + 1], nullptr, 10);
        query.push_back(make_pair(stop, (long long) stopNumber));
    }

    // Add also the last stop with some value for route number which is unnecessary.
    query.push_back(make_pair(results[results.size() - 1], -27));

    querySetOfTickets(query);
}

/**
 * Regex checks whether the request for a ticket is valid or no.
 */
const regex ticketRequestRegex("\\? [a-zA-Z_^]+( [0-9]+ [a-zA-Z_^]+)*");

bool checkTicketRequest(const string &line) {
    if (regex_match(line, ticketRequestRegex)) {
        createQuery(line);
        return true;
    }

    return false;
}

bool is_number(const string &s) {
    string::const_iterator it = s.begin();
    while (it != s.end() && (isdigit(*it) || *it == '.')) ++it;

    return !s.empty() && it == s.end();
}

void createTicket(const string &line) {
    vector<string> results;
    istringstream iss(line);

    for (string s; iss >> s;)
        results.push_back(s);

    string name = results[0];
    int i = 1;
    while (!is_number(results[i])) {
        name.append(" ");
        name.append(results[i]);
        i++;
    }

    double cost = stod(results[i], nullptr);
    int validity = stoi(results[i + 1], nullptr, 10);

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
    if (allRouts.find(id) == allRouts.end())
        return false;

    route newRoute;
    // resultSize is odd, but we start from 1, so cycling will be even times.
    for (unsigned long i = 1; i < results.size(); i += 2) {
        int minutes = convertToMinutes(results[i + 1]);
        stop stopTime = make_pair(results[i], minutes);
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
    return !createRoute(line);
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

// returns 0 if nothing erased, 1 if a erased and 2 if b erased
int deleteSetOfTickets(const setOfTickets &a, const setOfTickets &b) {
    if (a.first.first > b.first.first) {
        int ret = deleteSetOfTickets(b, a);
        if (ret == 2) return 1;
        if (ret == 1) return 2;
        return 0;
    }

    if (a.first == b.first) {
        if (a.second.size() < b.second.size()) {
            allSetsOfTickets.erase(b);
            return 2;
        } else {
            allSetsOfTickets.erase(a);
            return 1;
        }
    }

    if (a.first.second >= b.first.second) {
        allSetsOfTickets.erase(a);
        return 1;
    }

    return 0;
}

// Creates a new setOfTickets, by adding a new ticket to the existing
// setOfTickets
setOfTickets addTicketToSet(setOfTickets sot, const ticket &t, int idOfTicket) {
    sot.second.push_back(idOfTicket);
    sot.first.first += t.second.second;
    sot.first.second += t.second.first;
    return sot;
}

// Adds a setOfTickets to allSetsOfTickets, and maybe delets some sets of tickets
// from it.
void addSetOfTickets(const setOfTickets &t) {
    allSetsOfTickets.insert(t);
    auto added = allSetsOfTickets.find(t);

    if (added != allSetsOfTickets.begin()) {
        auto pr = prev(added);
        if (deleteSetOfTickets((*pr), (*added)) == 2) return;
    }

    while (true) {
        added = allSetsOfTickets.find(t);
        auto ne = next(added);

        if (ne == allSetsOfTickets.end()) break;
        if (deleteSetOfTickets((*ne), (*added)) != 1) return;
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

void addTicket(ticket &t) {
    schedule.push_back(t);
    int idOfTicket = schedule.size() - 1;
    vector<setOfTickets> tmpSetsOfTickets;
    tmpSetsOfTickets.push_back({{0, 0}, vector<int>()});

    for (auto tickets : allSetsOfTickets) {
        if (tickets.second.size() < 3) {
            tmpSetsOfTickets.push_back(addTicketToSet(tickets, t, idOfTicket));
        }
    }

    for (auto tickets : tmpSetsOfTickets) {
        addSetOfTickets(tickets);
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

    for (int i = 0; i < n - 1; i++) {
        int endOfDrive = getTime(query[i].second, query[i].first);
        int startOfDrive = getTime(query[i].second, query[i + 1].first);

        if (endOfDrive != startOfDrive) {
            return ":-( " + query[i].first;
        }
    }

    int start = getTime(query[0].second, query[0].first);
    int finish = getTime(query[n - 2].second, query[n - 1].first);
    int lengthOfTrip = finish - start + 1;

    auto ans = allSetsOfTickets.lower_bound({{lengthOfTrip, -1}, vector<int>()});
    if (ans == allSetsOfTickets.end()) return ":-|";
    string answer = "!";

    for (auto x : (*ans).second) {
        countTickets++;
        answer.push_back(' ');
        answer = answer + schedule[x].first;
        answer.push_back(';');
    }
    answer.pop_back();
    return answer;
}

/*
END OF ALGORITHMIC PART
*/

int main() {
    string line;
    int lineNumber = 0;
    while (getline(cin, line)) {
        // Ignore empty lines
        if (!line.empty()) {
            // Do checking staff...if wrong print on the stderr.
            if (!checkRouteAddCommand(line) && !checkTicketAddCommand(line) && !checkTicketRequest(line)) {
                cerr << "Error in line " << ++lineNumber << ": " << line << endl;
            }
        } else {
            lineNumber++;
        }
    }

    /****** Assumption that below code will not be reached because of input. ******/
//    // Checking the input reading status.
//    if (cin.bad()) {
//        // IO error
//    } else if (!cin.eof()) {
//        // format error (not possible with getline)
//    }

    return 0;
}