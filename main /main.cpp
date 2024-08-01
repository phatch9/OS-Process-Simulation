#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

void reporter() {
   std::cout << "Reporter process: " << getpid() << std::endl;
   // Simulate work with sleep
   sleep(1);
}

void processManager() {
   for (int i = 0; i < 3; ++i) {
       pid_t pid = fork();
       if (pid == 0) {
           // Child process - Reporter
           reporter();

           exit(0);

       }
        else if (pid > 0) {
        // Parent process - Process Manager
           wait(NULL); // Wait for child to finish
       }
       else 
        {
           std::cerr << "Fork failed." << std::endl;
           exit(1);
        }
    }

}

int main() {

   std::cout << "Commander process: " << getpid() << std::endl;

   pid_t pid = fork();

   if (pid == 0) {
       // Child process - Process Manager
       processManager();
       exit(0);
   }
    else if (pid > 0) {
       // Parent process - Commander
       wait(NULL); // Wait for process manager to finish
   }

    else {

       std::cerr << "Fork failed." << std::endl;
    return 1;

   }
return 0;

}

