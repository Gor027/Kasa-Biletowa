#include <iostream>
#include <bits/stdc++.h>

using namespace std;

// Nazwa biletu, cena, długość działania.
using ticket = pair<string, pair<int, int>>;

// Nazwa przystanku, czas zatrzymania.
using stop = pair<string, int>;

// Route: Set of the stops ordered by their times (**Unique stops).
// Each next one should have time bigger then previous.
using route = set<stop>;

// długość działania, cena, vector indeksów ze schedule
using setOfTicets = pair<pair<int, int>, vector<int> >;

/**
 * Schedule contains all possible tickets a customer can buy.
 */
vector<ticket> schedule;

// map of all added routs. the key is the ID of given route.
map<int, route> allRouts;

// Contains all possible sets of tickets up to three tickets that are relevant.
// That is when we take all possible sets of tickets, some set of tickets is
// relevant if there are no other set of tickets that is cheaper and lasts for
// a longer period of time.
// The first parameter is a pair <price_of_tickets, sum of times of tickets>.
// The second parameter is a vector of size up to three, containing the positions
// of chosen tickets is schedule.
set<setOfTicets> allSetsOfTickets;

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


/*
ALGORITHMIC PART
*/

/*
TODO
swap first and second in  setOfTickets (?)
*/

/*
INNER FUNCTIONS
*/

// returns 0 if nothing erased, 1 if a erased and 2 if b erased
int deleteSetOfTickets(setOfTicets a, setOfTicets b) {
    if(a.first.first > b.first.first) {
        int ret = deleteSetOfTickets(b, a);
        if(ret == 2) return 1;
        if(ret == 1) return 2;
        return 0;
    }

    if(a.first == b.first) {
        if(a.second.size() < b.second.size()) {
            allSetsOfTickets.erase(b);
            return 2;
        } else {
            allSetsOfTickets.erase(a);
            return 1;
        }
    }

    if(a.first.second >= b.first.second) {
        allSetsOfTickets.erase(b);
        return 2;
    }
}

setOfTicets addTicketToSet(setOfTicets sot, ticket& t, int idOfTicket) {
    sot.second.push_back(idOfTicket);
    sot.first.first += t.second.first;
    sot.first.second += t.second.second;
    return sot;
}

void addSetOfTickets(setOfTicets t) {
    allSetsOfTickets.insert(t);
    auto cur = allSetsOfTickets.find(t);

    if(cur != allSetsOfTickets.begin()) {
        auto pr = prev(cur);

        if(deleteSetOfTickets((*pr), (*cur)) == 2) return;
    }

    while(true) {
        cur = allSetsOfTickets.find(t);
        auto ne = next(cur);

        if(ne == allSetsOfTickets.end()) break;

        if(deleteSetOfTickets((*ne), (*cur)) != 1) return;
    }
}

int getTime(int idOfRoute, string nameOfStop) {
    return (*allRouts[idOfRoute].lower_bound({nameOfStop, -1})).second;
}

/*
EXTERNAL FUNCTIONS
*/

void addRoute(int idRoute, route& r) {
    allRouts[idRoute] = r;
}

void addTicket(ticket& t) {
    schedule.push_back(t);
    int idOfTicket = schedule.size() - 1;
    vector<setOfTicets> tmpSetsOfTickets;
    tmpSetsOfTickets.push_back({{0, 0}, vector<int>()});

    for(auto tickets : allSetsOfTickets) {
        if(tickets.second.size() < 3) {
            tmpSetsOfTickets.push_back(addTicketToSet(tickets, t, idOfTicket));
        }
    }

    for(auto tickets : tmpSetsOfTickets) {
        addSetOfTickets(tickets);
    }
}

// zakładam że podane zapytanie jest poprawne, i jedyne co muszę sprawdzić
// to czekanie na przystanku oraz czy da się tam dojechać przy pomocy maksymalnie
// 3 biletów.
// query jest vectorem par<string, int>, gdzie string to nazwa przystanku a int
// to numer kursu. ostatnia para ma int = -1.
// funkcja zwracajedno z trzech:
// :-( nazwa_przystanku_gdzie_trzeba_czekać
// :-|
// ! nazwa_biletu; nazwa_biletu_2; ...; nazwa_biletu_n
string querySetOfTickets(vector <pair<string, int> > query) {
    // siup
    int n = query.size();

    for(int i = 0; i < n - 1; i++) {
        int endOfDrive = getTime(query[i].second, query[i].first);
        int startOfDrive = getTime(query[i].second, query[i + 1].first);
        if(endOfDrive != startOfDrive) {
            return ":-( " + query[i].first;
        }
    }

    // int start = (*allRouts[query[0].second].lower_bound({query[0].first, -1})).second;
    int start = getTime(query[0].second, query[0].first);
    int finish = getTime(query[n - 2].second, query[n - 1].first);
    int lengthOfJourney = finish - start + 1;

    // allSetsOfTickets.lower_bound()
    // wyszukiwanie na secie dodać
}

/*
END OF ALGORITHMIC PART
*/

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