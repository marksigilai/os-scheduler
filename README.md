This assignment involved implementing a simple system for running a series of tasks within one process. Tasks in this system run as separate functions, and the scheduler switches between them. These tasks have the ability to perform self blocking operations. When a running enters the blocked state, the scheduler stops running the current task and selects a new one to run in the meantime. This is cooperative scheduling.

Dependencies

This is a c program, and uses the gcc compiler to run the project. In your terminal/command window, run the gcc --version command to see if you have it installed. If not, install gcc for your specific OS by following this [link](https://www.guru99.com/c-gcc-install.html).

To run this project (and play the worm game!) follow these commands:

      git clone https://github.com/marksigilai/os-scheduler.git'
      
In the project root repo

      make 
      
      ./worm
      
      make clean

While the system implemented could be used for other purposes, it was implemented to specifically support the game Worm!.

This game implementation uses several tasks:

- A main task that starts up the game
- A task to update the worm state in the game board
- A task that redraws the board periodically
- A task that reads input from the user and processes controls
- A task that generates "apples" at random locations on the board
- A task that updates existing "apples" by spinning them and removing them after some time
- Each task runs in a loop that contains a blocking operation.

The scheduling system runs these tasks in round-robin fashion, switching between them as the currently-executing task blocks.
