

#include <iostream>
#include <stack>
#include <tuple>
#include <string> 
#include <chrono>
#include <thread>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

#define N 9

using namespace std;


class SudokuGrid{
private:
    int grid[N][N]; //copie de la grille qui sera manipulée au cours du programme
    int originalGrid[N][N]; //copie persistente de la grille donnée qui sera utilisée pour réinitialiser la grille
    stack <tuple<int, int, int>> moveList; //UNDO() :pile contenant les derniers mouvements. Je priorise cette structure 
                                            //car elle est parfaite pour manipuler les derniers elements à 
                                            //cause de sa methode LIFO
                                            //J'utilise aussi les operations sur les <vector> qui me facilitent la manipultion des donnnees

public:
    //constructeur de la classe SudokuGrid
    SudokuGrid(int givenGrid[N][N]){
        for (int row = 0; row < N; row++){
            for (int col = 0; col < N; col++){
                grid[row][col] = givenGrid[row][col];
                originalGrid[row][col] = givenGrid[row][col];
            }
        }
    }

    void printGrid(){
        cout << "\n";
        cout << "      ";
        for (char col = 'A'; col <= 'I'; ++col) {
        cout << col << " "; 
        if ((col - 'A' + 1) % 3 == 0) {
            cout << "  "; 
            }
        } 
        cout << "\n\n";

        cout << "    +-------+-------+-------+\n";
        for (int i = 0; i < 9; i++){
            cout << i+1<< "   ";
            cout << "| ";
            for (int j = 0; j < 9; j++){
                if (grid[i][j] == 0)
                    cout << ". ";
                else
                    cout << grid[i][j] << " ";
                if ((j+1)% 3 == 0){
                    cout << "| ";
                }
            }
            cout << endl;
            if ((i+1) % 3 == 0){
                cout << "    +-------+-------+-------+\n";
            }
        }
    }


    //verifie si l'entree est valide
    bool isSafeMove(int num, int row, int col){
        //check if in column
        for (int i = 0; i < N; i++){
            if (grid[i][col] == num){
                return false;
            }
        }

        //check if in row
        for (int j = 0; j < N; j++){
            if (grid[row][j] == num){
                return false;
            }
        }

        //check if in 3x3 box
        int blockStartRow = (row/3)*3;
        int blockStartCol = (col/3)*3;
        
        for (row = 0; row < 3; row++){
            for (col = 0; col < 3; col++){
                if (grid[blockStartRow + row][blockStartCol + col] == num){
                    return false;
                }
            }
        }

        return true;
    }

    
    void makeMove(int row, int col, int num){
        //check if the case is already occupied
        if (grid[row][col] != 0){
            cout << "\nLa case est déjà occupée.\n";
            this_thread::sleep_for(std::chrono::seconds(1));
            return;
        }

        if (!isSafeMove(num, row, col)){
            cout << "\nMouvement invalide!"<< endl;
            this_thread::sleep_for(std::chrono::seconds(1));
            return;
        }

        moveList.push({row, col, grid[row][col]});
        grid[row][col] = num;
    }

    //verifier si la grille sudoku a ete completee avec succes
    bool isSolved() {
    // Check rows and columns
    for (int i = 0; i < N; i++) {
        bool rowCheck[N + 1] = {false};
        bool colCheck[N + 1] = {false};
        
        for (int j = 0; j < N; j++) {
            // Check the row
            if (grid[i][j] < 1 || grid[i][j] > 9 || rowCheck[grid[i][j]]) {
                return false;
            }
            rowCheck[grid[i][j]] = true;

            // Check the column
            if (grid[j][i] < 1 || grid[j][i] > 9 || colCheck[grid[j][i]]) {
                return false;
            }
            colCheck[grid[j][i]] = true;
        }
    }

    // Check each 3x3 subgrid
    for (int blockRow = 0; blockRow < 3; blockRow++) {
        for (int blockCol = 0; blockCol < 3; blockCol++) {
            bool blockCheck[N + 1] = {false};

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int num = grid[blockRow * 3 + i][blockCol * 3 + j];
                    if (num < 1 || num > 9 || blockCheck[num]) {
                        return false;
                    }
                    blockCheck[num] = true;
                }
            }
        }
    }

    return true;
}


    //reinitialise completement la grille
    void resetGrid(){
        for (int i = 0; i < N; i++){
            for (int j = 0; j < N; j++){
                grid[i][j] = originalGrid[i][j];
            }
        }
        while (!moveList.empty()){
            moveList.pop();
        }
    }

    //revenir au mouvement precedent
    void undoMove(){
        if (moveList.empty()){
            cout << "\nVous n'avez pas encore joué!\n";
            this_thread::sleep_for(std::chrono::seconds(1));
            return;
        }
        auto [row, col, prevMove] = moveList.top();
        moveList.pop();
        grid[row][col] = prevMove;
    }

};

void playGame(SudokuGrid& grid){
    string move;
    while (true){
        system(CLEAR);
        grid.printGrid();
        cout << "\nEntrez un mouvement (ex : B4 9), RESET, UNDO ou QUIT : ";
        getline(cin, move);

        if (move == "QUIT"){
            cout << "\nGOODBYE !!!" << endl;
            break;
        }
        else if(move == "RESET"){
            grid.resetGrid();
            cout << "\nRÉINITIALISATION DE LA GRILLE" << endl;
            this_thread::sleep_for(std::chrono::seconds(1));
        }
        else if(move == "UNDO"){
            grid.undoMove();
        }
        else{
            if (move.length() == 4 && move[2] == ' ') {
                int col = move[0] - 'a';
                int row = move[1] - '1';
                int num = move[3] - '0';

                if (col >= 0 && col < 9 && row >= 0 && row < 9 && num >= 1 && num <= 9) {

                    grid.makeMove(row, col, num);
                    
                    if (grid.isSolved()) {
                        cout << "\nFélicitations, vous avez résolu le Sudoku !\n";
                        this_thread::sleep_for(std::chrono::seconds(1));
                        break;
                    }  
                } 
                else {
                    cout << "\nIndices ou valeur hors limites. Vérifiez votre commande.\n";
                    this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
            else {
                cout << "\nFormat de commande invalide.  Vérifiez votre commande.\n";
                this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
}


int main(){
    int givenGrid[N][N] = {
        {0,0,0, 0,0,0, 0,0,0},
        {0,7,9, 0,5,0, 1,8,0},
        {8,0,0, 0,0,0, 0,0,7},

        {0,0,7, 3,0,6, 8,0,0},
        {4,5,0, 7,0,8, 0,9,6},
        {0,0,3, 5,0,2, 7,0,0},

        {7,0,0, 0,0,0, 0,0,5},
        {0,1,6, 0,3,0, 4,2,0},
        {0,0,0, 0,0,0, 0,0,0}
    };
    
    SudokuGrid sudoku(givenGrid);
    
    playGame(sudoku);

    return 0;
}