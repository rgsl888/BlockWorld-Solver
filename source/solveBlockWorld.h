#ifndef __SOLVE_BLOCK_WORLD_H__
#define __SOLVE_BLOCK_WORLD_H__

#define FIRST_ROW 0
#define MAX_ITERATIONS 50000

class Node
{
    public :
    // Parent node
    class Node* parent;

    // Array to store the block world
    char **blockWorld;
    // Path cost
    int numOfStacks;
    int numOfBlocks;
    int pathCost;
    // total ccost = Heuristic + pathCost
    int totalCost;

    // Functions
    // Default constructor
    Node ()
    {

    }
    Node (char **array = NULL, int stackSize = 3, int blockSize = 5, int pathCost_t = 0, Node *parent_t = NULL)
    {
        pathCost = pathCost_t;
        totalCost = 0;
        blockWorld = array;
        numOfStacks = stackSize;
        numOfBlocks = blockSize;
        parent = parent_t;
    }

    ~Node ()
    {

    }
};

struct LessThanByPathCost
{
    bool operator()(const Node* lhs, const Node* rhs) const
    {
        return lhs->totalCost > rhs->totalCost;
    }
};

#endif
