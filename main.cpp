#include <iostream>
#include <set>
#include <string>
#include <utility>

using namespace std;

int board[256][256];
set<pair<int, int>> filledTiles;

void printBoard()
{
    for (int i = 'A'; i <= 'G'; cout << endl, i++)
        for (int j = 'a'; j <= 'g'; j++)
            cout << board[i][j];
}

bool place(string myMove)
{
    int colour = myMove[0] - '0';
    char i = myMove[1];
    char j = myMove[2];

    // Try to place a tile of some colour at (i, j).
    // Return false if the move is illegal.

    if (i < 'A' || i > 'G') return false;
    if (j < 'a' || j > 'g') return false;
    board[i][j] = colour;
    return true;
}

bool slide(string myMove)
{
    char i = myMove[0];
    char j = myMove[1];
    char m = myMove[2];
    char n = myMove[3];

    // Try to move a tile from (i, j) to (m, n).
    // Return false if the move is illegal.

    if (i < 'A' || i > 'G') return false;
    if (j < 'a' || j > 'g') return false;
    if (m < 'A' || m > 'G') return false;
    if (n < 'a' || n > 'g') return false;

    if (i == m)
    {
        int delta = ((j < n) ? 1 : -1);
        for (int t = j + 1; t <= n; t += delta)
            if (board[i][t]) return false;
        swap(board[i][j], board[m][n]);
        return true;
    }

    if (j == n)
    {
        int delta = ((i < m) ? 1 : -1);
        for (int t = i + 1; t <= m; t += delta)
            if (board[t][j]) return false;
        swap(board[i][j], board[m][n]);
        return true;
    }

    return false;
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

int main()
{
    place("4Gd");
    place("4Ge");
    place("4Gf");
    place("4De");
    place("4Ee");

    cout << getScore() << endl;
    printBoard();
}
