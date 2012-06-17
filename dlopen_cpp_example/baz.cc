#include <iostream>
#include "plugin.h"

using namespace std;

const char * classname = "baz Object";

class baz : public plugin
{
    public:
        baz()  {  cout << "    baz created [baz.cc - baz::baz()]" << endl;  };
        ~baz() {  cout << "    baz destroyed [baz.cc - baz::~baz()]" << endl;  };
        virtual void test() {  cout << "    baz tested [baz.cc - baz::test()]" << endl;  };
};


class bazfactory : public factory
{
    public:
        plugin * makedyn() {  cout << "    making baz [baz.cc bazfactory::makedyn()]" << endl;  return new baz;  }
};

bazfactory Factory;
