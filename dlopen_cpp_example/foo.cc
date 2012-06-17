#include <iostream>
#include "plugin.h"

using namespace std;

const char * classname = "foo Object";

class foo : public plugin
{
    public:
        foo()  {  cout << "    foo created [foo.cc - foo::foo()]" << endl;  };
        ~foo() {  cout << "    foo destroyed [foo.cc - foo::~foo()]" << endl;  };
        virtual void test() {  cout << "    foo tested [foo.cc - foo::test()]" << endl;  };
};


class foofactory : factory
{
    public:
        foo * makedyn() {  cout << "    making foo [foo.cc foofactory::makedyn()]" << endl;  return new foo;  }
};

foofactory Factory;
