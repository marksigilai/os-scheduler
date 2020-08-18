In this assignment, we implemented a simple system for running a series of tasks within one process. Tasks in this system run separate functions, and the scheduler needed to switch between them. These tasks have the ability to perform blocking operations. When a running task blocks, your scheduler stops running the current task and selects a new one to run in the meantime. This was cooperative scheduling.

While the system implemented could be used for other purposes, we were implementing it specifically to support the game Worm!.

This game implementation uses several tasks:

A main task that starts up the game
A task to update the worm state in the game board
A task that redraws the board periodically
A task that reads input from the user and processes controls
A task that generates "apples" at random locations on the board
A task that updates existing "apples" by spinning them and removing them after some time
Each task runs in a loop that contains a blocking operation.

The scheduling system runs these tasks in round-robin fashion, switching between them as the currently-executing task blocks.


----------------

For the dining philosophers problem, I have most of it working well, but for the
worm game running, it stops and crashes at some point, and the error changes when I change the order of running the scheduling loop(what task it begins with).

All the tests run fine though, I am not sure what could be causing the issue but the snake moves fine and everything seems to render well

The make file runs when the tests are all in the same subfolder 
