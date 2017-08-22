#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAXS 19683
#define LOS_POS 	(1<<0)
#define TIE_POS 	(1<<1)
#define WIN_POS 	(1<<2)

//dynamic programming, optimal next state, min precedence
int dp[MAXS][2], nxt[MAXS][2], prec[MAXS][2];

//given a board, build the mask
int board2mask(int board[3][3]) {
	int mask = 0;
	for(int i=2; i>=0; i--) {
		for(int j=2; j>=0; j--) {
			mask *= 3;
			mask += board[i][j];
		}
	}
	return mask;
}

//given a mask, build the board
void mask2board(int mask, int board[3][3]) {
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			board[i][j] = mask%3;
			mask /= 3;
		}
	}
}

//current board global variable
int board[3][3];

int check() {
	//check lines
	for(int i=0; i<3; i++) {
		if (board[i][0] == 0) continue;
		bool found = true;
		for(int j=1; j<3 && found; j++) {
			if (board[i][j] != board[i][0]) found = false;
		}
		if (found) return board[i][0];
	}
	
	//check columns
	for(int j=0; j<3; j++) {
		if (board[0][j] == 0) continue;
		bool found = true;
		for(int i=1; i<3 && found; i++) {
			if (board[i][j] != board[0][j]) found = false;
		}
		if (found) return board[0][j];
	}
	
	//check main diagonal
	if (board[0][0] != 0) {
		bool found = true;
		for(int i=1; i<3 && found; i++) {
			if (board[i][i] != board[0][0]) found = false;
		}
		if (found) return board[0][0];
	}
	
	//check secondary diagonal
	if (board[0][2] != 0) {
		bool found = true;
		for(int i=1; i<3 && found; i++) {
			if (board[i][2-i] != board[0][2]) found = false;
		}
		if (found) return board[0][2];
	}
	
	//check if game is tied
	int counter = 0;
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			if (board[i][j] == 0) counter++;
		}
	}
	if (counter == 0) return 0;
	
	//not finished
	return -1;
}

void precompute() {
	
	//truly random moves
	srand(time(NULL));
	
	//precedence, a situation with less precedence is more desirable
	//situation refers to the enemy's next move, a losing position for him is desirable
	int precedence[10];
	precedence[LOS_POS] 					= 0;
	precedence[LOS_POS | TIE_POS] 			= 1;
	precedence[TIE_POS] 					= 2;
	precedence[LOS_POS | TIE_POS | WIN_POS] = 3;
	precedence[LOS_POS | WIN_POS] 			= 4;
	precedence[TIE_POS | WIN_POS] 			= 5;
	precedence[WIN_POS] 					= 6;
	
	//possible next states with minimum precedence
	int possibilities[10], npos, myprec;
	
	for(int mask=MAXS-1; mask >=0; mask--) {
		
		//build current board and get current situation
		mask2board(mask, board);
		//1 = player 1 win, 2 = player 2 win, 0 = tie, -1 = ongoing
		int finalSituation = check();
		
		//turn 0 = player 1, turn 1 = player 2
		for(int turn = 0; turn < 2; turn++) {
			nxt[mask][turn] = -1;
			prec[mask][turn] = -1;
			
			//game is over
			if (finalSituation >= 0) {
				if (finalSituation == 0) dp[mask][turn] = TIE_POS;
				else if (finalSituation == turn+1) dp[mask][turn] = WIN_POS;
				else dp[mask][turn] = LOS_POS;
			}
			
			//game ongoing
			else {
				dp[mask][turn] = 0;
				myprec = 7;
				npos = 0;
				for(int i=0; i<3; i++) {
					for(int j=0; j<3; j++) {
						
						//field not empty, not valid move
						if (board[i][j] != 0) continue;
						
						//get next state
						board[i][j] = 1+turn;
						int newmask = board2mask(board);
						board[i][j] = 0;
						
						//update my precedence and add move possibility
						int noptions = dp[newmask][1-turn];
						if (myprec > precedence[noptions]) {
							npos = 0;
							myprec = precedence[noptions];
							dp[mask][turn] = 0;
							if (noptions & LOS_POS) dp[mask][turn] |= WIN_POS;
							if (noptions & TIE_POS) dp[mask][turn] |= TIE_POS;
							if (noptions & WIN_POS) dp[mask][turn] |= LOS_POS;
						}
						if (myprec == precedence[noptions]) {
							possibilities[npos++] = newmask;
						}
					}
				}
				
				//pick a random possibility between the moves with minimum precedence
				nxt[mask][turn] = possibilities[rand()%npos];
				prec[mask][turn] = myprec;
			}
		}
	}
}

int gameBoard[3][3];

//reads input from human
void getHumanInput(int * row, int * col) {
	printf("You are X. Please insert your next move's row and column:\n");
	while(scanf("%d %d", row, col) != 2 || *row < 0 || *row > 2 || *col < 0 || *col > 2 || gameBoard[*row][*col] != 0) {
		fflush(stdin);
		printf("Invalid input. Please insert your next move's row and column:\n");
	}
}

void printGameState(int turn, bool over) {
	
	printf("================================================================\n");
	printf("%s game board. Turn: %s\n\n", over ? "Final" : "Current", turn == 0 ? "Human" : "Machine");
	
	//before line 0
	printf("\t   0 1 2\n");
	printf("\t  %c%c%c%c%c%c%c\n", 201, 205, 203, 205, 203, 205, 187);
	
	//line 0
	printf("\t0 %c", 186);
	for(int j=0; j<3; j++) printf("%c%c", gameBoard[0][j] == 0 ? ' ' : gameBoard[0][j] == 1 ? 'X' : 'O', 186);
	printf("\n\t  %c%c%c%c%c%c%c\n", 204, 205, 206, 205, 206, 205, 185);
	
	//line 1
	printf("\t1 %c", 186);
	for(int j=0; j<3; j++) printf("%c%c", gameBoard[1][j] == 0 ? ' ' : gameBoard[1][j] == 1 ? 'X' : 'O', 186);
	printf("\n\t  %c%c%c%c%c%c%c\n", 204, 205, 206, 205, 206, 205, 185);
	
	//line 2
	printf("\t2 %c", 186);
	for(int j=0; j<3; j++) printf("%c%c", gameBoard[2][j] == 0 ? ' ' : gameBoard[2][j] == 1 ? 'X' : 'O', 186);
	printf("\n\t  %c%c%c%c%c%c%c\n\n", 200, 205, 202, 205, 202, 205, 188);
}

int game(int turn) {
	
	//precompute states each time a game start to get other random moves
	precompute();
	int currentState = 0, nturns = 0;
	
	do {
		//get and print current game board
		mask2board(currentState, gameBoard);
		printGameState(turn, false);
		//printf("precedence = %d\n", prec[currentState][turn]);
		
		//human's turn
		if (turn == 0) {
			int i, j;
			getHumanInput(&i, &j);
			gameBoard[i][j] = turn+1;
			currentState = board2mask(gameBoard);
			turn = 1;
		}
		
		//machine's turn
		else {
			currentState = nxt[currentState][turn];
			turn = 0;
		}
		
		nturns++;
		
	} while (nxt[currentState][turn] >= 0);
	
	//Game over message
	mask2board(currentState, gameBoard);
	printGameState(currentState, true);
	
	//game ended at human's play
	if (turn == 0) {
		if (dp[currentState][turn] == WIN_POS) printf("VICTORY! Congratulations!\n\n");
		if (dp[currentState][turn] == LOS_POS) printf("DEFEAT! Better luck next time!\n\n");
		if (dp[currentState][turn] == TIE_POS) printf("THAT'S A TIE! Play again!\n\n");
	}
	
	//game ended at machine's play
	if (turn == 1) {
		if (dp[currentState][turn] == LOS_POS) printf("VICTORY! Congratulations!\n\n");
		if (dp[currentState][turn] == WIN_POS) printf("DEFEAT! Better luck next time!\n\n");
		if (dp[currentState][turn] == TIE_POS) printf("THAT'S A TIE! Play again!\n\n");
	}
	
}

int main() {
	
	bool newgame = false;
	printf("Welcome to Tic Tac ToITA\nBy Lucas Franca de Oliveira, COMP-18.\n\n");
	char choice;
	
	do {
		//read if player wants to start
		printf("Would you like to start (Y/N)?\n");
		while(scanf(" %c", &choice), choice != 'N' && choice != 'n' && choice != 'y' && choice != 'Y');
		if (choice >= 'A' && choice <= 'Z') choice += 'a' - 'A';
		
		//run game
		game(choice == 'y' ? 0 : 1);
		
		//ask if want to play again
		printf("Would you like to play again (Y/N)?\n");
		while(scanf(" %c", &choice), choice != 'N' && choice != 'n' && choice != 'y' && choice != 'Y');
		if (choice >= 'A' && choice <= 'Z') choice += 'a' - 'A';
		newgame = (choice == 'y');
		
	} while (newgame);
	
	printf("\nThank you for playing.\nBy Lucas Franca de Oliveira, COMP-18.\n");
	
	return 0;
}