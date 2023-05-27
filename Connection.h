#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
using namespace std;

class Connection {
public:
    Connection(){}
    Connection(string n1, string n2, int i1, int i2, int pN):
        module_name1(n1),
        module_name2(n2),
        index_name1 (i1),
        index_name2 (i2),
        pin_Number  (pN){}
    ~Connection(){}

    string      module_name1;
    string      module_name2;
    int         index_name1;
    int         index_name2;
    int         pin_Number;

};


#endif