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
    enum State {
    STATE_READY,
    STATE_RUNNING,
    STATE_BLOCKED
    };
    class PcbEntry {
        public:
            int processId;
            int parentProcessId;
            vector<Instruction> program;
            unsigned int programCounter;
            int value;
            unsigned int priority;
            State state;
            unsigned int startTime;
            unsigned int timeUsed;
            };
        PcbEntry pcbEntry[10];
        unsigned int timestamp = 0;
        Cpu cpu;
    // For the states below, -1 indicates empty (since it is an invalid index).
        int runningState = -1;
        deque<int> readyState;
        deque<int> blockedState;
    /* In this implementation, we'll never explicitly clear PCB entries and the index in
    the table will always be the process ID. These choices waste memory, but since this
    program is just a simulation it the easiest approach. Additionally, debugging is 
    simpler since table slots and process IDs are never re-used. */
    double cumulativeTimeDiff = 0;
    int numTerminatedProcesses = 0;
    bool createProgram(const string &filename, vector<Instruction> &program)
    {
        ifstream file;
        int lineNum = 0;
        file.open(filename.c_str());
        if (!file.is_open()) 
        {
            cout << "Error opening file " << filename << endl;
        return false;
        }
    while (file.good()) {
        string line;
        getline(file, line);
    
    trim(line);
    if (line.size() > 0) {
        Instruction instruction;
        instruction.operation = toupper(line[0]);
        instruction.stringArg = trim(line.erase(0, 1));
        stringstream argStream(instruction.stringArg);
        switch (instruction.operation) {
        case 'S': // Integer argument.
        case 'A': // Integer argument.
        case 'D': // Integer argument.
        case 'F': // Integer argument.
        if (!(argStream >> instruction.intArg)) {
        cout << filename << ":" << lineNum << " - Invalid integer argument "
        << instruction.stringArg << " for "
        << instruction.operation << " operation" << endl;
    file.close();
    return false;
    }
    break;
    case 'B': // No argument.
    case 'E': // No argument.
    break;
    case 'R': // String argument.

