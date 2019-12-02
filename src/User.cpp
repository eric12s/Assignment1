//
// Created by ericsa on 21/11/2019.
//
#include "../include/User.h"
#include "../include/Session.h"
#include "../include/Watchable.h"
#include <vector>
#include <string>
#include <map>
using namespace std;
        //User
        User::User(const std::string& name): history(), name(name){}
        string User::getName() const{return name;}
        vector<Watchable*> User:: get_history() const{return history;}

        void User:: setHistory(vector<Watchable*>& other){
            history = other;
        }

        User::~User() {
            for(unsigned i = 0; i < history.size(); i++){
                history[i] = nullptr;
            }
        }

        User:: User(const User &other):history(), name(other.getName()){ //Copy Constructor
            for(unsigned i = 0; i < other.get_history().size(); i++){
                history.push_back(other.get_history()[i]);
            }
        }


        User &User::operator=(const User &other) { //Copy Assignment Operator
            if(this == &other)
                return *this;
            delete this;
            for(auto& cont : other.get_history()){
                history.push_back(cont);
            }
            return *this;
        }

        //LengthRecommenderUser
        LengthRecommenderUser :: LengthRecommenderUser(const std::string& name): User(name){}
        Watchable* LengthRecommenderUser:: getRecommendation(Session& s) {
            int sum = 0;
            vector<Watchable *> cont = s.get_Content();
            vector<Watchable *> hist = s.getUser()->get_history();
            for (unsigned i = 0; i < hist.size(); i++) {
                sum = sum + hist[i]->getLength();
            }
            float avg = sum / hist.size();

            unsigned loc = 0;
            for(unsigned j = 0; j < hist.size() && loc<cont.size(); j++)
                if(cont[loc] == hist[j]){
                    loc=loc+1;
                    j=0;
                }
            if(loc == cont.size())
                return nullptr;
            int y;
            int minimum= abs(cont[loc]->getLength()-avg);
            for (unsigned i = loc+1;  i < cont.size(); i++) {
                y= abs(cont[i]->getLength()-avg);
                bool b = true;
                for(unsigned j = 0; j < hist.size(); j++) {
                    if(hist[j] == cont[i])
                        b = false;
                }
                if(b) {
                    y = min(y, minimum);
                    if (y < minimum) {
                        loc = i;
                        minimum = y;
                    }
                }
            }
            return cont[loc];
        }


        LengthRecommenderUser::~LengthRecommenderUser() {}
        User* LengthRecommenderUser:: duplicate(string name){
            LengthRecommenderUser* clone = new LengthRecommenderUser(name);
            clone->setHistory(history);
            return clone;
        }
        LengthRecommenderUser* LengthRecommenderUser:: clone(){
            LengthRecommenderUser* clone = new LengthRecommenderUser(this->getName());
            for(auto& cont : this -> history){
                clone->get_history().push_back(cont);
            }
            return clone;
        }

        LengthRecommenderUser::LengthRecommenderUser(const LengthRecommenderUser &other): User(other.getName()) { //Copy Constructor
            for(auto& cont : other.get_history()){
                history.push_back(cont);
            }
        }
        LengthRecommenderUser &LengthRecommenderUser::operator=(const LengthRecommenderUser &other) { //Copy Assignment Operator
            if(this == &other)
                return *this;
            delete this;
            for(auto& cont : other.get_history()){
                history.push_back(cont);
            }
            return *this;
        }

        //RerunRecommenderUser
        RerunRecommenderUser:: RerunRecommenderUser(const std::string& name):User(name),i(-1){}
        Watchable* RerunRecommenderUser:: getRecommendation(Session& s) {
            if (history.size() == 0)
                return nullptr;
            i=i+1;
            int n = get_history().size();
            int ans = i% n;
            return history[ans];
        }
        RerunRecommenderUser::~RerunRecommenderUser() {}
        User* RerunRecommenderUser:: duplicate(string name){
            RerunRecommenderUser* clone = new RerunRecommenderUser(name);
            clone->setHistory(history);
            return clone;
        }

        User* RerunRecommenderUser:: clone(){
            RerunRecommenderUser* clone = new RerunRecommenderUser(this->getName());
            for(auto& cont : this -> history){
                clone->get_history().push_back(cont);
            }
            clone->setI(this->getI());
            return clone;
        }

        RerunRecommenderUser::RerunRecommenderUser(const RerunRecommenderUser &other): User(other.getName()), i(other.getI()) { //Copy Constructor
            for(auto& cont : other.get_history()){
                history.push_back(cont);
            }

        }

        RerunRecommenderUser &RerunRecommenderUser::operator=(const RerunRecommenderUser &other) { //Copy Assignment Operator
            if(this == &other)
                return *this;
            delete this;
            for(auto& cont : other.get_history()){
                history.push_back(cont);
            }
            i = other.getI();
            return *this;
        }

        int RerunRecommenderUser:: getI() const{return i;}
        void RerunRecommenderUser:: setI(int in){i = in;}

        //GenreRecommenderUser
        GenreRecommenderUser:: GenreRecommenderUser(const std::string& name):User(name){}
        Watchable* GenreRecommenderUser:: getRecommendation(Session& s){
            map <string,int> m;
            for(unsigned i=0; i< history.size(); i++){
                vector<string> tags = history[i]->getTagsByVec();
                for(unsigned j = 0; j < tags.size(); j++) {
                    if(m.find(tags[j]) == m.end()) {
                        m.insert(pair<string, int>(tags[j],1));
                    }
                    else{
                        m.find(tags[j])->second = m.find(tags[j])-> second + 1;
                    }
                }
            }
            while(!m.empty()){
                int max = 0;
                string st = "";
                for(map<string,int>::iterator iter = m.begin(); iter != m.end(); iter++){
                    int n = m.find(iter->first)->second;
                    if(max < n){
                        max = n;
                        st = m.find(iter->first)->first;
                    }
                }
                vector<Watchable*> vec = s.get_Content();
                Watchable* ans = nullptr;
                for(unsigned j = 0; j < vec.size(); j++){
                    for(unsigned t = 0; t < vec[j]->getTagsByVec().size() && ans == nullptr; t++){
                        if(st.compare(vec[j]->getTagsByVec()[t]) == 0) {
                            ans = vec[j];
                            bool found = false;
                            for (unsigned i = 0; i < history.size() && !found; i++) {
                                if (history[i] == ans)
                                    found = true;
                            }
                            if(!found)
                                return ans;
                            ans = nullptr;
                        }
                    }
                }
                m.erase(st);
            }
            return nullptr;
        }
        GenreRecommenderUser::~GenreRecommenderUser() {}
        User* GenreRecommenderUser:: duplicate(string name){
            GenreRecommenderUser* clone = new GenreRecommenderUser(name);
            clone->setHistory(history);
            return clone;
        }
        GenreRecommenderUser* GenreRecommenderUser:: clone(){
            GenreRecommenderUser* clone = new GenreRecommenderUser(this->getName());
            for(auto& cont : this -> history){
                clone->get_history().push_back(cont);
            }
            return clone;
        }

        GenreRecommenderUser::GenreRecommenderUser(const GenreRecommenderUser &other): User(other.getName()) { //Copy Constructor
            for(auto& cont : other.get_history()){
                history.push_back(cont);
            }
        }

        GenreRecommenderUser &GenreRecommenderUser::operator=(const GenreRecommenderUser &other) { //Copy Assignment Operator
            if(this == &other)
                return *this;
            delete this;
            for(auto& cont : other.get_history()){
                history.push_back(cont);
            }
            return *this;
        }


        void User::add_history(Watchable* cont){
            history.push_back(cont);
        }


