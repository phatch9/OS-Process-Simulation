#include <algorithm>  // for std::remove_if
#include <cctype>     // for toupper()
#include <cerrno>     // for errno
#include <cstdlib>    // for EXIT_SUCCESS and EXIT_FAILURE
#include <cstring>    // for strerror()
#include <deque>      // for deque (used for ready and blocked queues)
#include <fstream>    // for ifstream (used for reading simulated programs)
#include <iostream>   // for cout, endl, and cin
#include <limits>     // for numeric_limits
#include <map>        // for map (used for PCB table)
#include <sstream>    // for stringstream (used for parsing simulated programs)
#include <sys/wait.h> // for wait()
#include <unistd.h> // for pipe(), read(), write(), close(), fork(),and _exit()
#include <vector>   // for vector (used for PCB table)

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

enum State { STATE_READY, STATE_RUNNING, STATE_BLOCKED };

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
int runningState = -1;
deque<int> readyState;
deque<int> blockedState;
double cumulativeTimeDiff = 0;
int numTerminatedProcesses = 0;

string trim(const string &s) 
{
    auto start = s.begin();
    while (start != s.end() && isspace(*start)) {
    start++;
    }
    auto end = s.end();
    do { 
        end--;
    } while (distance(start, end) > 0 && isspace(*end));
    return string(start, end + 1);
}
bool createProgram (const string &filename,  vector<Instruction> &program) 
{
    ifstream file;
    int lineNum = 0;
    string filename_proper = filename + ".txt";
    file.open(filename_proper);
    cout << filename << endl;

    if (!file.is_open()) {
        cout << "error occurs when opening" << filename <<endl;
        return false;
    }
    while (file.good()) {
        string line;
        getline(file, line);
        line = trim(line);
        if (line.size() > 0) {
        Instruction instruction;
        instruction.operation = toupper(line[0]);
        instruction.stringArg = trim(line.erase(0, 1));
        stringstream argStream(instruction.stringArg);
        switch (instruction.operation) {
            // Instructions SADF all take integer input
            // Integer argument
            case 'S': 
            case 'A': 
            case 'D': 
            case 'F':
            if (!(argStream >> instruction.intArg)) {
            cout << filename << ":" << lineNum << " - Invalid integer argument " << instruction.stringArg << " for " << instruction.operation
                << " operation" << endl;
                file.close();
            return false;
            }
            break;
                case 'B': // No argument.
                case 'E': // No argument.
            break;
            // String input
                case 'R': // String argument.
            if (instruction.stringArg.size() == 0) {
            cout << filename << ":" << lineNum << " - Missing string argument"
                << endl;
            file.close();
            return false;
            }
            break;
        default:
            cout << filename << ":" << lineNum << " - Invalid operation, "  << instruction.operation << endl;
            file.close();
            return false;
            }
            program.push_back(instruction);
        }
        lineNum++;
    }
    file.close();
    return true;
}
