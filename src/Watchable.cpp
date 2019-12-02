#include "../include/Watchable.h"
//
// Created by ericsa on 21/11/2019.
//
#include "../include/Session.h"
#include "../include/User.h"
#include <string>

using namespace std;
Watchable::Watchable(long id, int length, const std::vector<std::string>& tags) : id(id),length(length),tags(tags){}
Watchable:: ~Watchable(){tags.clear();}
int Watchable::getLength() const{ return length;}
string Watchable::getTags() const{
    string tags2;
    auto it = tags.begin();
    tags2 = "[" + *it;
    it++;
    for(; it < tags.end(); it++){
        tags2 = tags2 + "," + *it;
    }
    tags2 = tags2 + "]";
    return tags2;
}

Watchable::Watchable(const Watchable& other): id(other.id),length(other.length),tags(other.tags){}
long Watchable::getID() const{return id;}
vector<string>& Watchable::getTagsByVec() {return tags;}



Movie:: Movie(long id, const std::string& name, int length, const std::vector<std::string>& tags) : Watchable(id, length, tags), name(name){};
string Movie:: toString() const{return name;}
Watchable* Movie:: getNextWatchable(Session& sess) const{
    User* user = sess.getUser();
    Watchable* w = user->getRecommendation(sess);
    return w;
}


Movie* Movie::clone() {
    Movie* clone = new Movie(this->getID(),this->toString(),this->getLength(),this ->getTagsByVec());
    return clone;
}

void Movie:: setName(string other){
    name = other;
}


Episode::Episode(long id, const std::string& seriesName,int length, int season, int episode ,const std::vector<std::string>& tags):
                Watchable(id, length, tags), seriesName(seriesName), season(season), episode(episode), nextEpisodeId(-1){};

string Episode::toString() const{return seriesName + " S" + to_string(season) + "E" + to_string(episode);}
Watchable* Episode::getNextWatchable(Session& sess) const{
    if(nextEpisodeId != -1){
        vector<Watchable*> cont = sess.get_Content();
        for(unsigned i = 0; i < cont.size(); i++)
            if(cont[i]->getID() == nextEpisodeId)
                return cont[i];
    }
    return sess.getUser()->getRecommendation(sess);
}

void Episode:: setNextEpisodeID(int i){nextEpisodeId = i;}

Episode* Episode::clone() {
    Episode* clone = new Episode(this->getID(), this->toString(), this->getLength(),this->getSeason(), this->getEpisode(),this ->getTagsByVec());
    return clone;
}
int Episode:: getEpisode()
{
    return episode;
}
int Episode::getSeason() {
    return season;
}