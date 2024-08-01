# Project: Process Management Simulation

## Goal: 
To simulate five process management functions: process creation, replacing the
current process image with a new process image, process state transition, process
scheduling, and context switching.
Using Linux system calls such as fork(), wait( ), pipe( ), and sleep( ). 
Read main pages of these system calls for details.

This simulation exercise consists of three types of Linux processes: commander, process
manager, and reporter. There is one commander process (this is the process that starts
your simulation), one process manager process that is created by the commander process,
and a number of reporter processes that get created by the process manager, as needed
