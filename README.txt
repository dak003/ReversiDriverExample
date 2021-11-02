--------------------------------READ ME--------------------------------
REVERSI Kernel Module implementation

Included in this Repo are two programs:
driver.c
reversi.c
userSpaceReversi.c

and two Documents:
preliminary.pdf
design.pdf

The driver.c program is a rudementary program developed for running the reversi game. 
driver.c is able to validate read() and write() operations from the reversi module, and
output a formated game board. This driver allows for running of the whole game from one file.
This file is not needed to run the Kernel module

userSpaceReversi.c is a user space implementation of reversi.c. This file is located under /test 
and was used to develope functions in a more developer friendly envioronment. This file is not needed
to run the Kernel module.

reversi.c is the kernel module implementation of Reversi. It correctly reads/writes from /dev/reversi.
reversi.c should run correctly, allowing for the user to choose to go first or second (X or O).
reversi.c should allow the player to modify the game board according to the rules of reversi.
reversi.c should allow the player to read the game board.
reversi.c does not hava all user input validation properly working, but works with correct commands
reversi.c does not have all user return messages implemented
reversi.c should properly implement the game logic needed to play reversi.