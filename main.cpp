#pragma GCC optimize("Ofast,unroll-loops")
//#pragma GCC target("bmi,bmi2,lzcnt,popcnt")
#pragma GCC target("avx2,popcnt,lzcnt,abm,bmi,bmi2,fma,tune=native")

#include<bits/stdc++.h>

using namespace std;

#define int long long

const int MAX_DEPTH_FOR_CHAOS = 3;
const int MAX_DEPTH_FOR_ORDER = 3;
const int DEPTH=3;

int board[256][256];
int tileCountOfColour[10] = {0, 7, 7, 7, 7, 7, 7, 7};

vector<string> tileSequenceForChaos;
vector<string> tileSequenceForOrder;

vector<string> tileSequenceForChaos1;
vector<string> tileSequenceForOrder1;

// Utility
void printBoard();
void generateRandomColourSequence(vector<int>& colourSequence);
string cat(char i, char j); // Concatenate 'a' and 'b' into "ab".
bool isOutOfBound(const char& i, const char& j);

// Placing and Sliding
void place(string myMove);
void undoPlace(string myMove);
void slide(string myMove);
void undoSlide(string myMove);

// Evaluation
int getScoreFromHorizontalExpansion(char row, char left, char right);
int getScoreFromVerticalExpansion(char column, char up, char down);
int getScoreInRow(char row);
int getScoreInColumn(char column);

// Stoccfish
void initialiseTileSequences();
void init();
int _OwO_(const char& i, const char& j);
int getBestMoveForChaos(string& bestContinuation, const string& colour, const int& depth, const int& maxDepth, const int& currentDelta);
int getBestMoveForOrder(string& bestContinuation, const string& colour, const int& depth, const int& maxDepth, const int& currentDelta);

int getBestMoveForChaos1(string& bestContinuation, const string& colour, int depth, int currentDelta);
int getBestMoveForOrder1(string& bestContinuation, const string& colour, int depth, int currentDelta);

signed main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    initialiseTileSequences();
    init();

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
            getBestMoveForChaos(bestContinuation, colour, 1, 3 , 0);
            place(bestContinuation);

            cout << bestContinuation.substr(1, 2) << endl;
        }
        else if (mode == "Order")
        {
            place(input);

            colour = input.substr(0, 1);

            string bestContinuation;
            getBestMoveForOrder1(bestContinuation, colour, 1, 0);
            slide(bestContinuation);

            cout << bestContinuation << endl;
        }
    }
}

int getBestMoveForChaos1(string& bestContinuation, const string& colour, int depth, int currentDelta)
{
    int deltaInCaseOfBestMove = INT_MAX;
    for (auto tile : tileSequenceForChaos)
    {
        char i = tile[0];
        char j = tile[1];

        if (board[i][j]) continue;

        string myMove = colour + tile;

        int curr = getScoreInRow(i) + getScoreInColumn(j);

        place(myMove);

        int next = getScoreInRow(i) + getScoreInColumn(j);

        int delta = currentDelta + next - curr;
        string temp;
        if (depth < DEPTH)
            delta = getBestMoveForOrder1(temp, colour, depth + 1, delta);

        if (delta < deltaInCaseOfBestMove)
        {
            bestContinuation = myMove;
            deltaInCaseOfBestMove = delta;
        }

        undoPlace(myMove);
    }
    return deltaInCaseOfBestMove;
}

int getBestMoveForOrder1(string& bestContinuation, const string& colour, int depth, int currentDelta)
{
    float deltaInCaseOfBestMove = INT_MIN;
    for (auto tile : tileSequenceForChaos)
    {
        char i = tile[0];
        char j = tile[1];

        if (!board[i][j]) continue;

        queue<string> legalMoves;
        legalMoves.push(tile + tile);

        int searchRadius = 4 ;
        if (depth > 2) searchRadius = 2;

        for (char column = j + 1; column <= 'g' && board[i][column] == 0 && column <= j + searchRadius; column++)
            legalMoves.push(tile + cat(i, column));
        for (char column = j - 1; column >= 'a' && board[i][column] == 0 && column >= j - searchRadius; column--)
            legalMoves.push(tile + cat(i, column));
        for (char row = i + 1; row <= 'G' && board[row][j] == 0 && row <= i + searchRadius; row++)
            legalMoves.push(tile + cat(row, j));
        for (char row = i - 1; row >= 'A' && board[row][j] == 0 && row >= i - searchRadius; row--)
            legalMoves.push(tile + cat(row, j));

        while (!legalMoves.empty())
        {
            string myMove = legalMoves.front();
            legalMoves.pop();

            int curr = getScoreInRow(myMove[0]) + getScoreInColumn(myMove[1])
                    + getScoreInRow(myMove[2]) + getScoreInColumn(myMove[3]);

            slide(myMove);

            int next = getScoreInRow(myMove[0]) + getScoreInColumn(myMove[1])
                       + getScoreInRow(myMove[2]) + getScoreInColumn(myMove[3]);

            float delta = currentDelta + next - curr;
            if (depth < DEPTH)
            {
                float expectedDelta = 0;
                string temp;
                int tilesLeft = 0;
                for (int nextColour = 1; nextColour <= 7; nextColour++)
                {
                    if (!tileCountOfColour[nextColour]) continue;

                    int deltaIfThisColour = getBestMoveForChaos1(temp, to_string(nextColour), depth + 1, delta);

                    expectedDelta += tileCountOfColour[nextColour] * deltaIfThisColour;
                    tilesLeft += tileCountOfColour[nextColour];
                }
                if (tilesLeft == 0) break;
                expectedDelta /= tilesLeft;
                delta = expectedDelta;
            }

            if (delta > deltaInCaseOfBestMove)
            {
                bestContinuation = myMove;
                deltaInCaseOfBestMove = delta;
            }

            undoSlide(myMove);
        }
	}
    return deltaInCaseOfBestMove;
}

void printBoard()
{
    cout << "  a b c d e f g " << endl;
    for (char i = 'A'; i <= 'G'; i++)
    {
        cout << i << ' ';
        for (char j = 'a'; j <= 'g'; j++)
            cout << board[i][j] << ' ';
        cout << endl;
    }
}

void generateRandomColourSequence(vector<int>& colourSequence)
{
    for (int i = 1; i <= 7; i++)
        for (int j = 1; j <= 7; j++)
            colourSequence.push_back(i);
    random_device rd;
    mt19937 g(rd());
    shuffle(colourSequence.begin(), colourSequence.end(), g);
}

string cat(char i, char j)
{
    return string(1, i) + string(1, j);
}

bool isOutOfBound(const char& i, const char& j)
{
    if (i < 'A' || i > 'G') return true;
    if (j < 'a' || j > 'g') return true;
    return false;
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
        if (left < right) score += right - left + 1;
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

int getScoreInRow(char row)
{
    int score = 0;
    for (char j = 'a'; j <= 'g'; j++)
    {
        score += getScoreFromHorizontalExpansion(row, j, j);
        score += getScoreFromHorizontalExpansion(row, j, j + 1);
    }
    return score;
}

int getScoreInColumn(char column)
{
    int score = 0;
    for (char i = 'A'; i <= 'G'; i++)
    {
        score += getScoreFromVerticalExpansion(column, i, i);
        score += getScoreFromVerticalExpansion(column, i, i + 1);
    }
    return score;
}

void initialiseTileSequences()
{
    for (char i = 'D'; i <= 'G'; i++)
    {
    	for (char j = 'd'; j <= 'g'; j++)
            tileSequenceForChaos.push_back(cat(i, j));
    	for (char j = 'c'; j >= 'a'; j--)
            tileSequenceForChaos.push_back(cat(i, j));
    }

    for (char i = 'C'; i >= 'A'; i--)
    {
    	for (char j = 'd'; j <= 'g'; j++)
            tileSequenceForChaos.push_back(cat(i, j));
    	for (char j = 'c'; j >= 'a'; j--)
            tileSequenceForChaos.push_back(cat(i, j));
    }

    for (char i = 'D'; i <= 'G'; i++)
    {
    	tileSequenceForOrder.push_back(cat(i, 'G'));
    	for (char j = 'c'; j < 'g'; j++)
            tileSequenceForOrder.push_back(cat(i, j));
    	for (char j = 'b'; j >= 'a'; j--)
            tileSequenceForOrder.push_back(cat(i, j));
    }

    for (char i = 'C'; i >= 'A'; i--)
    {
    	for (char j = 'c'; j < 'g'; j++)
            tileSequenceForOrder.push_back(cat(i, j));
    	for (char j = 'b'; j >= 'a'; j--)
            tileSequenceForOrder.push_back(cat(i, j));
        tileSequenceForOrder.push_back(cat(i, 'G'));
    }
}

void init() {
	for (char i = 'D'; i <= 'G'; i++)
    {
    	for (char j = 'd'; j <= 'g'; j++)
            tileSequenceForChaos1.push_back(cat(i, j));
    	for (char j = 'c'; j >= 'a'; j--)
            tileSequenceForChaos1.push_back(cat(i, j));
    }

    for (char i = 'C'; i >= 'A'; i--)
    {
    	for (char j = 'c'; j >= 'a'; j--)
            tileSequenceForChaos1.push_back(cat(i, j));
    	for (char j = 'd'; j <= 'g'; j++)
            tileSequenceForChaos1.push_back(cat(i, j));
    }

    for (char i = 'D'; i <= 'G'; i++)
    {
    	for (char j = 'd'; j <= 'g'; j++)
            tileSequenceForOrder1.push_back(cat(i, j));
        for (char j = 'c'; j >= 'a'; j--)
            tileSequenceForOrder1.push_back(cat(i, j));
    }

    for (char i = 'C'; i >= 'A'; i--)
    {
    	for (char j = 'd'; j <= 'g'; j++)
            tileSequenceForOrder1.push_back(cat(i, j));
    	for (char j = 'c'; j >= 'a'; j--)
            tileSequenceForOrder1.push_back(cat(i, j));
    }
}

int dx[] = {1, 0, -1, 0};
int dy[] = {0, 1, 0, -1};

int _OwO_(const char& i, const char& j)
{
    int currentColour = board[i][j];
    string tile = cat(i, j);

    int cnt = 0;

    int searchRadius = 3;

    vector<string> visitableTiles;
    visitableTiles.push_back(tile);

    for (char column = j + 1; column <= 'g' && board[i][column] == 0 && column <= j + searchRadius; column++)
        visitableTiles.push_back(cat(i, column));
    for (char column = j - 1; column >= 'a' && board[i][column] == 0 && column >= j - searchRadius; column--)
        visitableTiles.push_back(cat(i, column));
    for (char row = i + 1; row <= 'G' && board[row][j] == 0 && row <= i + searchRadius; row++)
        visitableTiles.push_back(cat(row, j));
    for (char row = i - 1; row >= 'A' && board[row][j] == 0 && row >= i - searchRadius; row--)
        visitableTiles.push_back(cat(row, j));

    for (auto tile : visitableTiles)
    {
        char curr_i = tile[0];
        char curr_j = tile[1];

        for (int k = 0; k < 4; k++)
        {
            int next_i = curr_i + 2 * dx[k];
            int next_j = curr_j + 2 * dy[k];
            if (isOutOfBound(next_i, next_j)) continue;
            if (board[next_i][next_j] == currentColour) cnt += 1;
        }
    }

    return cnt;
}

int getBestMoveForChaos(string& bestContinuation, const string& colour, const int& depth, const int& maxDepth, const int& currentDelta)
{
    int deltaInCaseOfBestMove = INT_MAX;
    int evaluationInCaseOfBestMove = INT_MIN;
    for (auto tile : tileSequenceForChaos1)
    {
        char i = tile[0];
        char j = tile[1];

        if (board[i][j]) continue;

        string myMove = colour + tile;

        int curr = getScoreInRow(i) + getScoreInColumn(j);

        place(myMove);

        int next = getScoreInRow(i) + getScoreInColumn(j);

        int delta = currentDelta + next - curr;
        string temp;
        if (depth < maxDepth)
            delta = getBestMoveForOrder(temp, colour, depth + 1, maxDepth, delta);

        int evaluation = - delta;
        if (depth == 1) evaluation -= _OwO_(i, j);
        if (evaluation > evaluationInCaseOfBestMove)
        {
            bestContinuation = myMove;
            evaluationInCaseOfBestMove = evaluation;
            deltaInCaseOfBestMove = delta;
        }

        undoPlace(myMove);
    }
    return deltaInCaseOfBestMove;
}

int getBestMoveForOrder(string& bestContinuation, const string& colour, const int& depth, const int& maxDepth, const int& currentDelta)
{
    float deltaInCaseOfBestMove = INT_MIN;
    float evaluationInCaseOfBestMove = INT_MIN;
    for (auto tile : tileSequenceForOrder1)
    {
        char i = tile[0];
        char j = tile[1];

        if (!board[i][j]) continue;

        stack<string> legalMoves;
        legalMoves.push(tile + tile);

        int searchRadius = 4;
        if (depth > 2) searchRadius = 2;

        for (char column = j + 1; column <= 'g' && board[i][column] == 0 && column <= j + searchRadius; column++)
            legalMoves.push(tile + cat(i, column));
        for (char column = j - 1; column >= 'a' && board[i][column] == 0 && column >= j - searchRadius; column--)
            legalMoves.push(tile + cat(i, column));
        for (char row = i + 1; row <= 'G' && board[row][j] == 0 && row <= i + searchRadius; row++)
            legalMoves.push(tile + cat(row, j));
        for (char row = i - 1; row >= 'A' && board[row][j] == 0 && row >= i - searchRadius; row--)
            legalMoves.push(tile + cat(row, j));

        while (!legalMoves.empty())
        {
            string myMove = legalMoves.top();
            legalMoves.pop();

            int curr = getScoreInRow(myMove[0]) + getScoreInColumn(myMove[1])
                    + getScoreInRow(myMove[2]) + getScoreInColumn(myMove[3]);

            slide(myMove);

            int next = getScoreInRow(myMove[0]) + getScoreInColumn(myMove[1])
                       + getScoreInRow(myMove[2]) + getScoreInColumn(myMove[3]);

            float delta = currentDelta + next - curr;
            if (depth < maxDepth)
            {
                float expectedDelta = 0;
                string temp;
                int tilesLeft = 0;
                for (int nextColour = 1; nextColour <= 7; nextColour++)
                {
                    if (!tileCountOfColour[nextColour]) continue;

                    int deltaIfThisColour = getBestMoveForChaos(temp, to_string(nextColour), depth + 1, maxDepth, delta);

                    expectedDelta += tileCountOfColour[nextColour] * deltaIfThisColour;
                    tilesLeft += tileCountOfColour[nextColour];
                }
                if (tilesLeft == 0) break;
                expectedDelta /= tilesLeft;
                delta = expectedDelta;
            }

            int evaluation = delta; // + _OwO_(i, j);
            if (evaluation > evaluationInCaseOfBestMove)
            {
                bestContinuation = myMove;
                evaluationInCaseOfBestMove = evaluation;
                deltaInCaseOfBestMove = delta;
            }

            undoSlide(myMove);
        }
    }
    return deltaInCaseOfBestMove;
}
