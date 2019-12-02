//
// Created by ericsa on 21/11/2019.
//
#include <fstream>
#include "../include/Session.h"
#include "../include/json.hpp"
#include "../include/Watchable.h"
#include "../include/User.h"
#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;
using json = nlohmann::json;



Session::Session(const std::string &configFilePath) : content(), actionsLog(), userMap(), syntax(""), activeUser(new LengthRecommenderUser("default")){
    userMap.insert(pair<string,User*>("default",  activeUser));
    fstream ifs(configFilePath);
    json buffer = json::parse(ifs);
    json movies = buffer["movies"];
    json tv_series = buffer["tv_series"];
    int counter = 1;
    for (auto &i : movies.items()) {
        vector<string> tags;
        json single = i.value();
        int l = single["length"];
        for (auto &j : single["tags"]) {
            tags.push_back(j);
        }
        string name = single["name"];
        Movie *add = new Movie(counter, name, l, tags);
        this->content.push_back(add);
        counter = counter + 1;
    }
    for (auto &i : tv_series.items()) {
        vector<string> tags;
        json single = i.value();
        for (auto &j : single["tags"]) {
            tags.push_back(j);
        }
        int l = single["episode_length"];
        string name = single["name"];
        int counter2 = 1; //Season
        for (auto &se : single["seasons"]) {
            for (int c = 1; c <= se; c++) {
                Episode *add = new Episode(counter, name, l, counter2, c, tags);
                this->content.push_back(add);
                counter = counter + 1;
                if(se != single["seasons"] || c != se)
                    add->setNextEpisodeID(counter);
            }
            counter2 = counter2 + 1;
        }
    }
}
Session::~Session(){
    deleteLog();
    deleteContent();
    deleteUserMap();
    activeUser = nullptr;
}

void Session::start(){
    cout <<"SPLFLIX is now on!"<<endl;
    string s;
    getline(cin, s);
    while(s != "exit") {
        int loc = s.find(" ");
        string act;
        if (loc != (int) string::npos) {
            act = s.substr(0, loc);
            syntax = s.substr(loc + 1);
        } else {
            act = s;
            syntax = "";
        }

        if (act == "createuser") {
            CreateUser *action = new CreateUser();
            action->act(*this);
            actionsLog.push_back(action);
        } else if (act == "changeuser") {
            ChangeActiveUser *action = new ChangeActiveUser();
            action->act(*this);
            actionsLog.push_back(action);
        } else if (act == "deleteuser") {
            DeleteUser *action = new DeleteUser();
            action->act(*this);
            actionsLog.push_back(action);
        } else if (act == "dupuser") {
            DuplicateUser *action = new DuplicateUser();
            action->act(*this);
            actionsLog.push_back(action);
        } else if (act == "content") {
            PrintContentList *action = new PrintContentList();
            action->act(*this);
            actionsLog.push_back(action);
        } else if (act == "watchhist"){
            PrintWatchHistory* action = new PrintWatchHistory();
            action->act(*this);
            actionsLog.push_back(action);
        }
        else if(act == "watch"){
            Watch* action = new Watch();
            action->act(*this);
            actionsLog.push_back(action);
        }
        else if(act == "log"){
            PrintActionsLog* action = new PrintActionsLog();
            action->act(*this);
            actionsLog.push_back(action);
        }
        else if(act == "exit"){
            Exit* action = new Exit();
            action->act(*this);
            actionsLog.push_back(action);
        }
        else
            cout << "Invalid action name!"<< endl;
        getline(cin,s);
    }
}

void Session:: setSyntax(string s){
    syntax = s;
}

vector<BaseAction*>& Session:: getLog(){return actionsLog;}

Session:: Session(const Session &other): content(), actionsLog(), userMap(), syntax(""),  activeUser(){
    for (unsigned i=0; i<other.content.size(); i++) //Copy content
        content.push_back(other.content[i]->clone());
    for (unsigned i=0; i<other.actionsLog.size(); i++)//Copy actionLog
        actionsLog.push_back(other.actionsLog[i]->clone());
    for(auto& i : other.userMap){//Copy userMap
        userMap.insert(pair<string,User*>(i.first, i.second->clone()));
        vector<Watchable*> hist;
        userMap.find(i.second->getName())->second->get_history().clear();
        for(auto& it : i.second->get_history()){
            for(auto& it2 : content){
                if(it2->getID() == it->getID())
                    hist.push_back(it2);
            }
        }
        userMap.find(i.second->getName())->second->setHistory(hist);
        hist.clear();
    }
    string s = other.activeUser->getName();
    activeUser = userMap.find(s)->second;
}

Session& Session::operator=(const Session& other) {
    cout << "TEST" << endl;
    if(this == &other)
        return *this;

    deleteUserMap();
    deleteLog();
    deleteContent();

    for (unsigned i=0; i<other.content.size(); i++) //Copy content
        content.push_back(other.content[i]->clone());
    for (unsigned i=0; i<other.actionsLog.size(); i++)//Copy actionLog
        actionsLog.push_back(other.actionsLog[i]->clone());
    for(auto& i : other.userMap){//Copy userMap
        userMap.insert(pair<string,User*>(i.first, i.second->clone()));
        vector<Watchable*> hist = userMap.find(i.second->getName())->second->get_history();
        for(auto& it : i.second->get_history()){
            for(auto& it2 : content){
                if(it2->getID() == it->getID())
                    hist.push_back(it2);
            }
        }
        userMap.find(i.second->getName())->second->setHistory(hist);
        hist.clear();
    }
    string s = other.activeUser->getName();
    activeUser = userMap.find(s)->second;
    return *this;
}


Session:: Session(Session &&other) : content(other.content), actionsLog(other.actionsLog), userMap(other.userMap), syntax(other.syntax), activeUser(other.activeUser){
    other.activeUser = nullptr;
    for(unsigned i = 0; i < other.content.size(); i++){
        other.content[i] = nullptr;
    }
    for(unsigned i = 0; i < other.actionsLog.size(); i++){
        other.actionsLog[i] = nullptr;
    }
    for(auto& i : other.userMap){
        ((string)i.first).clear();
        i.second = nullptr;
    }
    for(unordered_map<string,User*>::iterator i = other.userMap.begin(); i != other.userMap.end(); i++){
        other.userMap.erase(i);
    }
    other.userMap.clear();
}

Session &Session::operator=(Session&& other){
    activeUser = nullptr;
    deleteContent();
    deleteLog();
    deleteUserMap();

    activeUser = other.activeUser;
    content = other.content;
    actionsLog = other.actionsLog;
    userMap = other.userMap;

    for(unsigned i = 0; i < other.content.size(); i++){
        other.content[i] = nullptr;
    }
    for(unsigned i = 0; i < other.actionsLog.size(); i++){
        other.actionsLog[i] = nullptr;
    }
    for(auto& i : other.userMap){
        ((string)i.first).clear();
        i.second = nullptr;
    }
    for(unordered_map<string,User*>::iterator i = other.userMap.begin(); i != other.userMap.end(); i++){
        other.userMap.erase(i);
    }
    other.userMap.clear();
    return *this;
}



void Session::deleteContent() {
    for(unsigned i = 0; i < content.size(); i++){
        if(content[i] != nullptr)
           delete content[i];
        content[i] = nullptr;
    }
    content.clear();
}

void Session::deleteLog() {
    for(unsigned i = 0; i < actionsLog.size(); i++){
        if(actionsLog[i] != nullptr)
            delete actionsLog[i];
        actionsLog[i] = nullptr;
    }
    actionsLog.clear();
}

void Session::deleteUserMap(){
    for(unordered_map<string,User*>::iterator i = userMap.begin(); i != userMap.end(); i++){
        if(i->second != nullptr)
             delete (i->second);
        i->second = nullptr;
    }
    userMap.clear();
}

User* Session:: getUser()const{
    return activeUser;
}


string Session::getSyntax() const{
    return syntax;
}

void Session:: setUser(string other){
    activeUser = userMap.find(other)->second;
}

unordered_map<std::string,User*>& Session:: getUserMap(){
    return userMap;
}

std::vector<Watchable*> Session::get_Content() const {
    return content;
}
