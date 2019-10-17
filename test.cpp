#include <iostream>
#include <bits/stdc++.h>
#define debug(x) cerr << #x << " " << x << endl
using namespace std;

// Contains the name of the ticket; the price of the ticket;
// time of validity.
using ticket = pair<string, pair<long long, int> >;

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
unsigned long long countTickets;

const int max_time = 1500;

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

// Adds a setOfTickets to allSetsOfTickets, and maybe delets some sets of tickets
// from it.
void addSetOfTickets(const setOfTickets &t) {
    if(t.second.size() > 3) return;

    int id_time = t.first.first;
    int id_ticket = t.second.size();

    if(allSetsOfTickets[id_time][id_ticket].first.first == -1) {
        allSetsOfTickets[id_time][id_ticket] = t;
    } else if(allSetsOfTickets[id_time][id_ticket].first.second > t.first.second) {
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

void addTicket(ticket &t) {
    schedule.push_back(t);
    int idOfTicket = schedule.size() - 1;

    for (int i = 0; i <= max_time; i++) {
        for(int j = 0; j < 3; j++) {
            if(allSetsOfTickets[i][j].first.first == -1) continue;
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

    for(int i = lengthOfTrip; i <= max_time; i++) {
        for(int j = 1; j <= 3; j++) {
            if(allSetsOfTickets[i][j].first.first == -1) continue;

            if(id_time == -1) {
                id_time = i;
                id_ticket = j;
            }
            if(allSetsOfTickets[i][j].first.second < allSetsOfTickets[id_time][id_ticket].first.second) {
                id_time = i;
                id_ticket = j;
            }
        }
    }

    if(id_time == -1) return ":-|";
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
    for(int i = 0; i <= max_time; i++) {
        for(int j = 0; j <= 3; j++) {
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
    route r1, r2, r4;
    r1.insert({"Banacha", 600});
    r1.insert({"Pasteura", 700});
    r1.insert({"Winnicka", 800});

    r2.insert({"Winnicka", 600});
    r2.insert({"Banacha", 700});
    r2.insert({"Pasteura", 800});

    r4.insert({"Winnicka", 800});
    r4.insert({"Banacha", 900});

    addRoute(1, r1);
    addRoute(2, r2);
    addRoute(4, r4);

    ticket t = {"Niedlugi", {230, 3}};
    addTicket(t);
    t = {"Dlugi Drogi", {380, 100}};
    addTicket(t);
    t = {"Dlugi Tani", {370, 100}};
    addTicket(t);

    vector<pair<string, long long> > q1, q2;

    q1.push_back({"Banacha", 1});
    q1.push_back({"Winnicka", -1});

    q2.push_back({"Banacha", 1});
    q2.push_back({"Winnicka", 4});
    q2.push_back({"Banacha", -1});
    cout << querySetOfTickets(q1) << endl;
    cout << querySetOfTickets(q2) << endl;

    debug(countTickets);
}

/*
1 6:00 Banacha 7:00 Pasteura 8:00 Winnicka
2 6:00 Winnicka 7:00 Banacha 8:00 Pasteura
3 6:00 Winnicka 5:00 Banacha 6:00
4 8:00 Winnicka 9:00 Banacha
Niedlugi 2.30 3
Dlugi Drogi 3.80 60
Dlugi Tani 3.70 60
Niedobry 4.333 39
? Banacha 1 Winnicka
? Banacha 1 Winnicka 4 Banacha


*/
