/*
 * Author: Christopher Guzman partnered with Julia Gulia
 * Assignment: Lab 2 Assignment
 * Course: CSE512 - Intro to Artificial Intelligence
 * Description: Study and analysis of the Minimax and Alpha-Beta Pruning
 * 				algorithms through a game of Stake and Raid similar to Othello.
 * 				Program evaluates the best move for current player using either
 * 				of the two algorithms with a cutoff-test implementation. 
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <limits>

using namespace std;

// structure for positions of the board
struct Node
{
	vector<string> position; // position of the board
	int score;               // the evaluated score of the position
	string move;             // location of the move
	string type;             // the type of move made

	// variables to tell us where on the board we are
	bool middle;
	bool right;
	bool left;

	// default constructor
	Node()
	{
		type = "Stake";
		middle = false;
		right = false;
		left = false;
	}
};

// class that will hold all data from input file and run algorithms
class Board
{
public:
	Board(); // default constructor

	// Input / Output functions
	void InitBoard(); // read file and initialize member variables
	void PrintData(); // Print data from board to terminal. (Debugging)

	// Algorithm functions
	int Minimax(Node, int, bool, string);
	int AlphaBeta(Node, int, bool, string, int, int);
	// will run algorithm specified in file and output the best option to file
	void BestPosition();

	// Position manipulation functions
	void SetScore(Node&);                // evaluation score of the nodes board position
	bool isAdjacent(Node&, string, int); // check if player position adjacent to player
	bool isRaid(Node&, string, int);     // check if player position is raiding opponent
	void SetMove(Node&, int);            // set the location of the move played

private:
	int size;               // store value of the size of the board
	string mode;            // store the game mode (MINIMAX / ALPHABETA)
	string player;          // store maximizing player
	int depth;              // store value of the depth limit search for cutoff test
	vector<int> cellValues; // store integer values of the board cells
	int alpha;              // store value for alpha
	int beta;               // store value for beta
	Node initPos;           // initial position of the board
	Node bestPos;           // best position for maximizing player
	string arr[26] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P",
					  "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };
};

// BOARD CLASS IMPLEMENTATION //

Board::Board()
{
	// initialize member variables to default values
	size = 0;
	mode = "";
	player = "";
	depth = 0;

	// Initialize worst outcomes
	bestPos.score = numeric_limits<int>::min();
	alpha = numeric_limits<int>::min();
	beta = numeric_limits<int>::max();
}

void Board::InitBoard()
{
	// create ifstream object for file input
	ifstream infile("input.txt");

	// validate file
	if (!infile.is_open())
	{
		cout << "ERROR: FILE FAILED TO LOAD.\n";
		exit(1);
	}

	string temp;   // store the line of file temporarily
	int index = 1; // hold index of the file
	// read through file
	while (getline(infile, temp))
	{
		if (index == 1)
		{
			size = stoi(temp);
		}
		else if (index == 2)
		{
			mode = temp;
		}
		else if (index == 3)
		{
			player = temp;
		}
		else if (index == 4)
		{
			depth = stoi(temp);
		}
		else if (index < (5 + size))
		{
			// insert line into stringstream object
			stringstream ss(temp);
			int val; // will store single value from line
			// split line into cell values
			for (int i = 0; i < size; i++)
			{
				ss >> val;
				cellValues.push_back(val);
			}
		}
		else
		{
			// separate the lines into cell state
			for (int i = 0; i < size; i++)
			{
				// construct string with one char from line at position i
				string state(1, temp.at(i));
				initPos.position.push_back(state);
			}
		}
		index++;
	}

	// close ifstream
	infile.close();
} //END InitBoard

void Board::PrintData()
{
	// output member variables to screen
	cout << size << endl;
	cout << mode << endl;
	cout << player << endl;
	cout << depth;

	for (int i = 0; i < size * size; i++)
	{
		if (i % size == 0)
			cout << endl;
		cout << cellValues[i] << " ";
	}

	for (int j = 0; j < size * size; j++)
	{
		if (j % size == 0)
			cout << endl;
		cout << initPos.position[j];
	}
} //END PrintData

void Board::BestPosition()
{

	if (mode == "MINIMAX") {
		bestPos.score = Minimax(initPos, depth, true, player);
	}
	else {
		bestPos.score = AlphaBeta(initPos, depth, true, player, alpha, beta);
	}

	cout << bestPos.move << " " << bestPos.type;
	for (int i = 0; i < size * size; i++)
	{
		if (i % size == 0)
			cout << endl;
		cout << bestPos.position[i];
	}

	// open file for output
	ofstream outfile("output.txt");

	outfile << bestPos.move << " " << bestPos.type;
	for (int i = 0; i < size * size; i++)
	{
		if (i % size == 0)
			outfile << endl;
		outfile << bestPos.position[i];
	}

	outfile.close();
}

int Board::Minimax(Node n, int d, bool isMax, string p)
{
	// check for cutoff test
	if (d == 0)
	{
		SetScore(n);
		return n.score;
	}

	int bestScore;

	if (isMax)
	{
		// create variable to hold the best score for max
		bestScore = numeric_limits<int>::min();
		// check all children of the node position
		for (int i = 0; i < size * size; i++)
		{
			// create copy of node position
			Node temp;
			temp.position = n.position;

			if (temp.position[i] == ".")
			{
				// place player in position
				temp.position[i] = p;
				SetMove(temp, i);
				if (isAdjacent(temp, p, i))
				{
					if (isRaid(temp, p, i))
					{
						temp.type = "Raid";
					}
				}
				int score;
				p = (p == "O") ? "X" : "O";
				score = Minimax(temp, d - 1, false, p);
				// reset p for next iteration
				p = (p == "O") ? "X" : "O";
				// compare scores
				bestScore = max(bestScore, score);
				// save the best position best on score into bestPos
				if (bestScore > bestPos.score and d == depth)
				{
					bestPos.position = temp.position;
					bestPos.move = temp.move;
					bestPos.type = temp.type;
					bestPos.score = bestScore;
				}
			}
		}
		return bestScore;
	}
	else
	{
		// create variable to hold the best score for min
		bestScore = numeric_limits<int>::max();
		// check all children of the node position
		for (int i = 0; i < size * size; i++)
		{
			// create copy of node position
			Node temp;
			temp.position = n.position;

			if (temp.position[i] == ".")
			{
				// place player in position
				temp.position[i] = p;
				SetMove(temp, i);
				if (isAdjacent(temp, p, i))
				{
					if (isRaid(temp, p, i))
					{
						temp.type = "Raid";
					}
				}
				int score;
				p = (p == "O") ? "X" : "O";
				score = Minimax(temp, d - 1, true, p);
				// reset p for next iteration
				p = (p == "O") ? "X" : "O";
				// compare scores
				bestScore = min(bestScore, score);
			}
		}
		return bestScore;
	}
} //END Minimax

int Board::AlphaBeta(Node n, int d, bool isMax, string p, int alpha, int beta)
{
	// check for cutoff test
	if (d == 0)
	{
		SetScore(n);
		return n.score;
	}

	int bestScore;

	if (isMax)
	{
		// create variable to hold the best score for max
		bestScore = numeric_limits<int>::min();
		// check all children of the node position
		for (int i = 0; i < size * size; i++)
		{
			// create copy of node position
			Node temp;
			temp.position = n.position;

			if (temp.position[i] == ".")
			{
				// place player in position
				temp.position[i] = p;
				SetMove(temp, i);
				if (isAdjacent(temp, p, i))
				{
					if (isRaid(temp, p, i))
					{
						temp.type = "Raid";
					}
				}
				int score;
				p = (p == "O") ? "X" : "O";
				score = AlphaBeta(temp, d - 1, false, p, alpha, beta);
				// reset p for next iteration
				p = (p == "O") ? "X" : "O";
				// compare scores
				bestScore = max(bestScore, score);
				alpha = max(alpha, bestScore);
				if (alpha >= beta) {
					/*cout << "Node Trimmed after this position.";
					for (int k = 0; k < size * size; k++) {
						if (k % size == 0) cout << endl;
						cout << temp.position[k];
					}
					cout << endl;*/
					break;
				}
				// save the best position best on score into bestPos
				if (bestScore > bestPos.score and d == depth)
				{
					bestPos.position = temp.position;
					bestPos.move = temp.move;
					bestPos.type = temp.type;
					bestPos.score = bestScore;
				}
			}
		}
		return bestScore;
	}
	else
	{
		// create variable to hold the best score for min
		bestScore = numeric_limits<int>::max();
		// check all children of the node position
		for (int i = 0; i < size * size; i++)
		{
			// create copy of node position
			Node temp;
			temp.position = n.position;

			if (temp.position[i] == ".")
			{
				// place player in position
				temp.position[i] = p;
				SetMove(temp, i);
				if (isAdjacent(temp, p, i))
				{
					if (isRaid(temp, p, i))
					{
						temp.type = "Raid";
					}
				}
				int score;
				p = (p == "O") ? "X" : "O";
				score = AlphaBeta(temp, d - 1, true, p, alpha, beta);
				// reset p for next iteration
				p = (p == "O") ? "X" : "O";
				// compare scores
				bestScore = min(bestScore, score);
				beta = min(beta, bestScore);
				if (alpha >= beta) {
					/*cout << "Node Trimmed. After this Position.";
					for (int k = 0; k < size * size; k++) {
						if (k % size == 0) cout << endl;
						cout << temp.position[k];
					}
					cout << endl;*/
					break;
				}
			}
		}
		return bestScore;
	}
}

bool Board::isAdjacent(Node& n, string p, int pos)
{
	// find where on the board we are
	// then check if we are adjacent to the current player
	if ((pos % size) == 0)
	{
		n.left = true;

		if (n.position[pos + 1] == p)
			return true;
		else if ((pos + size) < size * size and n.position[pos + size] == p)
			return true;
		else if ((pos - size) >= 0 and n.position[pos - size] == p)
			return true;
		else
			return false;
	}
	else if ((pos + 1) % size == 0)
	{
		n.right = true;
		if (n.position[pos - 1] == p)
			return true;
		else if ((pos + size) < size * size and n.position[pos + size] == p)
			return true;
		else if ((pos - size) > 0 and n.position[pos - size] == p)
			return true;
		else
			return false;
	}
	else
	{
		n.middle = true;
		if (n.position[pos - 1] == p)
			return true;
		else if (n.position[pos + 1] == p)
			return true;
		else if ((pos + size) < size * size and n.position[pos + size] == p)
			return true;
		else if ((pos - size) > 0 and n.position[pos - size] == p)
			return true;
		else
			return false;
	}
	return false;
} // END isAdjacent

bool Board::isRaid(Node& n, string p, int pos)
{
	// check if we are in a raiding position and perform raid if we are
	string opponent = "";
	bool raid = false;
	opponent = (p == "O") ? "X" : "O";

	// check all possible raiding squares from pos parameter
	if (n.left)
	{
		if (n.position[pos + 1] == opponent)
		{
			n.position[pos + 1] = p;
			raid = true;
		}
		if ((pos + size) < size * size and n.position[pos + size] == opponent)
		{
			n.position[pos + size] = p;
			raid = true;
		}
		if ((pos - size) >= 0 and n.position[pos - size] == opponent)
		{
			n.position[pos - size] = p;
			raid = true;
		}
	}
	else if (n.right)
	{
		if (n.position[pos - 1] == opponent)
		{
			n.position[pos - 1] = p;
			raid = true;
		}
		if ((pos + size) < size * size and n.position[pos + size] == opponent)
		{
			n.position[pos + size] = p;
			raid = true;
		}
		if ((pos - size) > 0 and n.position[pos - size] == opponent)
		{
			n.position[pos - size] = p;
			raid = true;
		}
	}
	else
	{
		if (n.position[pos - 1] == opponent)
		{
			n.position[pos - 1] = p;
			raid = true;
		}
		if (n.position[pos + 1] == opponent)
		{
			n.position[pos + 1] = p;
			raid = true;
		}
		if ((pos + size) < size * size and n.position[pos + size] == opponent)
		{
			n.position[pos + size] = p;
			raid = true;
		}
		if ((pos - size) > 0 and n.position[pos - size] == opponent)
		{
			n.position[pos - size] = p;
			raid = true;
		}
	}
	return raid;
} // END isRaid

void Board::SetScore(Node& n)
{
	// variables for the accumulated points of each player
	int oPoints = 0;
	int xPoints = 0;

	// add points together for each player
	for (int i = 0; i < size * size; i++)
	{
		if (n.position[i] == "O")
			oPoints += cellValues[i];
		else if (n.position[i] == "X")
			xPoints += cellValues[i];
	}

	// determine the score value for the node depending on who is max
	if (player == "O")
		n.score = oPoints - xPoints;
	else
		n.score = xPoints - oPoints;
} //END SetScore

void Board::SetMove(Node& n, int pos)
{
	n.move = arr[pos % size];
	n.move += to_string((pos / size) + 1);
} //END SetMove

int main()
{
	Board board;
	board.InitBoard();
	//board.PrintData();
	board.BestPosition();
	return 0;
}