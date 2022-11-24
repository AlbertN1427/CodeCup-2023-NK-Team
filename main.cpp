#include <iostream>
#include <climits>
#include <queue>
#include <string>
#include <utility>

using namespace std;

const int DEPTH = 3;

int board[256][256];
int tileCountOfColour[10];

void printBoard();
string cat(char i, char j); // Concatenate 'a' and 'b' into "ab".

// Placing and Sliding
void place(string myMove);
void undoPlace(string myMove);
void slide(string myMove);
void undoSlide(string myMove);

// Evaluation
int getScoreFromHorizontalExpansion(char row, char left, char right);
int getScoreFromVerticalExpansion(char column, char up, char down);
int getScore();

int getBestMoveForChaos(string& bestContinuation, const string& colour, int depth);
int getBestMoveForOrder(string& bestContinuation, const string& colour, int depth);

int main()
{
    ios::sync_with_stdio(false);

    for (int i = 1; i <= 7; i++)
        tileCountOfColour[i] = 7;

    int score = 0;

    string input;
    string mode = "Order";
    string colour;

    while (getline(cin, input))
    {
        if (input == "Quit") break;
        else if (input == "Start") mode = "Chaos";
        else if (mode == "Chaos")
        {
            if (input.size() == 4)
            {
                // Order makes a move
                slide(input);
                continue;
            }

            colour = input;

            string bestContinuation;
            getBestMoveForChaos(bestContinuation, colour, 1);
            place(bestContinuation);

            cout << bestContinuation.substr(1, 2) << endl;
        }
        else if (mode == "Order")
        {
            place(input);

            colour = input.substr(0, 1);

            string bestContinuation;
            getBestMoveForOrder(bestContinuation, colour, 1);
            slide(bestContinuation);

            cout << bestContinuation << endl;
        }
        //printBoard();
    }
}

void printBoard()
{
    for (int i = 'A'; i <= 'G'; cout << endl, i++)
        for (int j = 'a'; j <= 'g'; j++)
            cout << board[i][j];
}

string cat(char i, char j)
{
    return string(1, i) + string(1, j);
}

void place(string myMove)
{
    int colour = myMove[0] - '0';
    char i = myMove[1];
    char j = myMove[2];
    board[i][j] = colour;
    tileCountOfColour[colour]--;
}

void undoPlace(string myMove)
{
    int colour = myMove[0] - '0';
    char i = myMove[1];
    char j = myMove[2];
    board[i][j] = 0;
    tileCountOfColour[colour]++;
}

void slide(string myMove)
{
    char i = myMove[0];
    char j = myMove[1];
    char m = myMove[2];
    char n = myMove[3];
    swap(board[i][j], board[m][n]);
}

void undoSlide(string myMove)
{
    char i = myMove[0];
    char j = myMove[1];
    char m = myMove[2];
    char n = myMove[3];
    swap(board[m][n], board[i][j]);
}

int getScoreFromHorizontalExpansion(char row, char left, char right)
{
    int score = 0;
    while ('a' <= left && right <= 'g')
    {
        if (!board[row][left]) break;
        if (!board[row][right]) break;
        if (board[row][left] != board[row][right]) break;
        if (left < right)
            score += right - left + 1;
        left--; right++;
    }
    return score;
}

int getScoreFromVerticalExpansion(char column, char up, char down)
{
    int score = 0;
    while ('A' <= up && down <= 'G')
    {
        if (!board[up][column]) break;
        if (!board[down][column]) break;
        if (board[up][column] != board[down][column]) break;
        if (up < down) score += down - up + 1;
        up--; down++;
    }
    return score;
}

int getScore()
{
    int score = 0;
    for (char i = 'A'; i <= 'G'; i++)
        for (char j = 'a'; j <= 'g'; j++)
        {
            score += getScoreFromHorizontalExpansion(i, j, j);
            score += getScoreFromHorizontalExpansion(i, j, j + 1);
            score += getScoreFromVerticalExpansion(j, i, i);
            score += getScoreFromVerticalExpansion(j, i, i + 1);
        }
    return score;
}

int getNewScore(int previousScore, string previousMove);

int getBestMoveForChaos(string& bestContinuation, const string& colour, int depth)
{
    int scoreInCaseOfBestMove = INT_MAX;
    for (char i = 'A'; i <= 'G'; i++)
        for (char j = 'a'; j <= 'g'; j++)
        {
            if (board[i][j]) continue;

            string tile = cat(i, j);
            string myMove = colour + tile;

            place(myMove);

            int nextScore = getScore();
            string temp;
            if (depth < DEPTH)
                nextScore = getBestMoveForOrder(temp, colour, depth + 1);

            if (nextScore < scoreInCaseOfBestMove)
            {
                bestContinuation = myMove;
                scoreInCaseOfBestMove = nextScore;
            }

            undoPlace(myMove);
        }
    return scoreInCaseOfBestMove;
}

int getBestMoveForOrder(string& bestContinuation, const string& colour, int depth)
{
    int scoreInCaseOfBestMove = INT_MIN;
    for (char i = 'A'; i <= 'G'; i++)
        for (char j = 'a'; j <= 'g'; j++)
        {
            if (!board[i][j]) continue;

            string tile = cat(i, j);

            queue<string> legalMoves;
            legalMoves.push(tile + tile);

            for (char column = j + 1; column <= 'g' && board[i][column] == 0; column++)
                legalMoves.push(tile + cat(i, column));
            for (char column = j - 1; column >= 'a' && board[i][column] == 0; column--)
                legalMoves.push(tile + cat(i, column));
            for (char row = i + 1; row <= 'G' && board[row][j] == 0; row++)
                legalMoves.push(tile + cat(row, j));
            for (char row = i - 1; row >= 'A' && board[row][j] == 0; row--)
                legalMoves.push(tile + cat(row, j));

            while (!legalMoves.empty())
            {
                string myMove = legalMoves.front();
                legalMoves.pop();

                slide(myMove);

                int nextScore = INT_MIN;

                int worstNextScore = INT_MAX;
                string temp;
                if (depth < DEPTH)
                    for (int nextColour = 1; nextColour <= 7; nextColour++)
                    {
                        if (!tileCountOfColour[nextColour]) continue;

                        int nextScoreIfThisColour = getBestMoveForChaos(temp, to_string(nextColour), depth + 1);

                        if (nextScoreIfThisColour < worstNextScore)
                            worstNextScore = nextScoreIfThisColour;

                        nextScore = worstNextScore;
                    }
                else nextScore = getScore();

                if (nextScore > scoreInCaseOfBestMove)
                {
                    bestContinuation = myMove;
                    scoreInCaseOfBestMove = nextScore;
                }

                undoSlide(myMove);
            }
        }
    return scoreInCaseOfBestMove;
}
