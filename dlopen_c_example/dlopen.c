#include <dlfcn.h>
#include <stdio.h>

int main(int argv, char ** argc)
{
    char ** library_name;
    void * i;
    void (* test)();

    if ( argv < 3 )
    {  printf("Usage: dlopen library_to_use.so function_to_call\n");  return -1;  }

    if ( ! ( i = dlopen(argc[1], RTLD_GLOBAL | RTLD_LAZY) ) )
    {  printf("Could not open library %s\n", argc[1]);  return -1;  }

    if ( ! ( library_name = dlsym(i, "libname") ) )
    {  printf("Could not find library name in library %s\n", argc[1], argc[2]);  return -1;  }

    printf("Using library: %s\n", *library_name);

    if ( ! ( test = dlsym(i, argc[2]) ) )
    {  printf("Could not find symbol %s in library %s\n", argc[1], argc[2]);  return -1;  }

    test();
    return 0;
}
