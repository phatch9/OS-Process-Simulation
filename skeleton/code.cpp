#include <cctype> // for toupper()
#include <cstdlib> // for EXIT_SUCCESS and EXIT_FAILURE
#include <cstring> // for strerror()
#include <cerrno> // for errno
#include <deque> // for deque (used for ready and blocked queues)
#include <fstream> // for ifstream (used for reading simulated programs)
#include <iostream> // for cout, endl, and cin
#include <sstream> // for stringstream (used for parsing simulated programs)
#include <sys/wait.h> // for wait()
#include <unistd.h> // for pipe(), read(), write(), close(), fork(), and _exit()
#include <vector> // for vector (used for PCB table)

using namespace std;
    class Instruction {
    public:
        char operation;
        int intArg;
        string stringArg;
    };
class Cpu {
    public:
        vector<Instruction> *pProgram;
        int programCounter;
        int value;
        int timeSlice;
        int timeSliceUsed;
};
   