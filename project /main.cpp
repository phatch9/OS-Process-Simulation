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
void printCpuState(const string &operation) {
    cout << "Operation: " << operation << endl;
    cout << "CPU Value: " << cpu.value << endl;
    cout << "Program Counter: " << cpu.programCounter << endl;
    cout << "----------------------------" << endl;
}

// Implements the S operation.
void set(int value) {
    PcbEntry &runningProcess = pcbEntry[runningState];
    runningProcess.value = value;
    cpu.value = value;
    printCpuState("Set");
    }

// Implements the A operation.
void add(int value) {
    PcbEntry &runningProcess = pcbEntry[runningState];
    runningProcess.value += value;
    cpu.value += value;
    printCpuState("Add");
}

// Implements the D operation.
void decrement(int value) {
    PcbEntry &runningProcess = pcbEntry[runningState];
    runningProcess.value -= value;
    cpu.value -= value;
    printCpuState("Decrement");
}

// Performs scheduling.
void schedule() {
  if (runningState != -1) {
    return;
  }
  if (!readyState.empty()) {
    runningState = readyState.front();
    readyState.pop_front();
    PcbEntry &pcb = pcbEntry[runningState];
    pcb.state = STATE_RUNNING;
    cpu.pProgram = &pcb.program;
    cpu.programCounter = pcb.programCounter;
    cpu.value = pcb.value;
    cpu.timeSliceUsed = 0;
  }

  printCpuState("Schedule");
}


// Implements the E operation.
void end() {
  if (runningState == -1) {
    cout<<"Failed to end process"<<endl;
    return;
  }
  PcbEntry &pcb = pcbEntry[runningState];
  cumulativeTimeDiff += timestamp + 1 - pcb.startTime;
  numTerminatedProcesses++;
  runningState = -1;
  schedule();

  printCpuState("End");
}

// Implements the B operation.
void block() {
  if (runningState == -1)
    return;
  end();
  blockedState.push_back(runningState);
  PcbEntry &pcb = pcbEntry[runningState];
  pcb.state = STATE_BLOCKED;
  pcb.programCounter = cpu.programCounter;
  pcb.value = cpu.value;
  runningState = -1;
  schedule();

  printCpuState("Block");
}

// Implements the F operation.
void fork(int value) {
  int freeIndex = -1;
  for (int i = 0; i < 10; i++) {
    if (pcbEntry[i].processId == -1) {
        freeIndex = i;
        break;
    }
  }
  if (freeIndex == -1) {
    cout << "No free PCB entry available" << endl;
    return;
  }
    PcbEntry &parentPCB = pcbEntry[runningState];
    pcbEntry[freeIndex].processId = freeIndex;
    pcbEntry[freeIndex].parentProcessId = parentPCB.processId;
    pcbEntry[freeIndex].program = parentPCB.program;
    pcbEntry[freeIndex].programCounter =
    cpu.programCounter + 1; // Start execution after the 'F' instruction
    pcbEntry[freeIndex].value = parentPCB.value; // Start with the same value as parent
    pcbEntry[freeIndex].priority = parentPCB.priority; // Same priority as parent
    pcbEntry[freeIndex].state = STATE_READY;
    pcbEntry[freeIndex].startTime = timestamp;
    pcbEntry[freeIndex].timeUsed = 0;
    readyState.push_back(freeIndex);

    printCpuState("Fork");

    cpu.programCounter += value; // Move program counter to skip n instructions after the 'F' instruction

}

// Implements the R operation.
void replace(string &filename) {
    cpu.pProgram->clear();
    if (!createProgram(filename, *cpu.pProgram)) {
        // If loading the program fails, handle it accordingly
        cerr << "Failed to replace program from file: " << filename << endl;
        return;
    }
    cpu.programCounter = 0; // Reset the program counter
    cout << "Program replaced with the contents of file: " << filename << endl;
    }