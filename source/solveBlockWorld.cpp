#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <queue>
#include <time.h>
#include "solveBlockWorld.h"

using namespace std;

// Description : This function will form the solution step list and will print for the reference
// Paramerters : A pointer of type Node which holds the end solution node
// Return      : None

void goalMoves (Node* goalNode)
{
    if (NULL == goalNode)
    {
	cout <<"Error : Goal node provided is NULL"<<endl;
	return ;
    }
    Node* parentNode = goalNode->parent;	// Temporary pointer to move through the parents
    vector<Node*> solNodes;			// Vector to store the solution nodes
    int stackSize = goalNode->numOfStacks;	
    int blkSize = goalNode->numOfBlocks;
    int numOfMoves = 0;				// Stores the number of moves made for statistics

    solNodes.push_back (goalNode);		

    if (parentNode != NULL)
    {
        // Loop through the parent chain and assemble the solution in an order
	do
	{
            solNodes.push_back (parentNode);
	    parentNode = parentNode->parent;
            numOfMoves ++;
        }while (parentNode != NULL);

        printf ("Number of moves for the solution : %d \n", numOfMoves);
        printf ("The solution shown below, \n");

        for (int index = (solNodes.size() - 1); index >= 0 ; index--)
	{
	    printf ("Move number : %lu\n", ((solNodes.size() - 1) - index));
            for (int blk = blkSize-1; blk >= 0; blk--)
            {
                for (int stack = 0; stack < stackSize; stack++)	
                {
                    if ('\0' == solNodes[index]->blockWorld[stack][blk])
                    {
                        printf (" [ ] ");
                    }
                    else
                    {
                        printf (" [%c] ", solNodes[index]->blockWorld[stack][blk]);
                    }
                }
                printf ("\n");
            }
            printf ("\n");
            for (int stk = 0; stk <stackSize; stk++)
            {
                printf(" --- ");
            }
            printf ("\n");
            for (int stk = 0; stk <stackSize; stk++)
            {
                printf("  %d  ", (stk+1));
            }
            printf ("\n\n");
        }
    }
    else
    {
        printf ("Problem is in solved state no need to solve \n");
    }
}


// Description : This function will form the solution step list and will print for the reference
// Paramerters : A pointer of type Node which holds the child node which will be checked against the list of 
//               checked node
//               A vector which holds all the children which have been gone through
// Return      : true - if node was already visited
//               false - if node is new

bool isVisitedNode (Node* childNode, vector<Node*> &visitedNodes)
{
    bool diffFound = false;
    bool matchFound = false;

    if (NULL == childNode)
    {
        cout <<"Error : Child node is NULL"<<endl;

        return false;
    }

    for (unsigned int index = 0; index < visitedNodes.size(); index++)
    {
        for (int stack = 0; stack < childNode->numOfStacks; stack++)
        {
            // Initialize status for the stack
            diffFound = false;
            if (0 != strcmp (childNode->blockWorld[stack], visitedNodes[index]->blockWorld[stack]))
            {
                // Difference found in one of the stack, so move to next node
                diffFound = true;
                break;
            }
        }
	
        //  If there was not even a signle difference in the matrix, i.e match found
        if (false == diffFound)
        {
            matchFound = true;		
	    
            break;
	}
    }

    return matchFound;
}


// Description : Function gets an approximate number of steps required to attain the solution from the current status
// Paramerters : A pointer of type Node which holds the child node 
// Return      : Returns approximate number of steps for the solution 

int heuristic (Node* childNode)
{
    unsigned int expNumOfMoves = 0;

    if (NULL != childNode)
    {
        int numOfBlksInFirstRow = 0;	        // To store number of block in the first row
        int stackSize = childNode->numOfStacks;
        int blockSize = childNode->numOfBlocks;
        bool inSeq = true;			// Will be used to check whether the first row characters are in sequence or not
        char currentReqChar = '\0';		// To store intermediate characters required to fill the first stack slot

        // To get and count the number of moves required to move the blocks which are taking place of other blocks in first stack
	if (0 != (numOfBlksInFirstRow = strlen (childNode->blockWorld[FIRST_ROW])))
	{
	    for (int blk = 0; blk < numOfBlksInFirstRow; blk++)
	    {
                // Check whether the sequence of the blocks are in sequence or anything is misplaced
                if ((('A' + blk) != childNode->blockWorld[FIRST_ROW][blk]) && ('\0' != childNode->blockWorld[FIRST_ROW][blk]))
                {
                    if (inSeq == true)
                    {
                        // The position of the first character which is out its the position in first row
                        currentReqChar = 'A' + blk;
                    }
                    inSeq = false;
                    // This expression is used to reduce any moves which tries to move the block from someother stacks to 
                    // first stack which is a improper move in most of the cases (exceptions are there)
                    expNumOfMoves += (abs(blockSize/(stackSize - 1)));
                }
                else if ('\0' != childNode->blockWorld[FIRST_ROW][blk])
                {
                    // This scenario is where a block is in its expected position in first stack
                    // but its prior elements are not in place, which makes position of the value misplcaed Eg stack0 : C B
                    if (false == inSeq)
                    {
                        expNumOfMoves += abs((blockSize)/(stackSize - 1));
                    }
                }
            }     
        }
	
        // Resetting the value back
	inSeq = true;
	for (int stk = 0; stk < stackSize; stk++)
	{
	    for (int blk = 0; blk < blockSize; blk++)
	    {
		if ('\0' == childNode->blockWorld[stk][blk])
		{
		    continue;
		}

		// Element is found in its proper place
		if ((0 == stk) && (('A' + blk) == childNode->blockWorld[stk][blk]) && (inSeq == true))
		{
		    continue;
		}
		else
		{
		    inSeq = false;
		}
		
                // Check all the elements above the current element and add the number of moves required to move them and get to
                // to the goal position of the current block
                for (int blk_t = blk + 1; blk_t < blockSize; blk_t++)
                {
                    if ('\0' != childNode->blockWorld[stk][blk_t])
                    {
                        // Check if the upper elements are bigger in ASCII value
                        // Number of moves required to remove bigger elements
                        if (childNode->blockWorld[stk][blk] < childNode->blockWorld[stk][blk_t])
                        {
		            // If the current position which needs to be filled in first row is under some higher elements,
                            // then increase the number of moves for any element above this.
			    // This is required to ensure that there will less chances of going for child node which might add
			    // more elements on top this element which needs to be moved to its solution
                            if (currentReqChar == childNode->blockWorld[stk][blk])
                            {
                                expNumOfMoves += 1;//(abs((blockSize)/(stackSize + 2)));
			    }
                            expNumOfMoves += 1;
                        }
                    }
                }
                // This is to move the element to its goal
		expNumOfMoves++;
            }
        }
    }
  
    return expNumOfMoves;
}

// Description : Generates all possible moves from the current state of the block world
// Paramerters : A pointer of type Node which represent the current state of the block world
//               and a vector which stores the nodes which have all possible moves
// Return      : None 

void successor (Node *parent, vector<Node*> &childNode)
{
    char **childBlockWorld = NULL;
    int lenOfRow = 0;
    int lenOfRow_i = 0;

    if (NULL == parent)
    {
        printf ("[%s:%u] Error : parent was NULL \n", __FILE__, __LINE__);
    }

    // Clear old data which might have been left in the vector
    childNode.clear();

    for (int stack = 0; stack < parent->numOfStacks; stack++)
    {
        // Skip if there are no element that can be moved
        if ((lenOfRow = strlen (parent->blockWorld[stack])) != 0)
        {
            for (int stack_i = 0; stack_i < parent->numOfStacks; stack_i++)
            {
                // Skip for the same row as we can't move a block within a row
                if (stack != stack_i)
                {
                    char** problem = NULL;
 
                    problem = new char *[parent->numOfStacks];

                    for (int index = 0; index < parent->numOfStacks; index++)
           	    {
                        problem [index] = new char[parent->numOfStacks];
                    }
                    lenOfRow_i = strlen (parent->blockWorld[stack_i]);
		    
                    for (int stk = 0; stk < parent->numOfStacks; stk++)
                    {
                        for (int blk = 0; blk < parent->numOfBlocks; blk++)
                        {
			    problem[stk][blk] = parent->blockWorld[stk][blk];
                        }
                    }
                    // The character position which we just moved is being changed to '\0'
                    problem[stack][lenOfRow - 1] = 0;
                    // The place which needed to be filled with themoved character
                    problem[stack_i][lenOfRow_i] = parent->blockWorld[stack][lenOfRow - 1];
                    Node* childBlockWorld = new Node(problem, parent->numOfStacks, parent->numOfBlocks, parent->pathCost + 1, parent);
                    // Push the child node to the vector
                    childNode.push_back (childBlockWorld); 
                }
            }
        }
    }
}


// Description : Checks whether alphabet is already used or not
// Paramerters : A character which needs to be checked
//               a pointer which has all the alphabets used  
// Return      : None
bool isAlphaUsed (char alpha, char* usedAlpha)
{
    bool used = false;

    if (NULL == usedAlpha)
    {
        printf ("[%s:%u]: Error: usedAlpha array is NULL \n", __FILE__, __LINE__ );

        return used;
    }

    if (NULL != strchr (usedAlpha, alpha))
    {
        used = true;
    }

    return used;
}

// Description : Generates a problem for block world
// Paramerters : Stack size, number of blocks and pointer to an array which would store the block world
// Return      : None
void probGenerator (int stackSize = 3, int blockSize = 5, char** problem = NULL)
{
    char usedAlpha [blockSize + 1] = {0}; // Stores the blocks/alphabets which are already added in the block world
    int index = 0;
    char alpha = '\0';

    if (NULL == problem )
    {
        printf ("[%s:%u]: Error: problem array is NULL \n", __FILE__, __LINE__);

        return ;
    }

    for (int stack = 1; stack < stackSize; stack++)
    {
        for (int blk = 0; blk < blockSize; blk++)
        {
            if (problem [stack][blk] == '\0')
            {
                // Reset the value
                alpha = '\0';
                while ((true == isAlphaUsed (alpha, usedAlpha)) && (alpha = 'A' + (time(NULL) % blockSize)));
                problem [stack][blk] = alpha;
                usedAlpha[index] = alpha;
                index ++;
                // To create a random way to get elements in different stack
                if (0 == ((time (NULL) + blk + stack) % 3 ) || (blockSize == strlen (usedAlpha)))
                {
                    break;
                }
            }
        }
        if (blockSize == strlen(usedAlpha))
        {
            break;
        }
	else if (stack == stackSize - 1)
        {
            // The increment in the for loop will get to 0
            stack = -1;
        }
    }
    
    printf ("Generated problem is as follows \n\n");
    for (int blk = blockSize - 1 ; blk >= 0; blk--)
    {
        for (int stk = 0; stk < stackSize; stk++)
        {
            if ('\0' == problem [stk][blk])
            {
                printf ("[ ] ");
            }
            else
            {
                printf ("[%c] ", problem [stk][blk]);
            }
        }
        printf ("\n");
    }
}


// Description : Checks whether problem is solved
// Paramerters : Number of blocks and a pointer to the node which has the current state of the block world
// Return      : None
bool isGoalReached (int numOfBlocks, Node* node)
{
    int indOfStack_1 = 0;
    bool ret = true;
   
    if (node != NULL)
    { 
        for (int blk = 0; blk < numOfBlocks; blk++)
        {
            // The goal is reached when all the blocks are arranged in order in first row
            if (('A'+blk) != node->blockWorld[indOfStack_1][blk])
            {
                ret = false;    

                break;
            }
        }
    }
    else
    {
        ret = false;
    }

    return ret;
}

int main (int argc, char** argv)
{
    if (argc == 3)
    {
        int numOfStacks = atoi (argv [1]);
        int numOfBlocks = atoi (argv [2]);
        vector<Node*> childNodes;             // To store the nodes generated in successor
        vector<Node*> visitedNodes;           // To store the visited nodes
        char** problem = NULL;                // Stores the blockworld root problem
        bool goalRchd = false;

        // Initialize the array
        problem = new char *[numOfStacks];

        for (int index = 0; index < numOfStacks; index++)
        {
            problem [index] = new char[numOfBlocks];
        }

        // Generate a problem
        probGenerator (numOfStacks, numOfBlocks, problem);
	printf ("\n\nStarted to solve .........\n");
 
        // Create the root node
        Node* rootProblem = new Node(problem, numOfStacks, numOfBlocks, 0, NULL);
        // Create a priority queue to store the nodes
        priority_queue<Node*, vector<Node*>, LessThanByPathCost> frontier;
        // Intialize frontier with root problem
        frontier.push (rootProblem);
	
        int iteration = 0;
        while (false == goalRchd)
        {
            if ( MAX_ITERATIONS < iteration)
            {
                printf ("[%s:%u] : Error: Iteration limit crossed, exiting the program.......... \n", __FILE__, __LINE__);
		break;
            }
            iteration++;
            // If the frontier size is 0, it means that the queue is exhausted and there is no solution
            if (0 == frontier.size())
            {
                cout << "The frontier is empty, so the graph is searched completely and could not find any solution "<< endl;

                 return 1;
            }
     
            // Get all the successors
            successor ((frontier.top ()), childNodes);
	    
            // Push the parent node to visited from which successors were derived
            visitedNodes.push_back (frontier.top ());
	    
            // Remove the parent node which will be replaced by its child nodes
            frontier.pop ();
            // Push the child into the priority queue as per their cost
            for (unsigned int index = 0; index < childNodes.size(); index++)
            {
                if (false == isVisitedNode (childNodes[index], visitedNodes))
                {
                    childNodes[index]->totalCost = childNodes[index]->pathCost + heuristic (childNodes[index]);
                    frontier.push (childNodes[index]);
                }
            }
            
            if (true == (goalRchd = isGoalReached (numOfBlocks, frontier.top ())))
            {
                printf ("\n----------------Goal is reached-------------------------"
                        "\nQueue Size      : %lu"
			"\nIterations      : %d\n", frontier.size(), iteration);
                // Print the goal moves
                goalMoves (frontier.top ());
            }
        }
    }
    else
    {
        printf ("Error: Arguments provided are improper, check the proper syntax below\n"
                "Syntax: blockworld stacksize numberofblocks \n");
    }

    return 0;
}
