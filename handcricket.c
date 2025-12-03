/*
 * ========================================
 * ODD OR EVEN HAND CRICKET GAME
 * ========================================
 * A complete console-based hand cricket game
 * Written in pure C language
 * 
 * Compile: gcc handcricket.c -o handcricket
 * Run: ./handcricket
 * ========================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/* ==================== CONSTANTS ==================== */
#define MAX_HISTORY 100
#define EASY 1
#define MEDIUM 2
#define HARD 3

/* ==================== GLOBAL VARIABLES ==================== */
int player_score = 0;
int computer_score = 0;
int difficulty = EASY;
int prev_moves[MAX_HISTORY];
int move_count = 0;
int same_choice_count = 0;
int last_player_input = -1;
int second_phase = 0;

/* ==================== UTILITY FUNCTIONS ==================== */

/* Clear screen - works on both Windows and Unix */
void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Pause and wait for user input */
void pause_game(void) {
    printf("\nPress Enter to continue...");
    getchar();
}

/* Get a single character from user (flush buffer) */
char get_char_input(void) {
    char c;
    scanf(" %c", &c);
    while (getchar() != '\n'); /* Clear input buffer */
    return c;
}

/* Get integer input from user with validation */
int get_int_input(int min, int max) {
    int value;
    char buffer[100];
    
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%d", &value) == 1) {
                if (value >= min && value <= max) {
                    return value;
                }
            }
        }
        printf("Invalid input! Please enter a number between %d and %d: ", min, max);
    }
}

/* Print a decorative line */
void print_line(char c, int length) {
    for (int i = 0; i < length; i++) {
        putchar(c);
    }
    putchar('\n');
}

/* Print centered text */
void print_centered(const char *text, int width) {
    int len = strlen(text);
    int padding = (width - len) / 2;
    for (int i = 0; i < padding; i++) putchar(' ');
    printf("%s\n", text);
}

/* ==================== DISPLAY FUNCTIONS ==================== */

/* Display game header */
void display_header(void) {
    print_line('=', 50);
    print_centered("ODD OR EVEN HAND CRICKET GAME", 50);
    print_line('=', 50);
    printf("\n");
}

/* Display current scores */
void display_scores(void) {
    print_line('-', 50);
    printf("|  YOUR SCORE: %-5d  |  COMPUTER SCORE: %-5d |\n", 
           player_score, computer_score);
    print_line('-', 50);
}

/* Display difficulty level */
void display_difficulty(void) {
    printf("\n[Difficulty: ");
    switch (difficulty) {
        case EASY:   printf("EASY");   break;
        case MEDIUM: printf("MEDIUM"); break;
        case HARD:   printf("HARD");   break;
    }
    printf("]\n");
}

/* Display main menu */
void display_main_menu(void) {
    clear_screen();
    display_header();
    
    printf("\n");
    print_centered("MAIN MENU", 50);
    print_line('-', 50);
    printf("\n");
    printf("  1. Start New Game\n");
    printf("  2. Change Difficulty\n");
    printf("  3. How to Play\n");
    printf("  4. Exit\n");
    printf("\n");
    display_difficulty();
    print_line('=', 50);
    printf("\nEnter your choice (1-4): ");
}

/* Display how to play instructions */
void display_instructions(void) {
    clear_screen();
    display_header();
    
    printf("\n");
    print_centered("HOW TO PLAY", 50);
    print_line('-', 50);
    printf("\n");
    printf("1. TOSS: Choose HEAD or TAILS to win the toss.\n\n");
    printf("2. CHOICE: If you win, choose to BAT or BOWL first.\n\n");
    printf("3. GAMEPLAY:\n");
    printf("   - Enter a number between 0 and 10\n");
    printf("   - Computer also picks a number (0-10)\n");
    printf("   - If numbers match, the batsman is OUT!\n");
    printf("   - If batting: your number adds to your score\n");
    printf("   - If bowling: computer's number adds to its score\n\n");
    printf("4. INNINGS:\n");
    printf("   - After first innings, roles swap\n");
    printf("   - Batting second? Chase the target!\n");
    printf("   - Bowling second? Defend your score!\n\n");
    printf("5. SPECIAL RULES:\n");
    printf("   - If you pick 0, you get computer's number as runs\n");
    printf("   - Don't repeat same number 5 times (you'll be OUT!)\n\n");
    printf("6. DIFFICULTY LEVELS:\n");
    printf("   - EASY: Computer picks randomly\n");
    printf("   - MEDIUM: Computer sometimes predicts your moves\n");
    printf("   - HARD: Computer analyzes your patterns!\n\n");
    
    print_line('=', 50);
    pause_game();
}

/* Display difficulty menu */
void display_difficulty_menu(void) {
    clear_screen();
    display_header();
    
    printf("\n");
    print_centered("SELECT DIFFICULTY", 50);
    print_line('-', 50);
    printf("\n");
    printf("  1. EASY   - Computer plays randomly\n");
    printf("  2. MEDIUM - Computer sometimes predicts\n");
    printf("  3. HARD   - Computer analyzes patterns\n");
    printf("\n");
    printf("Current difficulty: ");
    switch (difficulty) {
        case EASY:   printf("EASY\n");   break;
        case MEDIUM: printf("MEDIUM\n"); break;
        case HARD:   printf("HARD\n");   break;
    }
    printf("\n");
    print_line('=', 50);
    printf("\nEnter your choice (1-3): ");
}

/* ==================== GAME LOGIC FUNCTIONS ==================== */

/* Generate computer's move based on difficulty */
int generate_computer_move(void) {
    int move;
    int i, freq[11] = {0};
    int max_freq, predicted;
    
    switch (difficulty) {
        case EASY:
            /* Random move */
            move = rand() % 11;
            break;
            
        case MEDIUM:
            /* 30% chance to copy last player move, otherwise random */
            if (rand() % 100 < 30 && move_count > 0) {
                move = prev_moves[move_count - 1];
            } else {
                move = rand() % 11;
            }
            break;
            
        case HARD:
            /* Analyze player's move history and predict */
            if (move_count == 0) {
                move = rand() % 11;
            } else {
                /* Find most frequent player move */
                for (i = 0; i < move_count; i++) {
                    freq[prev_moves[i]]++;
                }
                
                max_freq = 0;
                predicted = rand() % 11;
                
                for (i = 0; i < 11; i++) {
                    if (freq[i] > max_freq) {
                        max_freq = freq[i];
                        predicted = i;
                    }
                }
                
                /* Add some variation */
                if (rand() % 100 < 50) {
                    move = predicted;
                } else if (rand() % 100 < 50) {
                    move = (predicted + 1) % 11;
                } else {
                    move = (predicted + 10) % 11;
                }
            }
            break;
            
        default:
            move = rand() % 11;
    }
    
    return move;
}

/* Reset game state for new game */
void reset_game(void) {
    player_score = 0;
    computer_score = 0;
    move_count = 0;
    same_choice_count = 0;
    last_player_input = -1;
    second_phase = 0;
}

/* Perform the toss */
int do_toss(void) {
    char player_toss;
    int coin;
    int player_won;
    
    clear_screen();
    display_header();
    
    printf("\n");
    print_centered("TOSS TIME!", 50);
    print_line('-', 50);
    printf("\n");
    printf("Choose: (H)ead or (T)ails? ");
    
    player_toss = toupper(get_char_input());
    while (player_toss != 'H' && player_toss != 'T') {
        printf("Invalid choice! Enter H for Head or T for Tails: ");
        player_toss = toupper(get_char_input());
    }
    
    /* Flip the coin */
    coin = rand() % 2; /* 0 = Head, 1 = Tails */
    
    printf("\n");
    printf("Flipping the coin...\n");
    printf("\n");
    
    /* Animation effect */
    for (int i = 0; i < 3; i++) {
        printf(".");
        fflush(stdout);
        /* Simple delay */
        for (volatile int j = 0; j < 50000000; j++);
    }
    
    printf("\n\nThe coin shows: %s!\n", coin == 0 ? "HEAD" : "TAILS");
    printf("You chose: %s\n", player_toss == 'H' ? "HEAD" : "TAILS");
    
    player_won = (player_toss == 'H' && coin == 0) || 
                 (player_toss == 'T' && coin == 1);
    
    if (player_won) {
        printf("\n*** YOU WON THE TOSS! ***\n");
    } else {
        printf("\n*** COMPUTER WON THE TOSS! ***\n");
    }
    
    pause_game();
    return player_won;
}

/* Let player or computer choose to bat/bowl */
int choose_batting(int player_chooses) {
    char choice;
    int player_bats_first;
    
    clear_screen();
    display_header();
    
    printf("\n");
    print_centered("CHOOSE YOUR ROLE", 50);
    print_line('-', 50);
    printf("\n");
    
    if (player_chooses) {
        printf("You won the toss! Choose:\n\n");
        printf("  (B)at first\n");
        printf("  (O)wl first\n\n");
        printf("Your choice: ");
        
        choice = toupper(get_char_input());
        while (choice != 'B' && choice != 'O') {
            printf("Invalid choice! Enter B to Bat or O to Bowl: ");
            choice = toupper(get_char_input());
        }
        
        player_bats_first = (choice == 'B');
        
        printf("\nYou chose to %s first!\n", player_bats_first ? "BAT" : "BOWL");
    } else {
        /* Computer chooses randomly */
        player_bats_first = rand() % 2;
        
        printf("Computer won the toss and chose to %s first.\n", 
               player_bats_first ? "BOWL" : "BAT");
        printf("You will %s first.\n", player_bats_first ? "BAT" : "BOWL");
    }
    
    pause_game();
    return player_bats_first;
}

/* Display the round result */
void display_round_result(int player_num, int comp_num, int is_out, int is_batting) {
    printf("\n");
    print_line('-', 40);
    printf("Your number:      %d\n", player_num);
    printf("Computer's number: %d\n", comp_num);
    print_line('-', 40);
    
    if (is_out) {
        printf("\n  *** SAME NUMBER! %s IS OUT! ***\n", 
               is_batting ? "YOU ARE" : "COMPUTER");
    } else {
        if (is_batting) {
            int runs = (player_num == 0) ? comp_num : player_num;
            printf("\nYou scored %d run(s)!\n", runs);
        } else {
            printf("\nComputer scored %d run(s)!\n", comp_num);
        }
    }
}

/* Play one innings */
int play_innings(int is_batting, int target, int is_second_innings) {
    int player_num, comp_num;
    int runs_scored = 0;
    int is_out = 0;
    int round_num = 1;
    
    /* Reset tracking for new innings */
    same_choice_count = 0;
    last_player_input = -1;
    move_count = 0;
    
    while (!is_out) {
        clear_screen();
        display_header();
        
        /* Show innings info */
        printf("\n");
        if (is_second_innings) {
            printf("*** %s INNINGS ***\n", is_batting ? "CHASING" : "DEFENDING");
            printf("Target: %d runs\n", target + 1);
        } else {
            printf("*** %s INNINGS ***\n", is_batting ? "BATTING" : "BOWLING");
        }
        printf("You are: %s\n", is_batting ? "BATTING" : "BOWLING");
        printf("Round: %d\n\n", round_num);
        
        display_scores();
        
        if (is_second_innings) {
            if (is_batting) {
                printf("\nYou need %d more run(s) to win!\n", 
                       (target + 1) - player_score);
            } else {
                printf("\nComputer needs %d more run(s) to win!\n", 
                       (target + 1) - computer_score);
            }
        }
        
        /* Get player input */
        printf("\nEnter your number (0-10): ");
        player_num = get_int_input(0, 10);
        
        /* Check for repeated inputs */
        if (player_num == last_player_input) {
            same_choice_count++;
            if (same_choice_count == 3) {
                printf("\n*** WARNING: Don't repeat the same number! ***\n");
                pause_game();
            } else if (same_choice_count == 4) {
                printf("\n*** BE CAREFUL! One more repeat and you're OUT! ***\n");
                pause_game();
            } else if (same_choice_count >= 5 && is_batting) {
                printf("\n*** You used the same number 5 times! YOU'RE OUT! ***\n");
                is_out = 1;
                pause_game();
                break;
            }
        } else {
            same_choice_count = 1;
            last_player_input = player_num;
        }
        
        /* Record move */
        if (move_count < MAX_HISTORY) {
            prev_moves[move_count++] = player_num;
        }
        
        /* Generate computer's number */
        comp_num = generate_computer_move();
        
        /* Check if out */
        is_out = (player_num == comp_num);
        
        /* Update scores */
        if (!is_out) {
            if (is_batting) {
                int runs = (player_num == 0) ? comp_num : player_num;
                player_score += runs;
                runs_scored += runs;
            } else {
                computer_score += comp_num;
                runs_scored += comp_num;
            }
        }
        
        /* Display result */
        display_round_result(player_num, comp_num, is_out, is_batting);
        display_scores();
        
        /* Check win/lose conditions in second innings */
        if (is_second_innings && !is_out) {
            if (is_batting && player_score > target) {
                printf("\n*** YOU CHASED THE TARGET! ***\n");
                pause_game();
                return runs_scored;
            } else if (!is_batting && computer_score > target) {
                printf("\n*** COMPUTER CHASED THE TARGET! ***\n");
                pause_game();
                return runs_scored;
            }
        }
        
        if (!is_out) {
            pause_game();
        }
        
        round_num++;
    }
    
    return runs_scored;
}

/* Display final result */
void display_final_result(void) {
    clear_screen();
    display_header();
    
    printf("\n");
    print_centered("GAME OVER!", 50);
    print_line('=', 50);
    printf("\n");
    
    display_scores();
    
    printf("\n");
    print_line('-', 50);
    
    if (player_score > computer_score) {
        printf("\n");
        printf("  *************************************\n");
        printf("  *                                   *\n");
        printf("  *   CONGRATULATIONS! YOU WIN!       *\n");
        printf("  *                                   *\n");
        printf("  *   You won by %d run(s)!           *\n", 
               player_score - computer_score);
        printf("  *                                   *\n");
        printf("  *************************************\n");
    } else if (computer_score > player_score) {
        printf("\n");
        printf("  *************************************\n");
        printf("  *                                   *\n");
        printf("  *   SORRY! YOU LOST!                *\n");
        printf("  *                                   *\n");
        printf("  *   Computer won by %d run(s)       *\n", 
               computer_score - player_score);
        printf("  *                                   *\n");
        printf("  *************************************\n");
    } else {
        printf("\n");
        printf("  *************************************\n");
        printf("  *                                   *\n");
        printf("  *   IT'S A TIE!                     *\n");
        printf("  *                                   *\n");
        printf("  *   Both scored %d runs!            *\n", player_score);
        printf("  *                                   *\n");
        printf("  *************************************\n");
    }
    
    printf("\n");
    print_line('=', 50);
    pause_game();
}

/* Main game loop */
void play_game(void) {
    int player_won_toss;
    int player_bats_first;
    int first_innings_score;
    
    /* Reset for new game */
    reset_game();
    
    /* Toss */
    player_won_toss = do_toss();
    
    /* Choose batting/bowling */
    player_bats_first = choose_batting(player_won_toss);
    
    /* First innings */
    clear_screen();
    display_header();
    printf("\n");
    print_centered("FIRST INNINGS STARTING!", 50);
    printf("\n");
    
    if (player_bats_first) {
        printf("You are BATTING first. Score as many runs as you can!\n");
    } else {
        printf("You are BOWLING first. Try to get the computer out!\n");
    }
    
    pause_game();
    
    /* Play first innings */
    play_innings(player_bats_first, 0, 0);
    
    /* Store first innings score */
    if (player_bats_first) {
        first_innings_score = player_score;
    } else {
        first_innings_score = computer_score;
    }
    
    /* Transition to second innings */
    clear_screen();
    display_header();
    printf("\n");
    print_centered("INNINGS BREAK", 50);
    print_line('-', 50);
    printf("\n");
    
    if (player_bats_first) {
        printf("Your score: %d runs\n", player_score);
        printf("\nComputer needs %d runs to win!\n", player_score + 1);
        printf("\nYou are now BOWLING. Defend your score!\n");
    } else {
        printf("Computer's score: %d runs\n", computer_score);
        printf("\nYou need %d runs to win!\n", computer_score + 1);
        printf("\nYou are now BATTING. Chase the target!\n");
    }
    
    pause_game();
    
    /* Reset tracking for second innings */
    second_phase = 1;
    
    /* Play second innings */
    play_innings(!player_bats_first, first_innings_score, 1);
    
    /* Display final result */
    display_final_result();
}

/* ==================== MAIN FUNCTION ==================== */

int main(void) {
    int choice;
    int running = 1;
    
    /* Seed random number generator */
    srand((unsigned int)time(NULL));
    
    /* Main menu loop */
    while (running) {
        display_main_menu();
        choice = get_int_input(1, 4);
        
        switch (choice) {
            case 1:
                play_game();
                break;
                
            case 2:
                display_difficulty_menu();
                difficulty = get_int_input(1, 3);
                printf("\nDifficulty set to ");
                switch (difficulty) {
                    case EASY:   printf("EASY!\n");   break;
                    case MEDIUM: printf("MEDIUM!\n"); break;
                    case HARD:   printf("HARD!\n");   break;
                }
                pause_game();
                break;
                
            case 3:
                display_instructions();
                break;
                
            case 4:
                clear_screen();
                display_header();
                printf("\n");
                print_centered("Thanks for playing!", 50);
                print_centered("Goodbye!", 50);
                printf("\n");
                print_line('=', 50);
                running = 0;
                break;
        }
    }
    
    return 0;
}

/*
 * ========================================
 * END OF HAND CRICKET GAME
 * ========================================
 * 
 * FEATURES:
 * - Toss mechanism (heads/tails)
 * - Choose to bat or bowl
 * - Three difficulty levels (Easy, Medium, Hard)
 * - Computer AI with pattern recognition (Hard mode)
 * - Two innings gameplay
 * - Score tracking
 * - Warning system for repeated moves
 * - Target chasing mechanics
 * - Beautiful console UI with ASCII art
 * 
 * COMPILATION:
 *   gcc handcricket.c -o handcricket
 * 
 * RUNNING:
 *   ./handcricket (Linux/Mac)
 *   handcricket.exe (Windows)
 * 
 * ========================================
 */
