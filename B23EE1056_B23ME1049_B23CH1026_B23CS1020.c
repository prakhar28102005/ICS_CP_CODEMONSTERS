#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_PLAYERS 4
#define BOARD_SIZE 100
#define NUM_SNAKES 9
#define NUM_LADDERS 9
#define NAME_LENGTH 20

// Structure to represent a player
struct Player
{
    char *name;
    int position;
    int index;
    int hasWon;
    int rank;
};

// Structure to represent a snake or ladder
struct SnakeOrLadder
{
    int start;
    int end;
    char message[200];
};

// Structure to store dice and its corresponding rolls
struct DiceRolls
{
    int *roll;
    int *position;
    int index;
};

void getPlayerNames(int numPlayers, struct Player *players)
{
    for (int i = 0; i < numPlayers; i++)
    {
        players[i].hasWon = 0; // initializing variable value to keep track of winner player

        players[i].name = (char *)malloc(NAME_LENGTH * sizeof(char));

        if (players[i].name == NULL)
        {
            printf("Memory allocation failed. Exiting...\n");
            exit(1);
        }

        printf("Enter the name for Player %d: ", i + 1);
        scanf("%s", players[i].name);
      
        players[i].position = 0;
        players[i].index = i + 1;
    }
}


void freeMemory(struct Player *players, int numPlayers)
{
    for (int i = 0; i < numPlayers; i++)
    {
        free(players[i].name);
    }
}

int rollDie()
{
    int randomValue = rand() % 100;
    if (randomValue < 12) // Creating a biased die
    {
        return 6;
    }
    else
        return rand() % 5 + 1;
}

int movePlayer(struct Player *player, int diceRoll, struct SnakeOrLadder *snakes, struct SnakeOrLadder *ladders)
{
    int originalPosition = player->position;

    // If no snake or ladder, update position
    player->position += diceRoll;

    return player->position;
}

int checkWinner(struct Player *player, int diceRoll)
{

    if (player->position == BOARD_SIZE)
    {
        printf("%s wins!\n", player->name);
        player->hasWon = 1;
        return 1;
    }

    else if (player->position > BOARD_SIZE)
    {
        printf("%s overshoots! Please try again.\n", player->name);
        player->position -= diceRoll;
        printf("%s's position is now %d", player->name, player->position);
    }

    return 0;
}

void checkForSnake(struct Player *player, struct SnakeOrLadder *snakes)
{
    for (int i = 0; i < NUM_SNAKES; i++)
    {
        if (player->position == snakes[i].start)
        {
            printf("%s got bitten by a snake! %s moved from %d to %d. %s\n",
                   player->name, player->name, player->position, snakes[i].end, snakes[i].message);
            player->position = snakes[i].end;
            break;
        }
    }
}

void checkForLadder(struct Player *player, struct SnakeOrLadder *ladders)
{
    for (int i = 0; i < NUM_LADDERS; i++)
    {
        if (player->position == ladders[i].start)
        {
            player->position = ladders[i].end;
            printf("%s climbed a ladder! %s\n",
                   player->name, ladders[i].message);
            break;
        }
    }
}

// Function to store the previous 3 dice rolls and corresponding positions
void storing(struct DiceRolls *storage, int currentRoll, int currentPosition)
{
    // Shift the previous values
    for (int i = 0; i < 2; i++)
    {
        storage->roll[i] = storage->roll[i + 1];
        storage->position[i] = storage->position[i + 1];
    }

    // Store the current values
    storage->roll[2] = currentRoll;
    storage->position[2] = currentPosition;
}

// Function to initialize the DiceRolls structure
void initDiceRolls(struct DiceRolls *storage)
{
    storage->roll = (int *)calloc(3, sizeof(int));
    storage->position = (int *)calloc(3, sizeof(int));

    if (storage->roll == NULL || storage->position == NULL)
    {
        printf("Memory allocation failed. Exiting...\n");
        exit(1);
    }
}

void freeDiceRolls(struct DiceRolls *storage)
{
    free(storage->roll);
    free(storage->position);
}

void displayMessage(struct Player *players, int numPlayers, struct SnakeOrLadder *snakes, struct SnakeOrLadder *ladders, int currentPlayer)
{

    printf("\033[4m\nMessages:\n\033[0m");
    for (int i = 0; i < NUM_SNAKES; i++)
    {
        for (int j = 0; j < numPlayers; j++)
        {
            if (players[currentPlayer].position == snakes[i].start)
            {
                printf("Snake%d: %s\n", i + 1, snakes[i].message);
            }
        }
    }

    for (int i = 0; i < NUM_LADDERS; i++)
    {
        if (players[currentPlayer].position == ladders[i].start)
        {
            printf("Ladder%d: %s\n", i + 1, ladders[i].message);
        }
    }
}

// Function to print the current board
void printBoard(struct Player *players, int numPlayers, struct SnakeOrLadder *snakes, struct SnakeOrLadder *ladders, int currentPlayer, int diceRoll)
{
    printf("\nSnakes and Ladders Board:\n");

    int num;

    printf("\n|---+---+---+---+---+---+---+---+---+---|\n");
    for (int i = 9; i >= 0; i--)
    {
        for (int j = 1; j <= 10; j++)
        {
            if (i % 2 == 0)
            {
                num = 10 * i + j;
            }
            else
            {
                num = 10 * (i + 1) - j + 1;
            }
            int playersAtPosition[MAX_PLAYERS]; // Array to store players at the current position
            int numPlayersAtPosition = 0;

            // Check which players are at the current position
            for (int k = 0; k < numPlayers; k++)
            {
                if (players[k].position == num)
                {
                    playersAtPosition[numPlayersAtPosition++] = k;
                }
            }

            if (numPlayersAtPosition > 0 && num != 100)
            {
                // Display all players at the position
                printf("|");
                for (int k = 0; k < numPlayersAtPosition; k++)
                {
                    printf("P%d ", players[playersAtPosition[k]].index);

                    if (k < numPlayersAtPosition - 1)
                    {
                        printf("&");
                    }
                }
            }
            else
            {
                int foundSnakeOrLadder = 0;
                for (int k = 0; k < NUM_SNAKES; k++)
                {
                    if (snakes[k].start == num)
                    {
                        printf("|S%d ", k + 1);
                        foundSnakeOrLadder = 1;
                        break;
                    }
                    else if (snakes[k].end == num)
                    {
                        printf("|S%d'", k + 1);
                        foundSnakeOrLadder = 1;
                        break;
                    }
                }

                for (int k = 0; k < NUM_LADDERS; k++)
                {
                    if (ladders[k].start == num)
                    {
                        printf("|L%d ", k + 1);
                        foundSnakeOrLadder = 1;
                        break;
                    }
                    else if (ladders[k].end == num)
                    {
                        printf("|L%d'", k + 1);
                        foundSnakeOrLadder = 1;
                        break;
                    }
                }

                // If neither snake nor ladder, print the position number
                if (!foundSnakeOrLadder)
                {
                    if (num < 10)
                    {
                        printf("| %d ", num);
                    }
                    else if (num < 100)
                    {
                        printf("|%d ", num);
                    }
                    else
                    {
                        printf("|%d", num);
                    }
                }
            }
        }

        printf("|\n|---+---+---+---+---+---+---+---+---+---|\n");
    }

    for (int i = 0; i < numPlayers; i++)
    {
        if (players[i].position <= BOARD_SIZE)
        {
            printf("%s is at position %d.\n", players[i].name, players[i].position);
        }
        else if (players[i].position > BOARD_SIZE)
        {
            printf("%s is at position %d.\n", players[i].name, (players[i].position - diceRoll));
        }
    }
}

// Function to perform a player's turn
void performTurn(struct Player *player, struct SnakeOrLadder *snakes, struct SnakeOrLadder *ladders, struct Player *players, int numPlayers, int diceRoll, struct DiceRolls *storage, int *numWinners, int *rankCursor)
{
    int currentPlayerIndex = -1;

    for (int i = 0; i < numPlayers; i++)
    {
        if (strcmp(player->name, players[i].name) == 0)
        {
            currentPlayerIndex = i;
            break;
        }
    }

    if (currentPlayerIndex == -1)
    {
        printf("Error: Current player not found.\n");
        exit(1);
    }
    if (storage->roll[2] != 6)
    {
        printf("\n%s, press Enter to roll the die.\n", player->name);
    }
    else if (storage->roll[2] == 6)
    {
        printf("\n%s, press Enter to roll the die again! You got 6 in the previous turn\n", player->name);
    }

    // Check if the last three rolls were all 6s
    if (storage->roll[0] == 6 && storage->roll[1] == 6 && storage->roll[2] == 6)
    {
        printf("%s got three 6s in a row! Resetting values and moving to the next player.\n", player->name);

        // Resetting storage values of dice
        memset(storage->roll, 0, 3 * sizeof(int));
        memset(storage->position, 0, 3 * sizeof(int));

        // Setting player position to 18 steps back
        player->position -= 18;
        printf("%s is at position: %d now!", player->name, player->position);

        return;
    }

    getchar();

    printf("%s rolled a %d.\n", player->name, diceRoll);

    int originalPosition = player->position;
    player->position = movePlayer(player, diceRoll, snakes, ladders);

    checkForSnake(player, snakes);
    checkForLadder(player, ladders);

    // storing the current dice roll and player position
    storing(storage, diceRoll, player->position);

    printBoard(players, numPlayers, snakes, ladders, currentPlayerIndex, diceRoll);

    displayMessage(players, numPlayers, snakes, ladders, currentPlayerIndex);

    if (checkWinner(player, diceRoll))
    {
        printf("%s wins!\n", player->name);
        (*numWinners)++;
        player->rank = (*rankCursor)++;
    }
}

void rankSort(struct Player *players, int numPlayers)
{
    for (int i = 0; i < numPlayers - 1; i++)
    {
        for (int j = 0; j < numPlayers - i - 1; j++)
        {
            if (players[j].rank > players[j + 1].rank)
            {
                struct Player temp = players[j];
                players[j] = players[j + 1];
                players[j + 1] = temp;
            }
        }
    }
}

int main()
{
    srand(time(NULL));

   printf("-------------\033[1m\033[3m\033[4mWELCOME\033[0m-------------\n");
    printf("-----------\033[1m\033[3m\033[4mINSTRUCTIONS\033[0m------------\n\n");
    printf("1. Enter the name in 1 word, under length of 20\n");
    printf("2. You cannot play with player count as 1\n");
    printf("3. You gain a chance to roll die again when you roll a 6\n");
    printf("4. 3 Sixes in a row will lead to resetting your position\n");
    printf("5. Ladders start position is marked as L1 tile and its end marked as L1'\n");
    printf("6. Snakes start position is marked as S1 tile and its end marked as S1'\n");
    printf("\n-------------ENJOY!!-------------\n\n");

    struct SnakeOrLadder snakes[NUM_SNAKES] = {
        {16, 6, "Oops! You got bitten by a snake and moved down to a lower position from 16 to 6."},
        {47, 26, "Uh-oh! A snake sent you sliding down from 47 to 26 on the board."},
        {49, 11, "Yikes! A snake got you, and you slid down to 11 from 49."},
        {56, 53, "A snake caught you off guard! You're moving down now. You went from 56 to 53."},
        {62, 19, "You encountered a snake! It sent you sliding down from 62 to 19."},
        {87, 24, "A snake bit you, and now you're at a lower position."},
        {93, 73, "Bitten by a snake! You moved down to 73 from 93."},
        {95, 69, "A snake got you, and you slid down to a lower position. Journey from 95 to 69 is harsh"},
        {98, 3, "Watch out! A snake moved you down to a damn low position, from 98 to 3."}};

    struct SnakeOrLadder ladders[NUM_LADDERS] = {
        {4, 38, "Great! You climbed a ladder from position 4 to 38 and moved to a higher position."},
        {8, 14, "You found a ladder from position 8 to 14! It took you to a higher spot on the board."},
        {9, 31, "Climbing a ladder from position 9 to 31! You moved to a higher position."},
        {21, 42, "You climbed a ladder from position 21 to 42 and moved to a higher spot."},
        {28, 84, "Wow! You found a ladder from position 28 to 84 and moved to a much higher spot."},
        {36, 44, "Climbing a ladder from position 36 to 44! You moved to a higher position."},
        {51, 67, "You climbed a ladder from position 51 to 67 and moved up to a higher spot."},
        {71, 91, "Fantastic! You found a ladder from position 71 to 91 and moved to a higher position."},
        {80, 97, "Incredible! Climbing a ladder from position 80 to 97 took you to the top of the board."}};

    int numPlayers;
    printf("Enter the number of players (2 to %d): ", MAX_PLAYERS);
    scanf("%d", &numPlayers);
    printf("Don't enter any 2 same names.\n\n");

    if(numPlayers == 1){
        printf("You cannot play alone :(.\n");
        return 1;
    }
    if (numPlayers < 2 || numPlayers > MAX_PLAYERS)
    {
        printf("Invalid number of players. Exiting...\n");
        return 1;
    }

    struct Player *players = (struct Player *)malloc(MAX_PLAYERS * sizeof(struct Player));

    if (players == NULL)
    {
        printf("Memory allocation failed. Exiting...\n");
        return 1;
    }

    getPlayerNames(numPlayers, players);

    for (int i = 0; i < numPlayers - 1; i++)
    {
        for (int j = i + 1; j < numPlayers; j++)
        {
            if (strcmp(players[i].name, players[j].name) == 0)
            {
                printf("Two names can't be the same. Please restart the game!\n");
                freeMemory(players, numPlayers);
                free(players);
                return 1;
            }
        }
    }

    int currentPlayer = 0;

    struct DiceRolls storage;
    initDiceRolls(&storage);

    int numWinners = 0;
    int rankCursor = 1;

    printf("\n---------------\033[1m\033[3m\033[4mWARNING!!\033[0m---------------\n");
    for (int i = 0; i < NUM_SNAKES; i++)
    {
        printf("Snake%d starts from %d and drops you at %d\n", i + 1, snakes[i].start, snakes[i].end);
    }
    printf("\n\n");

    printf("-----------\033[1m\033[3m\033[4mLESSSGOOOOO!!\033[0m-----------\n");
    for (int i = 0; i < NUM_LADDERS; i++)
    {
        printf("Ladder%d starts from %d and will drop you at %d\n", i + 1, ladders[i].start, ladders[i].end);
    }

    printf("---------------------------------------------\n");

    while (numWinners < numPlayers  - 1)
    {
        int diceRoll = rollDie();

        if (!players[currentPlayer].hasWon)
        {
            performTurn(&players[currentPlayer], snakes, ladders, players, numPlayers, diceRoll, &storage, &numWinners, &rankCursor);

            if (diceRoll != 6)
            {
                currentPlayer = (currentPlayer + 1) % numPlayers; // Move to the next player
            }
        }
        else
        {
            currentPlayer = (currentPlayer + 1) % numPlayers; // Move to the next player if already won
        }
    }

    for (int i = 0; i < numPlayers; i++)
    {
        if (!players[i].hasWon)
        {
            players[i].rank = rankCursor;
        }
    }
    rankSort(players, numPlayers);

    printf("--*---*---*---*---*---*---*---*---*---*---*---*--");

    printf("\033[4m\n\t\tFinal Standings\n\n\033[0m");
    printf("\t\tRank\tPlayer\t\n");

    for (int i = 0; i < numPlayers; i++)
    {
        printf("\t\t");
        printf("%d\t%s\n", players[i].rank, players[i].name);
    }

    printf("--*---*---*---*---*---*---*---*---*---*---*---*--\n");

    printf("\033[4mMade by:\033[0m\n\n");
    printf("\033[3mPrakhar Srivastava - B23EE1056\n");
    printf("Prakhar Goyal - B23ME1049\n");
    printf("Manan Ajmera - B23CH1026\n");
    printf("Ishan Bhambare - B23CS1020\033[0m\n\n");
    printf("----------\033[4mThank You For Playing\033[0m!!----------\n");

    freeMemory(players, numPlayers);
    freeDiceRolls(&storage);
    free(players);

    return 0;
}