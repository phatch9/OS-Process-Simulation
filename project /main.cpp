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

string trim(const string &s) {
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
