#include <iostream>
#include <dlfcn.h>
#include "plugin.h"

using namespace std;

factory * myFactory;

int main(int argv, char ** argc)
{
    char ** name;
    void * handle;
    factory * myFactory;
    plugin * myPlugin;

    if ( argv < 2 )
    {  cout << "Usage: dlopen plugin_to_use.so" << endl;  return -1;  }

    if ( ! ( handle = dlopen(argc[1], 0) ) )
    {  cout << "Could not open library " << argc[1] << " : " << dlerror() << endl;  return -1;  }

    if ( ! ( name = (char **)dlsym(handle, "classname") ) )
    {  cout << "Could not find classname in " << argc[1] << endl;  return -1;  }

    cout << "creating a \"" << *(name) << "\"..." << endl;

    if ( ! ( myFactory = (factory *)dlsym(handle, "Factory") ) )
    {  cout << "Could not find Factory in " << argc[1] << endl;  return -1;  }

    myPlugin = myFactory->makedyn();
    myPlugin->test();
    delete myPlugin;

    dlclose(handle);
    return 0;
}
