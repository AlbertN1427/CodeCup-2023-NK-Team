/*
 * Current issues:
 * +) Not tested for depth > 0.
 */

#include <iostream>
#include <climits>
#include <queue>
#include <string>
#include <utility>

using namespace std;

int board[256][256];
int tileCountOfColour[10];

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

    // Assuming that a placement at (i, j) is legal.

    board[i][j] = colour;
    tileCountOfColour[colour]--;
}

void undoPlace(string myMove)
{
    int colour = myMove[0] - '0';
    char i = myMove[1];
    char j = myMove[2];

    // Assuming that the placement at (i, j) was legal.

    board[i][j] = 0;
    tileCountOfColour[colour]++;
}

void slide(string myMove)
{
    char i = myMove[0];
    char j = myMove[1];
    char m = myMove[2];
    char n = myMove[3];

    // Assuming that a slide from (i, j) to (m, n) is legal.

    swap(board[i][j], board[m][n]);
}

void undoSlide(string myMove)
{
    char i = myMove[0];
    char j = myMove[1];
    char m = myMove[2];
    char n = myMove[3];

    // Assuming that the slide from (i, j) to (m, n) was legal.

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

int getBestMove(string& bestMove, const string& type, const string& colour, int depth)
{
    if (type == "Chaos")
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
                if (depth != 0) nextScore = getBestMove(bestMove, "Order", colour, depth - 1);

                if (nextScore < scoreInCaseOfBestMove)
                {
                    bestMove = myMove;
                    scoreInCaseOfBestMove = nextScore;
                }

                undoPlace(myMove);
            }
        return scoreInCaseOfBestMove;
    }

    if (type == "Order")
    {
        int nextColourAsInt = 0;
        int mostAbundantColour = 0;
        for (int i = 1; i <= 7; i++)
            if (tileCountOfColour[i] > mostAbundantColour)
            {
                nextColourAsInt = i;
                mostAbundantColour = tileCountOfColour[i];
            }
        string nextColour = string(nextColourAsInt + '0', 1);

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

                    int nextScore = getScore();
                    if (depth != 0) nextScore = getBestMove(bestMove, "Chaos", nextColour, depth - 1);

                    if (nextScore > scoreInCaseOfBestMove)
                    {
                        bestMove = myMove;
                        scoreInCaseOfBestMove = nextScore;
                    }

                    undoSlide(myMove);
                }
            }
    }

    return 0;
}

int main()
{
    ios::sync_with_stdio(false);

    for (int i = 1; i <= 7; i++)
        tileCountOfColour[i] = 7;

    string input;
    string mode = "Order";

    while (getline(cin, input))
    {
        if (input == "Quit") break;
        else if (input == "Start") mode = "Chaos";
        else if (mode == "Chaos")
        {
            string myMove;
            getBestMove(myMove, "Chaos", input, 0);
            place(myMove);
            cout << myMove.substr(1, 2) << endl;
        }
        else if (mode == "Order")
        {
            string colour = input.substr(0, 1);

            place(input);

            string myMove;
            getBestMove(myMove, "Order", colour, 0);
            slide(myMove);

            cout << myMove << endl;
        }
    }
}
