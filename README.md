## Dynamic Libraries in C and C++ ##

  I first encountered software "plugins" with Photoshop 3.0 and Kai's Power 
Tools on Windows 3.1... and it blew my mind!  The idea that a piece of software 
could be architected so that you could add and remove extra *3rd party* modules 
and have them integrate so tightly was something of a revelation to me.  
Separate programs communicating was one thing, but this was a seamless 
extension to an existing piece of software.  Amazing.

  In programming this technique is known as a "dynamic library" and its 
implementation varies by operating system, largely splitting between Windows 
and Unix-variant lines.  This is because it involves a great deal of 
behind-the-scenes magic in order to open the library, sift through its symbol 
table to find the function(s) you are looking for, and then finally invoke the 
new function(s) without disrupting the operation of your program.  This process 
is known as 'linking' and it happens for most programs at run time as well, so 
it is natural that this is dependent on the operating system's understanding of 
libraries and how a running process is constructed.

  I'm going to show how this process works in C and C++, providing simple 
examples along the way.  Putting together sophisticated user interfaces that 
exploit these techniques is beyond the scope here, I want to just show the 
basics and leave the menus, etc. up to you.  My examples are for *nix systems 
and I'm going to assume proficiency in C and C++.


----

### Dynamic Libraries in C ###

  The conceptual problem is this: from inside of a running program, how do I 
find a function in a compiled file and then how do I execute it?

  The second part of that question is easily answered by the language itself.  
C and C++ provide a data type of "function pointer" that can be used 
syntactically in the same places that a function can, but can have an 
assignable value.  Declaring a function pointer looks like `void 
(*my_func_ptr)();` and you can call it just like any other function with 
`my_func_ptr();`.  This declares a function, `my_func_ptr`, that accepts no 
arguments and returns nothing.  If necessary, you can declare an argument list 
and return type, such as `int (*my_func_ptr)(char *, const int);` .  In this 
case, `my_func_ptr()` takes a pointer to char and an int and returns an int 
(for example, for counting the number of vowels in the first n characters in a 
string and returning that value).

   It's important to point out that C/C++ can only resolve this data typing at 
compile time.  Since type information is not baked into compiled objects, there 
is no way for the program to determine at run time if the function has the 
correct signature.  With dynamic libraries this it's entirely up to you to make 
sure that you're invoking the dynamically loaded functions with the correct 
arguments and working with their output properly.  This is why it's typical to 
put function signatures in `.h` files so that there is a published standard for 
interconnecting modules to conform to and the compiler can help sanity check 
the comings and goings of your function calls.

  So if we can get the address of a function in a compiled file, we can use 
assign it to a function pointer.  How do we get the address of a function in a 
compiled file, though?  In Unix systems this is provided by the functions 
`dlopen()` and `dlsym()` (there are also `dlclose()` and `dlerror()`, which 
I'll leave for you to read the man pages of).  `dlopen()` takes two arguments, 
a file path and a set of flags that indicate how the system should manage  the 
contents of the library, and it returns a handle to the library.  `dlsym()` 
takes the library handle that `dlopen()` provided and a string for a symbol 
(such as the name of a function), and returns... a function pointer! (or 
whatever datatype that symbol references)  Now we have the mechanisms to see 
how this works in code, disregarding error checking:

    #include <dlfcn.h>

    void * library_handle;
    void (* my_dyn_func)();
    library_handle = dlopen("mylibrary.so", 0);  // 0 == default flags == RTLD_GLOBAL | RTLD_LAZY
    my_dyn_func = dlsym(library_handle, "plugin_func_name");
    my_dyn_func();

   This isn't the whole story, though.  In order for `dlopen()` and `dlsym()` 
to be able to read and understand the library ("mylibrary.so" in the example), 
you'll need to compile it with special flags to indicate that you want the 
compiler to preserve the symbol table and format it so that the loader can read 
it properly.  With gcc this flag is "-shared", which stands for "shared 
object".  This is where the .so filename extension comes from as well.  *Note 
that this is not an "object" in the sense of object-oriented programming.*  
It's just a generic use of the term to indicate a binary file that contains a 
set of related functions and data.

So that's it.  Here's the bare minimum:

  * compile your function into a dynamic library (aka shared object) with "-shared"
  * open the library with `dlopen()` and find the function with `dlsym()`
  * assign the value to a function pointer

There is a very simple example in the `dlopen_c_example` directory.  It 
includes two library files (testlib1.c, testlib2.c), an application that can 
use either library based on its command line arguments (dlopen.c), and a 
Makefile.  You can compile with:

    # make

And try it out with:

    # ./dlopen testlib1.so test

  You can use "testlib1.so" or "testlib2.so" for the first argument or "testa" 
or "testb" for the second.  This illustrates not just dynamic loading of 
different code at run time, but also running of different functions.  I've also 
included finding a string that is the name of the library for the user's 
benefit.  Try out all the combinations at once with:

    # make test


----

### Dynamic Libraries in C++ ###

  For C we simply wanted to execute a dynamically loaded function.  Naturally 
in C++ we want to work with objects, ideally instantiating an object from a 
dynamically loaded class.

  Now that we've covered how the mechanisms work in C, certainly C++ has a more 
sophisticated OO paradigm for dynamically loading code, right?  Unfortunately, 
that's not the case.  We still use `dlopen()` and `dlsym()`, but because of the 
way C++ mangles method names it is difficult or impossible to acquire the 
address of an object's constructor on the fly.  This makes creating objects 
from dynamic libraries much more difficult.

<!---
TODO: Exposition on how it would be nice to have it work, including a reference 
to RTTI
-->

  One technique for managing this is to have a _statically defined_ object in 
the dynamic library that acts as a factory for creating the object we have in 
mind.  If we create a standard interface for this factory and a generic 
superclass for the object we want to create, we can subclass both while while 
still allowing the calling program to find the object and create the dynamic 
object.  It sounds complicated... and it is!  If we examine the technique one 
step at a time while looking at the necessity of each mechanism it should make 
the process clear.

<!---
TODO: Sidebar on how to use a less sophisticated version with extern C and the 
use of new
-->

  We start by understanding the need and design of our factory class and why an 
object of this factory class needs to be instantiated statically in the 
compiled library, instead of dynamically at runtime in the code as you may be 
more familiar with.

  In order to handle multiple methods with the same name but differing 
parameter types, as well as namespaces, C++ compilers will perform "name 
mangling" on the symbols that are stored in compiled object code.  This name 
mangling is not standardized, and it can even vary between different versions 
of the same compiler!  (There is an excellent example on Wikipedia in the 
[article on name mangling]).  Because of this name mangling it is impossible to 
reach inside a shared object and pull out the address of an class constructor, 
even if you know the name of the class.  For example, the constructor for a 
"Foo" object has the name "__ZN3fooC1Ev" in the symbol table on my laptop (GCC 
4.2.1 on MacOS 10.6).

  [article on name mangling]: http://en.wikipedia.org/wiki/Name_mangling#Complex_example

  So if all method names including constructors are unknown, how can we make 
use of a C++ dynamic library?  One solution, and the one I'll demonstrate, is 
to have a _statically allocated_ factory object in the library that inherits 
from a common factory class.  This common factory class contains a virtual 
method that we override with our own method that we'll call at run-time to 
create the dynamic object we ultimately want.  What this means in code is that 
we declare an instance object of this factory class in file scope (outside of 
any methods/functions) so we are declaring it as `static`.  This has two 
results: the dynamic library will be arranged so when `dlopen()` opens the 
library the constructor of this factory object is called, and most importantly 
_that the name of the instantiated factory object is not mangled_.  This gives 
us a known anchor into the dynamic library for code that we can use to do 
more... such as creating other objects!

  The use of a factory may seem a roundabout technique for being able to 
dynamically load in a new object, but as we walk through the example you may be 
inspired and see ways in which this technique provides a great deal of 
flexibility for managing dynamic objects of unknown class.  For the example, 
the objects that we want to create dynamically at runtime are descended from a 
base class, `plugin`.  The interface to `plugin` is defined as:

    class plugin
    {
        public:
            // default constructor
            virtual void test() { };
            virtual ~plugin() {};
    };

  Pretty straightforward.  We rely on the compiler to create the default 
constructor, and `test()` will be the function that we override in our plugins 
that performs expanded functionality.  The destructor is marked `virtual` 
because we expect full-featured plugins will have their own unloading 
requirements (removing menu entries, closing files or sockets, etc.)  However, 
for the example the plugins will have very simple destructors.

  Now we can look at the `factory` class.  Remember that the purpose of the 
factory is to have a known function entry point in the library that we can find 
and use to create our objects.  The factory can be made as complex as needed, 
but at its simplest it only provides a virtual function that returns an 
instance of "plugin", so its interface consists of a single virtual method:

    class factory
    {
        public:
            // default constructor
            virtual plugin * makedyn() { };
            // default destructor
    };

  We can now examine the code for a dynamic library that uses and extends these 
base classes.  In it are defined two classes: the plugin class that our program 
will use and a factory that knows how to create this plugin. Also included in 
the file is a string that provides a name for the plugin and the statically 
declared instance of the plugin factory that we'll use to create a new instance 
of the plugin.  Here's the code for the `foo` plugin:

    const char * classname = "foo Object";  // name for this plugin, useful for user feedback

    // the actual plugin class
    class foo : public plugin
    {
        public:
            foo()  {  cout << "    foo created [foo.cc - foo::foo()]" << endl;  };
            ~foo() {  cout << "    foo destroyed [foo.cc - foo::~foo()]" << endl;  };
            virtual void test() {  cout << "    foo tested [foo.cc - foo::test()]" << endl;  };
    };


    // the class of the plugin factory
    class foofactory : factory
    {
        public:
            foo * makedyn() {  cout << "    making foo [foo.cc foofactory::makedyn()]" << endl;  return new foo;  }
    };


    // a statically declared instance of our derived factory class
    foofactory Factory;


  Observe how the `foofactory` class overrides the `makedyn()` function to 
create and return a pointer to an object of class `foo`.  Also note the 
`foofactory` instance named `Factory`, declared at the bottom of the code 
snippet.  Finally, see that the foo class overrides the constructor, destructor 
and `test()` methods of the `plugin` parent class.

  Finally, let's examine how a program can make use of this plugin.  In the C 
example we declared a function pointer and then assigned to it the value of a 
function address once we had looked it up in the dynamic library, like this:

    void (* my_dyn_func)();
    library_handle = dlopen("mylibrary.so", 0);
    my_dyn_func = dlsym(library_handle, "plugin_func_name");
    my_dyn_func();

  Since this is C++ and we're working with objects, instead of a function 
pointer we'll use pointers to objects to hold both the dynamic library's "entry 
point" as well as the plugin that contains the dynamic code we're loading.  Now 
we can reveal the actual `dlopen()`, `dlsym()`, and factory method that loads 
our dynamic plugin class:

    #include <dlfcn.h>

    void * library_handle;
    factory * myFactory;
    plugin * myPlugin;

    library_handle = dlopen("mylibrary.so", 0);

    myFactory = (factory *)dlsym(handle, "Factory");  // find the statically declared factory
    myPlugin = myFactory->makedyn();                  // create our dynamic plugin
    myPlugin->test();                                 // call our plugin-specific code

  The critical point in this code is the string "Factory" that `dlsym()` uses 
to find the statically defined factory class.  Like "plugin_func_name" in the C 
example, there must be a variable with a known symbol name that `dlsym()` can 
find.  Once the address of this symbol is found and cast to a `factory` class 
we can use its overriden virtual method `makedyn()` to create a plugin object.  
Then we can call the overriden virtual method `test()` of the plugin object to 
call plugin-specific code.

  And, like the C example, the plugin must be compiled with the "-shared" flag 
so that the object file includes the symbol table that allows it to be parsed 
and then found by the `dlopen()` and `dlsym()` functions.  On most operating 
systems (but not MacOSX) an additional flag may need to be passed as well, 
"-fPIC".  PIC stands for "position independent code" and it means that the 
resulting object code is structured so that function calls aren't hardwired to 
specific locations in memory, a requirement for code that may be loaded 
anywhere and in any order at run-time.  A thorough explanation of PIC is 
outside the scope of this article but generally this means that the resulting 
assembly code uses relative addressing in function calls, rather than absolute 
addresses. Wikipedia has a [good article on PIC].

  [good article on PIC]: http://en.wikipedia.org/wiki/Position-independent_code

  That wraps up the C++ dynamic code example.  Here's the bare minimum for this 
technique:

  * two base classes: the plugin itself with virtual methods to be overridden 
in derived classes for custom functionality, and a factory with a single 
virtual method to be overridden by a plugin's corresponding derived factory 
class
  * plugin code that extends these two base classes, with a statically declared 
instance of the derived factory in the plugin library
  * compile your plugin code into a dynamic library (aka shared object) with 
"-shared" and "-fPIC"
  * open the library with dlopen and find the factory instance with dlsym
  * use the factory's virtual method to create a new plugin object

There is an example in the `dlopen_cpp_example` directory.  It includes three 
plugins (foo.cc, bar.cc, and baz.cc), an application that can dynamically load 
a library based on command line arguments (dlopen.cc), headers, and a Makefile.  
You can compile with:

    # make

And try it out with

    # ./dlopen foo.so

  You can substitute foo.so with bar.so or baz.so and the code will load up the 
bar and baz plugins appropriately.  Try all the combinations at once with:

    # make test

Have fun!  And if you have any questions, please feel free to comment below or 
[drop me a line].

  [drop me a line]: http://www.bartgrantham.com/contact/
