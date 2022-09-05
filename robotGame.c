#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <getopt.h>
#include "graphics.h"

#define WAITTIME 300
#define MAXGRIDSIZE 120
#define MAXWINDOWSIZE 700
#define MINBLOCKSIZE 5
#define MARKERVALUE 1
#define ROBOTVALUE 2
#define OBSTACLEVALUE 500
#define LENGTHOFIMAGEPATH 18
#define IMAGESIZE 40
#define WINDOWSIZE (gridSize+4)*blockSize
#define PADDING (WINDOWSIZE-gridSize*blockSize)/2

struct grid
{
    int gridArray[MAXGRIDSIZE][MAXGRIDSIZE];
};

struct robot
{
  int posX;
  int posY;
  char direction;
};

// preset values for the grid in case an error occurs while parsing command line arguments
static int gridSize = 10;
static int blockSize = 40;
static struct grid *myGrid;
void createGrid();
struct robot *createRobot(int x, int y, char d);
void createObjects(int numOfObstacles);
void calculateDistance(int markerRow, int markerColumn);
int gridToPixel(int position);
void drawGrid();
void drawRobot(struct robot *myRobot, char *imagePath);
void drawObjects();
char *getImagePath(struct robot *myRobot, char newDirection);
void turn(struct robot *myRobot, char newDirection);
void forward(struct robot* myRobot, char newDirection);
int canMove(struct robot* myRobot);
int atMarker(struct robot *myRobot);
void go(struct robot* myRobot);

void createGrid()
{
    myGrid = calloc(sizeof(struct grid),1);
}

struct robot *createRobot(int x, int y, char d) 
{
    struct robot *robot = calloc(sizeof(struct robot),1);
    robot->posX = x;
    robot->posY = y;
    robot->direction = d;
    // set the block as occupied
    myGrid->gridArray[y][x] = ROBOTVALUE;
    return robot;
}

void createObjects(int numOfObstacles)
{
    srand(time(0));
    // +1 is to create the marker
    for (int i = 0; i < numOfObstacles+1; i++)
    {
        int randomY = rand() % gridSize;
        int randomX = rand() % gridSize;
        // check if block on the grid is occupied
        while (myGrid->gridArray[randomY][randomX] != 0)
        {
            randomY = rand() % gridSize;
            randomX = rand() % gridSize;
        }
        // initialise marker first, then obstacles
        myGrid->gridArray[randomY][randomX] = (i == 0 ? MARKERVALUE : OBSTACLEVALUE);
    }
}

int gridToPixel(int position)
{
    return PADDING + position*blockSize;
}

void drawGrid()
{
    background();
    for (int i = 0; i <= gridSize*blockSize; i+=blockSize)
    {
        drawLine(PADDING+i, PADDING, PADDING+i, WINDOWSIZE-PADDING);
        drawLine(PADDING, PADDING+i, WINDOWSIZE-PADDING, PADDING+i);
    }
}

void drawRobot(struct robot *myRobot, char *imagePath)
{
    foreground();
    clear();
    // do not display image if it would not fit blocks on grid
    if (IMAGESIZE == blockSize)
    {
        displayImage(imagePath, gridToPixel(myRobot->posX), gridToPixel(myRobot->posY));
    }
    else
    {
        setColour(green);
        fillOval(gridToPixel(myRobot->posX), gridToPixel(myRobot->posY), blockSize, blockSize);
    }    
    free(imagePath);
}

void drawObjects()
{
    background();
    for (int i = 0; i < gridSize; i++)
    {
        for (int j = 0; j < gridSize; j++)
        {
            if (myGrid->gridArray[i][j] == OBSTACLEVALUE)
            {
                setColour(red);
                int gridPosX = gridToPixel(j);
                int gridPosy = gridToPixel(i);
                IMAGESIZE == blockSize ? displayImage("images/obstacle.png", gridPosX, gridPosy) : fillRect(gridPosX, gridPosy, blockSize, blockSize);                
            }
            else if (myGrid->gridArray[i][j] == MARKERVALUE)
            {
                calculateDistance(i, j);
                setColour(blue);
                int gridPosX = gridToPixel(j);
                int gridPosy = gridToPixel(i);
                IMAGESIZE == blockSize ? displayImage("images/marker.png", gridPosX, gridPosy) : fillRect(gridPosX, gridPosy, blockSize, blockSize);
            }         
        }
    }    
}

void calculateDistance(int markerRow, int markerColumn)
{
    // assign value to each block based on its distance from marker
    for (int i = 0; i < gridSize; i++)
    {
        for (int j = 0; j < gridSize; j++)
        {
            if (myGrid->gridArray[i][j] != OBSTACLEVALUE)
            {
                int distX = abs(j-markerColumn);
                int distY = abs(i-markerRow);
                myGrid->gridArray[i][j] = distX + distY + MARKERVALUE;
            }
        }
    }
}

char *getImagePath(struct robot *myRobot, char newDirection)
{
    char *imagePath = calloc(sizeof(char), LENGTHOFIMAGEPATH);
    strcat(imagePath, "images/robot");

    // construct the path for image to display the correct orientation
    switch (newDirection)
    {
        case 'W':
            strncat(imagePath, &newDirection, 1);
            myRobot->direction = 'W';
            break;
        case 'E':
            strncat(imagePath, &newDirection, 1);
            myRobot->direction = 'E';
            break;
        case 'S':
            strncat(imagePath, &newDirection, 1);
            myRobot->direction = 'S';
            break;   
        default:
            strncat(imagePath, &newDirection, 1);
            myRobot->direction = 'N';
            break;
    }    
    return strcat(imagePath, ".png");
}

// combines the right() and left() functions
void turn(struct robot *myRobot, char newDirection)
{
    drawRobot(myRobot, getImagePath(myRobot, newDirection));
    sleep(WAITTIME);
}

void forward(struct robot* myRobot, char newDirection)
{   
    while (newDirection != myRobot->direction)
    {
        turn(myRobot, newDirection);
    }

    // move robot in the correct direction
    switch (myRobot->direction)
    {
        case 'W':
            myRobot->posX--;
            break;
        case 'E':
            myRobot->posX++;
            break;
        case 'S':
            myRobot->posY++;
            break;   
        default:
            myRobot->posY--;
            break;
    }
    drawRobot(myRobot, getImagePath(myRobot, myRobot->direction));
}

int canMove(struct robot* myRobot)
{
    int currRow = myRobot->posY;
    int currColumn = myRobot->posX;
    
    // robot looks for block with value less then value of block it is currently on
    if (currColumn+1 < gridSize && myGrid->gridArray[currRow][currColumn+1] < myGrid->gridArray[currRow][currColumn])
    {        
        forward(myRobot, 'E');
        return 1;
    }
    else if (currColumn-1 >= 0 && myGrid->gridArray[currRow][currColumn-1] < myGrid->gridArray[currRow][currColumn])
    {        
        forward(myRobot, 'W');
        return 1;
    }
    else if (currRow+1 < gridSize && myGrid->gridArray[currRow+1][currColumn] < myGrid->gridArray[currRow][currColumn])
    {        
        forward(myRobot, 'S');
        return 1;
    }
    else if (currRow-1 >= 0 && myGrid->gridArray[currRow-1][currColumn] < myGrid->gridArray[currRow][currColumn])
    {        
        forward(myRobot, 'N');
        return 1;
    }
    else
    {
        return 0;
    }
}

int atMarker(struct robot *myRobot)
{
    return ((myGrid->gridArray[myRobot->posY][myRobot->posX] == MARKERVALUE) ? 1 : 0);    
}

void go(struct robot* myRobot)
{
    drawGrid();
    drawObjects();
    // show robot in starting position
    drawRobot(myRobot, getImagePath(myRobot, myRobot->direction));
    sleep(WAITTIME);
    while (!atMarker(myRobot) && canMove(myRobot))
    {
        sleep(WAITTIME);
    }
    atMarker(myRobot) ? fprintf(stderr, "Marker found!\n") : fprintf(stderr, "Marker could not be found :( Let the robot try again\n");
}

void startGame(int startX, int startY, char startDir, int numOfObstacles)
{
    setWindowSize(WINDOWSIZE, WINDOWSIZE);
    createGrid();
    struct robot *myRobot = createRobot(--startX, --startY, startDir);
    createObjects(numOfObstacles);
    go(myRobot);

    free(myRobot);
    free(myGrid);
}

int main(int argc, char **argv)
{    
    // default starting values
    int startX, startY = 1;
    int numOfObstacles = 2;
    char startDir = 'E';

    // parsing starting values for robot
    if (argc > 3)
    {
        if (atoi(argv[1]) >= 1 && atoi(argv[2]) >= 1)
        {
            startX = atoi(argv[1]);
            startY = atoi(argv[2]);
        }
        else
        {
            fprintf(stderr, "Error[01]: Robot's starting position is out of range. \"x\" and \"y\" position must be between 1 and gridSize.\nClick 'Close Window' or type Ctrl+C to try again.");   
            return 1;
        }
        if (toupper(*(argv[3])) == 'N' || toupper(*(argv[3])) == 'E' || toupper(*(argv[3])) == 'S' || toupper(*(argv[3])) == 'W')
        {
            startDir = toupper(*(argv[3]));
        }
        else
        {
            fprintf(stderr, "Error[03]: Robot's starting direction is out of range. Direction must be 'N', 'E', 'S', 'W' or a word beginning with either of these letter.\nClick 'Close Window' or type Ctrl+C to try again.");
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Error[04]: Missing values. Robot can not be initialised.\nClick 'Close Window' or type Ctrl+C to try again.");
        return 1;
    }

    // parsing flag arguments
    int option;
    while ((option = getopt (argc, argv, "o:g:b:")) != -1)
    {
        switch (option)
        {
            case 'o':
                if (optarg != NULL && atoi(optarg) >= 0)
                {
                    numOfObstacles = atoi(optarg);
                }
                else
                {
                    fprintf(stderr, "Error[05]: numOfObstacles is out of range. Number of obstacles must be between 0 and the total number of blocks - 2.\nGame will begin with preset values\nClick 'Close Window' or type Ctrl+C to try again.\n");
                }
                break;
            case 'g':
                if (optarg != NULL && atoi(optarg) >= 2 && atoi(optarg) <= MAXGRIDSIZE)
                {
                    gridSize = atoi(optarg);
                }
                else
                {
                    fprintf(stderr, "Error[06]: gridSize is out of range\nGame will begin with preset values\nClick 'Close Window' or type Ctrl+C to try again.\n");
                }
                break;
            case 'b':
                if (optarg != NULL && atoi(optarg) >= MINBLOCKSIZE && atoi(optarg)*gridSize <= MAXWINDOWSIZE)
                {
                    blockSize = atoi(optarg);
                }
                else
                {
                    fprintf(stderr, "Error[07]: blockSize is too small or too big\nGame will begin with preset values\nClick 'Close Window' or type Ctrl+C to try again\n");
                }
                break;
            default:
                fprintf(stderr, "Error[08]: Unknown option: %c\n", optopt);
                break;
        }
    }
     
    // parsing flag arguments and robot starting values together
    if (startX > gridSize || startY > gridSize || numOfObstacles > pow(gridSize, 2)-2 || (4+gridSize)*blockSize > MAXWINDOWSIZE)
    {
        fprintf(stderr, "Error[09]: Out of range values for initialising robot or for one or more of the flags (-o -g -b)\nClick 'Close Window' or type Ctrl+C to try again\n");
        return 1;
    }

    startGame(startX, startY, startDir, numOfObstacles);
    return 0;
}
