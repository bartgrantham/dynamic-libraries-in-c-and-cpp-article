#include <iostream>
#include "plugin.h"

using namespace std;

const char * classname = "bar Object";

class bar : public plugin
{
    public:
        bar()  {  cout << "    bar created [bar.cc - bar::bar()]" << endl;  };
        ~bar() {  cout << "    bar destroyed [bar.cc - bar::~bar()]" << endl;  };
        virtual void test() {  cout << "    bar tested [bar.cc - bar::test()]" << endl;  };
};


class barfactory : public factory
{
    public:
        plugin * makedyn() {  cout << "    making bar [bar.cc barfactory::makedyn()]" << endl;  return new bar;  }
};

barfactory Factory;
