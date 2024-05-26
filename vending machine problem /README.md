[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-f059dc9a6f8d3a56e377f745f24479a46679e63a5d9fe6f495e02850cd0d8118.svg)](https://classroom.github.com/online_ide?assignment_repo_id=6630333&assignment_repo_type=AssignmentRepo)
# Operating Systems 5783: Assignment 3 Threads and Locks: Bounded Buffer
Template repository for assignment 3 in the Operating Systems 5783 course

Lecturer: Michael J May, Kinneret College on the Sea of Galilee

# Submitted by
Student 1: Alon Haim Chitlaru 

Student 2: Alina Surov 

# Hours worked

Student 1: 20 Hours

Student 2: 20 Hours

# How the code works and enforces mutual exclusion

The program contains the vending-machine as the main thread , two additional threads (Consumer and Supplier) and a Monitor which contains the struct of the mutex, condition variables and counters.
Before each of the threads reach to the critical code in our program which is the action of increasing/decreasing the amount of units that are in the vending machine, each thread locks the mutex lock so 
there won't be interruptions or race conditions and unlocks the mutex after the critical code.
In addition there are condition variables that prevent from the buffer to be larger than the maximum capacity or lower than 0.
Each thread has a conditional variable in a while loop with a boolean statement that relate to their problem.
After a thread of Consumer or Supplier finishes its job it signals the other threads that are currently waiting by using broadcast to wake all of the waiting threads at the same time.
