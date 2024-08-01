# Project: Process Management Simulation

## Goal: 
To simulate five process management functions: process creation, replacing the
current process image with a new process image, process state transition, process
scheduling, and context switching.
Using Linux system calls such as fork(), wait( ), pipe( ), and sleep( ). 
Read the main pages of these system calls for details.

This simulation exercise consists of three types of Linux processes: commander, process
manager, and reporter. There is one commander process (this is the process that starts
your simulation), one process manager process that is created by the commander process,
and a number of reporter processes that get created by the process manager, as needed

## Commander Process
The commander process first creates a pipe and then a process manager process. It then
repeatedly reads commands (one command per second) from the standard input and
passes them to the process manager process via the pipe. There are four types of
commands:
```
1. Q: End of one unit of time.
2. U: Unblock the first simulated process in the blocked queue.
3. P: Print the current state of the system.
4. T: Print the average turnaround time and terminate the system
```
## Simulated Process
Process management simulation manages the execution of simulated processes. Each
simulated process is comprised of a program that manipulates (sets/updates) the value of
a single integer variable. Thus the state of a simulated process at any instant is comprised
of the value of its integer variable and the value of its program counter. A simulated
process program consists of a sequence of instructions. There are seven types of
instructions as follows:
- S n: Set the value of the integer variable to n, where n is an integer.
- A n: Add n to the value of the integer variable, where n is an integer.
- D n: Subtract n from the value of the integer variable, where n is an integer.
- B: Block this simulated process.
- E: Terminate this simulated process.
- F n: Create a new simulated process. The new (simulated) process is an exact copy of
the parent (simulated) process. The new (simulated) process executes from the instruction
immediately after this (F) instruction, while the parent (simulated) process continues its
execution N instructions after the next instruction.
- R filename: Replace the program of the simulated process with the program in the file
filename, and set the program counter to the first instruction of this new program


