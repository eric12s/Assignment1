#include <iostream>
#include "../include/Session.h"
using namespace std;
Session bar(char*  filepath);

int main(int argc, char** argv){
    if(argc!=2)
    {
        cout << "usage splflix input_file" << endl;
        return 0;
    }
    Session s(argv[1]);
    Session s2 (bar(argv[1]));
    s2.start();
//    Session *s1(new Session(argv[1]));
    s.start();
    return 0;
}

Session bar(char* filepath) {
    Session s(filepath);
    s.start();
    return s;
}
