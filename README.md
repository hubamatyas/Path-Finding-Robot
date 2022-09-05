# Path-Finding-Robot

### Overview
The robotGame.c file is the source code for a path finding program. When running the program, you must specify the robot’s starting position on the x and y axis as well as its direction. The robot is constructed of these three values and is declared by struct robot in the code. When the robot needs moving (i.e., it’s turning or moving forward) the pointer myRobot (type of struct robot), which is created after parsing the command line arguments, is passed onto the relevant functions as an argument. The grid, where the robot can move around, is stored as a 2D array within struct grid. As of now, declaring the grid with struct isn’t necessary, however, when scaling the program (e.g., creating more than one grid, adding attributes to a grid) it would make life easier. Each block on the grid is essentially an element of the 2D array. For example, gridArray[0][0] represents the top left block on the grid.

### Running the game
Clone this repo and compile with gitBash: `gcc -o robotGame robotGame.c graphics.c`

Run with gitBash: `./robotGame 1 1 south | java -jar drawapp-2.0.jar`

The first two values are the starting x and y positions of the robot. In the command line, 1 1 is the top left corner, 10 10 (if it’s a 10 by 10 grid) is the bottom right corner. The next value is the starting direction (the program recognises S, s, south as well as South). If either of the values are out of range the program displays an error message, then terminates.

Flags with arguments are available, making the grid and the randomly placed obstacles dynamic. If one or more of the flag arguments are out of range or don’t correspond to the first three initialiser values (e.g., grid is only 5 by 5 but the robot’s starting x value is 10) the program displays an error message, then, depending on the error, terminates or runs the game with pre-set values.

`-o` is for specifying the number of obstacles

`-b` is for setting the size of each block on the grid

`-g` is for setting the size of the grid (e.g., -g 20 results in a 20 by 20 grid)

Example 1. with gitBash: `./robotGame 4 4 s -o 5 -g 4 -b 80 | java -jar drawapp-2.0.jar`

Example 2. with gitBash: `./robotGame 4 4 s -o 100 -g 120 -b 5 | java -jar drawapp-2.0.jar`

### Finding the marker
The idea behind the path finding algorithm is to assign values to each block on the grid based on the particular block’s distance to the marker. Using these values, but not storing or modifying them, the robot tries to find its way to the marker. Similarly to avoiding obstacles and the walls of the grid, the robot looks at its current position and evaluates the four possible moves it could make before actually moving or turning. It looks for the block with lowest value (out of the four possibilities) in the `canMove` function, then moves accordingly. In case the robot is stuck `canMove` returns 0 and the program terminates. If the program didn’t terminate, the robot would enter an infinite loop where it would go back and forth between two blocks (this is because the robot doesn't know which blocks it has already visited).
