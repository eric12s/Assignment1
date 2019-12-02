//
// Created by ericsa on 21/11/2019.
//
#include <vector>
#include <iostream>
#include <string>
#include "../include/Action.h"
#include "../include/Session.h"
#include "../include/User.h"
#include "../include/Watchable.h"
using namespace std;

BaseAction::BaseAction(): errorMsg(""), status(PENDING){};

ActionStatus BaseAction::getStatus() const {return status;}

BaseAction::~BaseAction() = default;
string BaseAction:: getStatus2() const{
    ActionStatus s = getStatus();
    if(s == PENDING)
        return "PENDING";
    else if(status == ERROR)
        return "ERROR: " + getErrorMsg();
    else
        return "COMPLETED";
};

void BaseAction::complete() {status = COMPLETED; cout<<"COMPLETED"<<endl;}

void BaseAction::error(const std::string& errorMsg){this->errorMsg = errorMsg; status = ERROR; cout << "ERROR -" + getErrorMsg() << endl;}

string BaseAction::getErrorMsg() const {return errorMsg;}
void BaseAction::setErrorMsg(string s){errorMsg = s;}
void BaseAction::setStatus(ActionStatus s){status = s;}

void CreateUser:: act(Session& sess){
    string s = sess.getSyntax();
    unsigned loc = s.find(" ");
    string name;
    string algo;
    User* user;
    name = s.substr(0, loc);
    algo = s.substr(loc + 1);
     if(loc > 0 && loc != string:: npos && sess.getUserMap().count(name)  == 0){
        if(algo == "len"){
            user = new LengthRecommenderUser(name);
            sess.getUserMap().insert(pair<string,User*>(name,  user));
            complete();
        }
        else if(algo == "rer"){
            user = new RerunRecommenderUser(name);
            sess.getUserMap().insert(pair<string,User*>(name,  user));
            complete();
        }
        else if(algo == "gen"){
            user = new GenreRecommenderUser(name);
            sess.getUserMap().insert(pair<string,User*>(name,  user));
            complete();
        }
        else
            error("Not valid algorithm");
        }
     else if(sess.getUserMap().count(name)  > 0){
         error("Choose different name");
     }
     else {
         error("Not valid input");
     }
}
string CreateUser:: toString() const{
    return "Create user "+ getStatus2();
}


CreateUser* CreateUser::clone() {
    CreateUser* clone = new CreateUser();
    clone->setErrorMsg(this->getErrorMsg());
    clone->setStatus(this->getStatus());
    return clone;
}

void ChangeActiveUser:: act(Session& sess){
    unordered_map<string,User*>& map = sess.getUserMap();
    if(map.count(sess.getSyntax()) != 0){
        sess.setUser(sess.getSyntax());
        complete();
    }
    else{
        error("The user isn't exist");
    }

}
string ChangeActiveUser:: toString() const{
    return "Active user "+ getStatus2();
}

ChangeActiveUser* ChangeActiveUser::clone() {
    ChangeActiveUser* clone = new ChangeActiveUser();
    clone->setErrorMsg(this->getErrorMsg());
    clone->setStatus(this->getStatus());
    return clone;
}
void DeleteUser:: act(Session & sess){
    unordered_map<string,User*>& map = sess.getUserMap();
    if(map.count(sess.getSyntax()) != 0){
        unordered_map<string,User*>::iterator iter = map.find(sess.getSyntax());
        delete iter->second;
        map.erase(iter);
        complete();
    }
    else{
        error("The user isn't exist");
    }
}

string DeleteUser:: toString() const{
    return "Delete user "+ getStatus2();
}

DeleteUser* DeleteUser::clone() {
    DeleteUser* clone = new DeleteUser();
    clone->setErrorMsg(this->getErrorMsg());
    clone->setStatus(this->getStatus());
    return clone;
}
void DuplicateUser:: act(Session & sess) {
    unordered_map<string, User *> &map = sess.getUserMap();
    string s = sess.getSyntax();
    unsigned loc = s.find(" ");
    string oldName = s.substr(0, loc);
    string newName = s.substr(loc + 1);
    User *user;
    if (loc > 0 && loc < s.length() - 1) {
        if (map.count(oldName) != 0 ) {
            if(map.count(newName) == 0) {
                user = map.find(oldName)->second->duplicate(newName);
                map.insert(pair<string, User*>(newName, user));
                complete();
                user = nullptr;
            }
            else{
                error("The user name is already taken");
            }
        } else
            error("The user isn't exist");
    }
    else
        error("Not valid input!");
}

string DuplicateUser:: toString() const{
    return "Duplicate User "+ getStatus2();
}

DuplicateUser* DuplicateUser::clone() {
    DuplicateUser* clone = new DuplicateUser();
    clone->setErrorMsg(this->getErrorMsg());
    clone->setStatus(this->getStatus());
    return clone;
}

void PrintContentList:: act(Session& sess){
    vector<Watchable*> cont = sess.get_Content();
    for(unsigned i=0; i<cont.size();i++){
        cout << to_string(i+1) +". " + cont[i]->toString() + " " + to_string(cont[i]->getLength()) + " minutes " + cont[i]->getTags() << endl;
    }
    complete();
}


string PrintContentList::toString() const {
    return "Print content list " + getStatus2();
}

PrintContentList* PrintContentList::clone() {
    PrintContentList* clone = new PrintContentList();
    clone->setErrorMsg(this->getErrorMsg());
    clone->setStatus(this->getStatus());
    return clone;
}

void PrintWatchHistory:: act(Session& sess){
    vector<Watchable*> hist = sess.getUser()->get_history();
    cout << "Watch history for " + sess.getUser()->getName() << endl;
    for(unsigned i=0;i< hist.size();i++){
      cout << to_string(i+1) + ". " + hist[i]->toString() << endl;
    }
    complete();
}
string PrintWatchHistory::toString() const {
    return "Watch history " + getStatus2();
}

PrintWatchHistory* PrintWatchHistory::clone() {
    PrintWatchHistory* clone = new PrintWatchHistory();
    clone->setErrorMsg(this->getErrorMsg());
    clone->setStatus(this->getStatus());
    return clone;
}

bool is_number(const std::string &s) {//Check if Syntax is legal id number
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

void Watch:: act(Session& sess){
    if(is_number(sess.getSyntax())) {
        unsigned id = stoi(sess.getSyntax());
        vector<Watchable *> cont = sess.get_Content();
        if(id <= cont.size() && id > 0){
            cout << "Watching " + cont[id - 1]->toString() << endl;
            sess.getUser()->add_history(cont[id - 1]);
            Watchable *rec = cont[id - 1]->getNextWatchable(sess);
            complete();
            string ans = "";
            if (rec != nullptr) {
                cout << "We recommend watching " + rec->toString() + ", continue watching? [y/n]" << endl;
                while (ans != "y" && ans != "n") {
                    getline(cin, ans);
                    if (ans == "y") {
                        int id2 = rec->getID();
                        sess.setSyntax(to_string(id2));
                        sess.getLog().push_back(this->clone());
                        act(sess);
                    } else if (ans != "n" && ans != "y")
                        cout << "Choose only y or n!" << endl;
            }
        } else
            error("No recommended content available");
    }else{
            error("Not legal id!");
        }
    }
    else{
        error("Not valid input!");
    }
}

string Watch::toString() const {
    return "Watch " + getStatus2();
}

Watch* Watch::clone() {
    Watch* clone = new Watch();
    clone->setErrorMsg(this->getErrorMsg());
    clone->setStatus(this->getStatus());
    return clone;
}

void PrintActionsLog:: act(Session& sess){
    vector<BaseAction*>& log = sess.getLog();
    for(unsigned i = log.size() - 1; i > 0; i--){
        cout <<log[i]->toString()<< endl;
    }
    cout <<log[0]->toString()<< endl;
    complete();
}
string PrintActionsLog::toString() const {
    return "Print action log " + getStatus2();
}

PrintActionsLog* PrintActionsLog::clone() {
    PrintActionsLog* clone = new PrintActionsLog();
    clone->setErrorMsg(this->getErrorMsg());
    clone->setStatus(this->getStatus());
    return clone;
}

void Exit:: act(Session& sess){
    complete();
}
string Exit::toString() const {
    return "Exit " + getStatus2();
}

Exit* Exit::clone() {
    Exit* clone = new Exit();
    clone->setErrorMsg(this->getErrorMsg());
    clone->setStatus(this->getStatus());
    return clone;
}