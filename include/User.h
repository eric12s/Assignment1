#ifndef USER_H_
#define USER_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
class Watchable;
class Session;

class User{
public:
    User(const std::string& name);
    virtual Watchable* getRecommendation(Session& s) = 0;
    std::string getName() const;
    std::vector<Watchable*> get_history() const;
    virtual User* duplicate(std::string name)=0;
    void add_history(Watchable*);
    void setHistory(std::vector<Watchable*>& other);
    virtual ~User();
    virtual User* clone() = 0;
    //Copy Constructor
    User(const User &other);
    //Copy Assignment
    User& operator=(const User& other);

protected:
    std::vector<Watchable*> history;
private:
    std::string name;

};


class LengthRecommenderUser : public User {
public:
    LengthRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual User* duplicate(std::string name);
    virtual ~LengthRecommenderUser();
    LengthRecommenderUser* clone();
    LengthRecommenderUser(const LengthRecommenderUser& other);
    LengthRecommenderUser&operator=(const LengthRecommenderUser& other);
private:
};

class RerunRecommenderUser : public User {
public:
    RerunRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual User* duplicate(std::string name);
    virtual ~RerunRecommenderUser();
    User* clone();
    int getI()const;
    void setI(int in);
    RerunRecommenderUser(const RerunRecommenderUser& other);
    RerunRecommenderUser&operator=(const RerunRecommenderUser& other);
private:
    int i;
};

class GenreRecommenderUser : public User {
public:
    GenreRecommenderUser(const std::string& name);
    virtual Watchable* getRecommendation(Session& s);
    virtual User* duplicate(std::string name);
    virtual ~GenreRecommenderUser();
    GenreRecommenderUser* clone();
    GenreRecommenderUser(const GenreRecommenderUser& other);
    GenreRecommenderUser&operator=(const GenreRecommenderUser& other);
private:
};

#endif
