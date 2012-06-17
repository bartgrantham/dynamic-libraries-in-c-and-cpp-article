using namespace std;

class plugin
{
    public:
        // default constructor
        virtual void test() { };
        virtual ~plugin() {};
};

class factory
{
    public:
        virtual plugin * makedyn() { };
};

