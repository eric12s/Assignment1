#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "Action.h"
#include "json.hpp"

class User;
class Watchable;

class Session{
public:
    Session(const std::string &configFilePath);
    ~Session();
    void start();
    Session(const Session&);
    Session& operator=(const Session&);
    Session(Session&&);
    Session &operator=(Session&&);
    User* getUser() const;
    std::string getSyntax() const;
    std::unordered_map<std::string,User*>& getUserMap();
    void setUser(std::string other);
    std::vector<Watchable*> get_Content() const;
    void setSyntax(std::string s);
    std::vector<BaseAction*>& getLog();
    void deleteContent();
    void deleteLog();
    void deleteUserMap();
private:
    std::vector<Watchable*> content;
    std::vector<BaseAction*> actionsLog;
    std::unordered_map<std::string,User*> userMap;
    std::string syntax;
    User* activeUser;
};
#endif
