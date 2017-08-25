#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAXS 19683
#define LOS_POS -1
#define TIE_POS 0
#define WIN_POS 1
#define TYPE_HUMAN 0
#define TYPE_HEURISTIC 1
#define TYPE_COMPLETE 2
#define INF (1<<30)

//dynamic programming, optimal next state, winning possibilities
int dp[MAXS][2], nxt[MAXS][2], winbal[MAXS][2];

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

//returns index of player if someone won, 0 if game ended in tie, -1 if game has not ended
int check(int board[3][3]) {
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

void completeSearch() {
	
	//truly random moves
	srand(time(NULL));
	
	//possible next states with minimum value
	int possibilities[10], npos;
	int board[3][3];
	
	for(int mask=MAXS-1; mask >=0; mask--) {
		
		//build current board and get current situation
		mask2board(mask, board);
		//1 = player 1 win, 2 = player 2 win, 0 = tie, -1 = ongoing
		int finalSituation = check(board);
		
		//turn 0 = player 1, turn 1 = player 2
		for(int turn = 0; turn < 2; turn++) {
			nxt[mask][turn] = -1;
			
			//game is over
			if (finalSituation >= 0) {
				if (finalSituation == 0) {
					dp[mask][turn] = TIE_POS;
					winbal[mask][turn] = 0;
				}
				else if (finalSituation == turn+1) {
					dp[mask][turn] = WIN_POS;
					winbal[mask][turn] = 1;
				}
				else {
					dp[mask][turn] = LOS_POS;
					winbal[mask][turn] = -1;
				}
			}
			
			//random move at starting position, make it more fun since it will never lose
			else if (mask == 0) {
				dp[mask][turn] = TIE_POS;
				npos = rand()%9;
				int i = npos/3, j = npos%3;
				board[i][j] = turn + 1;
				nxt[mask][turn] = board2mask(board);
				board[i][j] = 0;
				winbal[mask][turn] = 0;
			}
			
			//game ongoing
			else {
				dp[mask][turn] = 2;
				npos = 0;
				winbal[mask][turn] = 0;
				int minbal = INF;
				for(int i=0; i<3; i++) {
					for(int j=0; j<3; j++) {
						
						//field not empty, not valid move
						if (board[i][j] != 0) continue;
						
						//get next state
						board[i][j] = 1+turn;
						int newmask = board2mask(board);
						board[i][j] = 0;
						
						//update my winning balance and add move possibility
						winbal[mask][turn] -= winbal[newmask][1-turn];
						if (dp[mask][turn] > dp[newmask][1-turn] ||
							(dp[mask][turn] == dp[newmask][1-turn] && minbal > winbal[newmask][1-turn])) {
							npos = 0;
							dp[mask][turn] = dp[newmask][1-turn];
							/*if (minbal > winbal[newmask][1-turn])*/ minbal = winbal[newmask][1-turn];
						}
						if (dp[mask][turn] == dp[newmask][1-turn] && minbal == winbal[newmask][1-turn]) {
							possibilities[npos++] = newmask;
						}
					}
				}
				
				//pick a random possibility between the moves with minimum precedence
				dp[mask][turn] = -dp[mask][turn];
				nxt[mask][turn] = possibilities[rand()%npos];
			}
		}
	}
}

//given a board and a player, returns how many sets are still possible for that player
int possibleWinningSets(int board[3][3], int turn) {	//turn 1-indexed
	int answer = 0;
	bool found;
	
	//check lines
	for(int i=0; i<3; i++) {
		found = true;
		for(int j=0; j<3 && found; j++) {
			if (board[i][j] == 3-turn) found = false;
		}
		if (found) answer++;
	}
	
	//check columns
	for(int j=0; j<3; j++) {
		found = true;
		for(int i=0; i<3 && found; i++) {
			if (board[i][j] == 3-turn) found = false;
		}
		if (found) answer++;
	}
	
	//check main diagonal
	found = true;
	for(int i=0; i<3 && found; i++) {
		if (board[i][i] == 3-turn) found = false;
	}
	if (found) answer++;
	
	//check secondary diagonal
	found = true;
	for(int i=0; i<3 && found; i++) {
		if (board[i][2-i] == 3-turn) found = false;
	}
	if (found) answer++;
	
	//return answer
	return answer;
}

//uses number of winning states and number of losing states as heuristic
int getNextState(int mask, int turn) {	//turn 0-indexed
	int board[3][3], bestmask = -1, balance, newmask, bestbalance = -INF;
	
	//get the board from mask
	mask2board(mask, board);
	
	//test all reachable states
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			if (board[i][j] != 0) continue;
			board[i][j] = 1+turn;
			newmask = board2mask(board);
			
			//heuristic: number of possible winning sets for me minus number of possible winning sets for oponent
			balance = possibleWinningSets(board, 1+turn) - possibleWinningSets(board, 2-turn);
			board[i][j] = 0;
			
			//update best choice
			if (balance > bestbalance) {
				bestbalance = balance;
				bestmask = newmask;
			}
		}
	}
	
	//return best possible next state
	return bestmask;
}

//reads input from human
void getHumanInput(int * row, int * col, int gameBoard[3][3], int turn) {	//turn 0-indexed
	printf("You are %c. Please insert your next move's row and column:\n", turn == 0 ? 'X' : 'O');
	while(scanf("%d %d", row, col) != 2 || *row < 0 || *row > 2 || *col < 0 || *col > 2 || gameBoard[*row][*col] != 0) {
		fflush(stdin);
		printf("Invalid input. Please insert your next move's row and column:\n");
	}
}

//prints current game state
void printGameState(int turn, bool over, int gameBoard[3][3]) {
	
	printf("================================================================\n");
	printf("%s game board. Turn: %s\n\n", over ? "Final" : "Current", turn == TYPE_HUMAN ? "Human" : "Machine");
	
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

int game(const int turnTypes[2]) {
	
	//completeSearch states each time a game start to get other random moves
	completeSearch();
	int currentState = 0, nturns = 0;
	int gameBoard[3][3], turn = 0;
	
	do {
		//get and print current game board
		mask2board(currentState, gameBoard);
		printGameState(turnTypes[turn], false, gameBoard);
		
		//human's turn
		if (turnTypes[turn] == TYPE_HUMAN) {
			int i, j;
			getHumanInput(&i, &j, gameBoard, turn);
			gameBoard[i][j] = 1+turn;
			currentState = board2mask(gameBoard);
		}
		
		//complete search machine's turn
		if (turnTypes[turn] == TYPE_COMPLETE) {
			currentState = nxt[currentState][turn];
		}
		
		//heuristic search machine's turn
		if (turnTypes[turn] == TYPE_HEURISTIC) {
			currentState = getNextState(currentState, turn);
		}
		
		nturns++;
		turn ^= 1;
		
	} while (nxt[currentState][turn] >= 0);
	
	//game over message
	mask2board(currentState, gameBoard);
	printGameState(turnTypes[turn], true, gameBoard);
	
	//print final game message
	if (dp[currentState][turn] == WIN_POS) printf("VICTORY for player %d!\n\n", 1+turn);
	if (dp[currentState][turn] == LOS_POS) printf("VICTORY for player %d!\n\n", 2-turn);
	if (dp[currentState][turn] == TIE_POS) printf("TIE!\n\n");
	
}

int main() {
	
	bool newgame = false;
	printf("Welcome to Tic Tac ToITA\nBy Lucas Franca de Oliveira, COMP-18.\n\n");
	char choice;
	int turnTypes[2], t1, t2;
	
	do {
		
		//reads game type
		printf("Please input type of game players (two numbers):\n%d = human, %d = heuristic machine, %d = complete search machine\n",
			TYPE_HUMAN, TYPE_HEURISTIC, TYPE_COMPLETE);
		while(scanf("%d %d", &t1, &t2) != 2 || t1 < 0 || t1 > 2 || t2 < 0 || t2 > 2) {
			fflush(stdin);
			printf("Invalid input. Please input type of game players (two numbers):\n");
		}
		turnTypes[0] = t1;
		turnTypes[1] = t2;
		
		//run game
		game(turnTypes);
		
		//ask if want to play again
		printf("Would you like to play again (Y/N)?\n");
		while(scanf(" %c", &choice), choice != 'N' && choice != 'n' && choice != 'y' && choice != 'Y');
		if (choice >= 'A' && choice <= 'Z') choice += 'a' - 'A';
		newgame = (choice == 'y');
		
	} while (newgame);
	
	printf("\nThank you for playing.\nBy Lucas Franca de Oliveira, COMP-18.\n");
	
	return 0;
}