/*
 * HAND CRICKET GAME - Complete Web Application in Pure C
 * With full UI: Grid, Panels, Buttons, Animations
 * 
 * Compile: gcc new_handcricket.c -o new_handcricket -pthread
 * Run: ./new_handcricket
 * Open: http://localhost:8080
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 131072
#define MAX_SESSIONS 100

typedef struct {
    char session_id[64];
    int player_score;
    int computer_score;
    int difficulty;
    int game_phase;
    int is_batting;
    int second_innings;
    int first_innings_score;
    int prev_moves[100];
    int move_count;
    int same_choice_count;
    int last_player_input;
    int last_computer_move;
    int is_out;
    char message[512];
    time_t last_activity;
} GameSession;

GameSession sessions[MAX_SESSIONS];
pthread_mutex_t sessions_mutex = PTHREAD_MUTEX_INITIALIZER;

/* CSS Styles embedded in C */
const char *CSS_STYLES = 
"<style>"
"*{margin:0;padding:0;box-sizing:border-box;}"
"body{font-family:'Segoe UI',Arial,sans-serif;background:linear-gradient(135deg,#1a1a2e 0%%,#16213e 50%%,#0f3460 100%%);min-height:100vh;display:flex;justify-content:center;align-items:center;padding:20px;}"
".container{background:rgba(255,255,255,0.95);border-radius:20px;box-shadow:0 20px 60px rgba(0,0,0,0.3);padding:30px;max-width:500px;width:100%%;}"
".header{text-align:center;margin-bottom:20px;padding-bottom:15px;border-bottom:3px solid #e94560;}"
".header h1{color:#e94560;font-size:24px;}"
".header p{color:#666;font-size:12px;margin-top:5px;}"
".panel{background:linear-gradient(145deg,#f5f5f5,#fff);border-radius:15px;padding:20px;margin-bottom:15px;box-shadow:0 5px 15px rgba(0,0,0,0.1);}"
".panel-title{color:#333;font-size:16px;font-weight:bold;margin-bottom:15px;padding-bottom:10px;border-bottom:2px solid #e94560;}"
".message-box{background:linear-gradient(135deg,#667eea,#764ba2);color:#fff;padding:15px;border-radius:12px;text-align:center;font-size:14px;margin-bottom:15px;min-height:50px;display:flex;align-items:center;justify-content:center;}"
".scoreboard{display:grid;grid-template-columns:1fr 1fr;gap:15px;margin-bottom:15px;}"
".score-card{background:#fff;border-radius:12px;padding:15px;text-align:center;box-shadow:0 4px 10px rgba(0,0,0,0.1);}"
".score-card.player{border-left:4px solid #28a745;}"
".score-card.computer{border-left:4px solid #dc3545;}"
".score-card .label{font-size:12px;color:#666;text-transform:uppercase;}"
".score-card .score{font-size:36px;font-weight:bold;}"
".score-card.player .score{color:#28a745;}"
".score-card.computer .score{color:#dc3545;}"
".choices{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-bottom:15px;}"
".choice-box{background:#f8f9fa;border-radius:10px;padding:12px;text-align:center;}"
".choice-box .label{font-size:11px;color:#666;}"
".choice-box .value{font-size:24px;font-weight:bold;color:#333;}"
".btn-grid{display:grid;grid-template-columns:repeat(4,1fr);gap:8px;}"
".btn-grid-2{display:grid;grid-template-columns:repeat(2,1fr);gap:10px;}"
".btn-grid-3{display:grid;grid-template-columns:repeat(3,1fr);gap:10px;}"
".btn{padding:12px 15px;font-size:14px;font-weight:bold;border:none;border-radius:10px;cursor:pointer;text-decoration:none;display:flex;align-items:center;justify-content:center;transition:all 0.2s;color:#fff;}"
".btn:hover{transform:translateY(-2px);box-shadow:0 5px 15px rgba(0,0,0,0.2);}"
".btn-primary{background:linear-gradient(135deg,#667eea,#764ba2);}"
".btn-success{background:linear-gradient(135deg,#11998e,#38ef7d);}"
".btn-danger{background:linear-gradient(135deg,#e94560,#ff6b6b);}"
".btn-warning{background:linear-gradient(135deg,#f093fb,#f5576c);}"
".btn-info{background:linear-gradient(135deg,#4facfe,#00f2fe);}"
".btn-number{background:linear-gradient(145deg,#fff,#e6e6e6);color:#333;font-size:18px;padding:15px;box-shadow:0 4px 10px rgba(0,0,0,0.1);}"
".btn-number:hover{background:linear-gradient(135deg,#667eea,#764ba2);color:#fff;}"
".btn-wide{grid-column:span 2;}"
".status-badge{display:inline-block;padding:5px 15px;border-radius:20px;font-size:11px;font-weight:bold;text-transform:uppercase;margin-bottom:10px;}"
".status-batting{background:#d4edda;color:#155724;}"
".status-bowling{background:#f8d7da;color:#721c24;}"
".target-info{background:#fff3cd;color:#856404;padding:10px;border-radius:8px;text-align:center;margin-bottom:15px;font-size:13px;}"
".result-banner{padding:25px;border-radius:15px;text-align:center;margin-bottom:15px;color:#fff;}"
".result-win{background:linear-gradient(135deg,#11998e,#38ef7d);}"
".result-lose{background:linear-gradient(135deg,#e94560,#ff6b6b);}"
".result-tie{background:linear-gradient(135deg,#667eea,#764ba2);}"
".result-icon{font-size:40px;margin-bottom:10px;}"
".result-text{font-size:22px;font-weight:bold;}"
".result-detail{font-size:13px;margin-top:8px;opacity:0.9;}"
".footer{text-align:center;margin-top:15px;padding-top:10px;border-top:1px solid #eee;color:#999;font-size:11px;}"
".difficulty-active{background:linear-gradient(135deg,#e94560,#ff6b6b) !important;color:#fff !important;}"
"@keyframes pulse{0%%,100%%{transform:scale(1);}50%%{transform:scale(1.02);}}"
".pulse{animation:pulse 0.3s;}"
"</style>";

void generate_session_id(char *sid) {
    sprintf(sid, "%ld%d", time(NULL), rand() % 10000);
}

GameSession* find_session(const char *sid) {
    pthread_mutex_lock(&sessions_mutex);
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (strcmp(sessions[i].session_id, sid) == 0) {
            sessions[i].last_activity = time(NULL);
            pthread_mutex_unlock(&sessions_mutex);
            return &sessions[i];
        }
    }
    pthread_mutex_unlock(&sessions_mutex);
    return NULL;
}

GameSession* create_session(void) {
    pthread_mutex_lock(&sessions_mutex);
    time_t now = time(NULL);
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].session_id[0] == '\0' || (now - sessions[i].last_activity) > 3600) {
            memset(&sessions[i], 0, sizeof(GameSession));
            generate_session_id(sessions[i].session_id);
            sessions[i].difficulty = 1;
            sessions[i].last_player_input = -1;
            sessions[i].last_computer_move = -1;
            sessions[i].last_activity = now;
            strcpy(sessions[i].message, "Welcome! Click 'New Game' to start playing!");
            pthread_mutex_unlock(&sessions_mutex);
            return &sessions[i];
        }
    }
    pthread_mutex_unlock(&sessions_mutex);
    return NULL;
}

int generate_computer_move(GameSession *s) {
    int move, freq[11] = {0};
    switch (s->difficulty) {
        case 1: move = rand() % 11; break;
        case 2:
            if (rand() % 100 < 30 && s->move_count > 0)
                move = s->prev_moves[s->move_count - 1];
            else move = rand() % 11;
            break;
        case 3:
            if (s->move_count == 0) { move = rand() % 11; }
            else {
                int max_f = 0, pred = rand() % 11;
                for (int i = 0; i < s->move_count; i++) freq[s->prev_moves[i]]++;
                for (int i = 0; i < 11; i++) if (freq[i] > max_f) { max_f = freq[i]; pred = i; }
                move = pred;
            }
            break;
        default: move = rand() % 11;
    }
    return move;
}

void reset_game(GameSession *s) {
    s->player_score = 0;
    s->computer_score = 0;
    s->game_phase = 1;
    s->is_batting = 0;
    s->second_innings = 0;
    s->first_innings_score = 0;
    s->move_count = 0;
    s->same_choice_count = 0;
    s->last_player_input = -1;
    s->last_computer_move = -1;
    s->is_out = 0;
    strcpy(s->message, "Choose HEAD or TAILS for the toss!");
}

char* get_session_cookie(const char *req) {
    static char sid[64];
    char *p = strstr(req, "session=");
    if (p) {
        p += 8;
        int i = 0;
        while (p[i] && p[i] != ';' && p[i] != ' ' && p[i] != '\r' && i < 63) {
            sid[i] = p[i]; i++;
        }
        sid[i] = '\0';
        return sid;
    }
    return NULL;
}

void build_html_head(char *buf) {
    sprintf(buf,
        "<!DOCTYPE html><html lang=\"en\"><head>"
        "<meta charset=\"UTF-8\">"
        "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
        "<title>Hand Cricket Game</title>%s</head><body>", CSS_STYLES);
}

void build_page_menu(char *resp, GameSession *s) {
    char html[BUFFER_SIZE];
    char head[8192];
    const char *diff_names[] = {"", "Easy", "Medium", "Hard"};
    
    build_html_head(head);
    
    sprintf(html,
        "%s<div class=\"container\">"
        "<div class=\"header\"><h1>Hand Cricket Game</h1><p>Odd or Even Cricket</p></div>"
        "<div class=\"message-box\">%s</div>"
        "<div class=\"panel\"><div class=\"panel-title\">Main Menu</div>"
        "<div class=\"btn-grid-2\">"
        "<a href=\"/start\" class=\"btn btn-success\">New Game</a>"
        "<a href=\"/help\" class=\"btn btn-info\">How to Play</a>"
        "</div></div>"
        "<div class=\"panel\"><div class=\"panel-title\">Difficulty: %s</div>"
        "<div class=\"btn-grid-3\">"
        "<a href=\"/diff/1\" class=\"btn btn-number %s\">Easy</a>"
        "<a href=\"/diff/2\" class=\"btn btn-number %s\">Medium</a>"
        "<a href=\"/diff/3\" class=\"btn btn-number %s\">Hard</a>"
        "</div></div>"
        "<div class=\"footer\">Made with C | Hand Cricket v2.0</div>"
        "</div></body></html>",
        head, s->message, diff_names[s->difficulty],
        s->difficulty == 1 ? "difficulty-active" : "",
        s->difficulty == 2 ? "difficulty-active" : "",
        s->difficulty == 3 ? "difficulty-active" : "");
    
    sprintf(resp,
        "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n"
        "Set-Cookie: session=%s; Path=/\r\nConnection: close\r\n\r\n%s",
        s->session_id, html);
}

void build_page_help(char *resp, GameSession *s) {
    char html[BUFFER_SIZE];
    char head[8192];
    build_html_head(head);
    
    sprintf(html,
        "%s<div class=\"container\">"
        "<div class=\"header\"><h1>How to Play</h1></div>"
        "<div class=\"panel\">"
        "<p><b>1. TOSS:</b> Pick HEAD or TAILS. Winner chooses role.</p><br>"
        "<p><b>2. GAMEPLAY:</b> Pick a number 0-10. Computer picks too.</p><br>"
        "<p><b>3. OUT:</b> If both pick the same number, batsman is OUT!</p><br>"
        "<p><b>4. SCORING:</b> Batting? Your number = runs. Bowling? Computer's number = their runs.</p><br>"
        "<p><b>5. SPECIAL:</b> Pick 0 to steal computer's number as your runs!</p><br>"
        "<p><b>6. TWO INNINGS:</b> After out, roles swap. Chase or defend!</p>"
        "</div>"
        "<div class=\"btn-grid-2\"><a href=\"/\" class=\"btn btn-primary\">Back to Menu</a>"
        "<a href=\"/start\" class=\"btn btn-success\">Start Game</a></div>"
        "<div class=\"footer\">Made with C</div>"
        "</div></body></html>", head);
    
    sprintf(resp,
        "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n"
        "Set-Cookie: session=%s; Path=/\r\nConnection: close\r\n\r\n%s",
        s->session_id, html);
}

void build_page_toss(char *resp, GameSession *s) {
    char html[BUFFER_SIZE];
    char head[8192];
    build_html_head(head);
    
    sprintf(html,
        "%s<div class=\"container\">"
        "<div class=\"header\"><h1>Toss Time!</h1></div>"
        "<div class=\"message-box\">%s</div>"
        "<div class=\"panel\"><div class=\"panel-title\">Make Your Call</div>"
        "<div class=\"btn-grid-2\">"
        "<a href=\"/toss/head\" class=\"btn btn-primary\" style=\"padding:25px;font-size:18px;\">HEAD</a>"
        "<a href=\"/toss/tail\" class=\"btn btn-warning\" style=\"padding:25px;font-size:18px;\">TAILS</a>"
        "</div></div>"
        "<a href=\"/\" class=\"btn btn-danger\" style=\"width:100%%;\">Back to Menu</a>"
        "<div class=\"footer\">Made with C</div>"
        "</div></body></html>", head, s->message);
    
    sprintf(resp,
        "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n"
        "Set-Cookie: session=%s; Path=/\r\nConnection: close\r\n\r\n%s",
        s->session_id, html);
}

void build_page_choose(char *resp, GameSession *s) {
    char html[BUFFER_SIZE];
    char head[8192];
    build_html_head(head);
    
    sprintf(html,
        "%s<div class=\"container\">"
        "<div class=\"header\"><h1>You Won the Toss!</h1></div>"
        "<div class=\"message-box\">%s</div>"
        "<div class=\"panel\"><div class=\"panel-title\">Choose Your Role</div>"
        "<div class=\"btn-grid-2\">"
        "<a href=\"/choose/bat\" class=\"btn btn-success\" style=\"padding:25px;font-size:18px;\">BAT First</a>"
        "<a href=\"/choose/bowl\" class=\"btn btn-danger\" style=\"padding:25px;font-size:18px;\">BOWL First</a>"
        "</div></div>"
        "<div class=\"footer\">Made with C</div>"
        "</div></body></html>", head, s->message);
    
    sprintf(resp,
        "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n"
        "Set-Cookie: session=%s; Path=/\r\nConnection: close\r\n\r\n%s",
        s->session_id, html);
}

void build_page_game(char *resp, GameSession *s) {
    char html[BUFFER_SIZE];
    char head[8192];
    char your_choice[8], comp_choice[8];
    char target_html[256] = "";
    char innings_text[64];
    
    build_html_head(head);
    
    if (s->last_player_input >= 0) sprintf(your_choice, "%d", s->last_player_input);
    else strcpy(your_choice, "-");
    if (s->last_computer_move >= 0) sprintf(comp_choice, "%d", s->last_computer_move);
    else strcpy(comp_choice, "-");
    
    if (s->second_innings) {
        sprintf(innings_text, "%s - 2nd Innings", s->is_batting ? "CHASING" : "DEFENDING");
        if (s->is_batting) {
            sprintf(target_html, "<div class=\"target-info\">Target: %d | Need: %d more to win</div>",
                s->first_innings_score + 1, (s->first_innings_score + 1) - s->player_score);
        } else {
            sprintf(target_html, "<div class=\"target-info\">Target: %d | Computer needs: %d more</div>",
                s->first_innings_score + 1, (s->first_innings_score + 1) - s->computer_score);
        }
    } else {
        sprintf(innings_text, "%s - 1st Innings", s->is_batting ? "BATTING" : "BOWLING");
    }
    
    sprintf(html,
        "%s<div class=\"container\">"
        "<div class=\"header\"><h1>Hand Cricket</h1></div>"
        "<div class=\"message-box\">%s</div>"
        "<div style=\"text-align:center;\">"
        "<span class=\"status-badge %s\">%s</span>"
        "</div>"
        "%s"
        "<div class=\"scoreboard\">"
        "<div class=\"score-card player\"><div class=\"label\">Your Score</div><div class=\"score\">%d</div></div>"
        "<div class=\"score-card computer\"><div class=\"label\">Computer</div><div class=\"score\">%d</div></div>"
        "</div>"
        "<div class=\"choices\">"
        "<div class=\"choice-box\"><div class=\"label\">Your Pick</div><div class=\"value\">%s</div></div>"
        "<div class=\"choice-box\"><div class=\"label\">Computer</div><div class=\"value\">%s</div></div>"
        "</div>"
        "<div class=\"panel\"><div class=\"panel-title\">Pick a Number (0-10)</div>"
        "<div class=\"btn-grid\">"
        "<a href=\"/play/0\" class=\"btn btn-number\">0</a>"
        "<a href=\"/play/1\" class=\"btn btn-number\">1</a>"
        "<a href=\"/play/2\" class=\"btn btn-number\">2</a>"
        "<a href=\"/play/3\" class=\"btn btn-number\">3</a>"
        "<a href=\"/play/4\" class=\"btn btn-number\">4</a>"
        "<a href=\"/play/5\" class=\"btn btn-number\">5</a>"
        "<a href=\"/play/6\" class=\"btn btn-number\">6</a>"
        "<a href=\"/play/7\" class=\"btn btn-number\">7</a>"
        "<a href=\"/play/8\" class=\"btn btn-number\">8</a>"
        "<a href=\"/play/9\" class=\"btn btn-number\">9</a>"
        "<a href=\"/play/10\" class=\"btn btn-number btn-wide\">10</a>"
        "<a href=\"/reset\" class=\"btn btn-danger btn-wide\">Reset Game</a>"
        "</div></div>"
        "<div class=\"footer\">Made with C</div>"
        "</div></body></html>",
        head, s->message,
        s->is_batting ? "status-batting" : "status-bowling",
        innings_text, target_html,
        s->player_score, s->computer_score,
        your_choice, comp_choice);
    
    sprintf(resp,
        "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n"
        "Set-Cookie: session=%s; Path=/\r\nConnection: close\r\n\r\n%s",
        s->session_id, html);
}

void build_page_gameover(char *resp, GameSession *s) {
    char html[BUFFER_SIZE];
    char head[8192];
    char result_class[32], result_icon[16], result_text[32], result_detail[64];
    
    build_html_head(head);
    
    if (s->player_score > s->computer_score) {
        strcpy(result_class, "result-win");
        strcpy(result_icon, "🎉");
        strcpy(result_text, "YOU WIN!");
        sprintf(result_detail, "Won by %d run(s)!", s->player_score - s->computer_score);
    } else if (s->computer_score > s->player_score) {
        strcpy(result_class, "result-lose");
        strcpy(result_icon, "😔");
        strcpy(result_text, "YOU LOST");
        sprintf(result_detail, "Lost by %d run(s)", s->computer_score - s->player_score);
    } else {
        strcpy(result_class, "result-tie");
        strcpy(result_icon, "🤝");
        strcpy(result_text, "IT'S A TIE!");
        sprintf(result_detail, "Both scored %d runs", s->player_score);
    }
    
    sprintf(html,
        "%s<div class=\"container\">"
        "<div class=\"header\"><h1>Game Over!</h1></div>"
        "<div class=\"result-banner %s\">"
        "<div class=\"result-icon\">%s</div>"
        "<div class=\"result-text\">%s</div>"
        "<div class=\"result-detail\">%s</div>"
        "</div>"
        "<div class=\"scoreboard\">"
        "<div class=\"score-card player\"><div class=\"label\">Your Score</div><div class=\"score\">%d</div></div>"
        "<div class=\"score-card computer\"><div class=\"label\">Computer</div><div class=\"score\">%d</div></div>"
        "</div>"
        "<div class=\"btn-grid-2\">"
        "<a href=\"/start\" class=\"btn btn-success\">Play Again</a>"
        "<a href=\"/\" class=\"btn btn-primary\">Main Menu</a>"
        "</div>"
        "<div class=\"footer\">Made with C</div>"
        "</div></body></html>",
        head, result_class, result_icon, result_text, result_detail,
        s->player_score, s->computer_score);
    
    sprintf(resp,
        "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n"
        "Set-Cookie: session=%s; Path=/\r\nConnection: close\r\n\r\n%s",
        s->session_id, html);
}

void handle_toss(GameSession *s, const char *choice) {
    int player_head = (strcmp(choice, "head") == 0);
    int coin = rand() % 2;
    int won = (player_head && coin == 0) || (!player_head && coin == 1);
    
    if (won) {
        s->game_phase = 2;
        sprintf(s->message, "Coin: %s | You called: %s | YOU WON! Choose to Bat or Bowl.",
            coin == 0 ? "HEAD" : "TAILS", player_head ? "HEAD" : "TAILS");
    } else {
        int comp_bats = rand() % 2;
        s->is_batting = !comp_bats;
        s->game_phase = 3;
        sprintf(s->message, "Coin: %s | You called: %s | Computer won! You are %s.",
            coin == 0 ? "HEAD" : "TAILS", player_head ? "HEAD" : "TAILS",
            s->is_batting ? "BATTING" : "BOWLING");
    }
}

void handle_choose(GameSession *s, const char *choice) {
    s->is_batting = (strcmp(choice, "bat") == 0);
    s->game_phase = 3;
    sprintf(s->message, "You chose to %s first. Pick a number!", s->is_batting ? "BAT" : "BOWL");
}

void handle_play(GameSession *s, int num) {
    int comp = generate_computer_move(s);
    s->last_player_input = num;
    s->last_computer_move = comp;
    
    if (s->move_count > 0 && num == s->prev_moves[s->move_count - 1])
        s->same_choice_count++;
    else
        s->same_choice_count = 1;
    
    if (s->move_count < 100) s->prev_moves[s->move_count++] = num;
    
    if (s->same_choice_count >= 5 && s->is_batting) {
        sprintf(s->message, "Same number 5 times! YOU'RE OUT!");
        s->is_out = 1;
    } else if (num == comp) {
        s->is_out = 1;
        sprintf(s->message, "OUT! Both picked %d! %s out!", num, s->is_batting ? "You're" : "Computer is");
    } else {
        s->is_out = 0;
        if (s->is_batting) {
            int runs = (num == 0) ? comp : num;
            s->player_score += runs;
            sprintf(s->message, "You: %d | Computer: %d | +%d runs!", num, comp, runs);
            if (s->second_innings && s->player_score > s->first_innings_score) {
                s->game_phase = 4;
                strcpy(s->message, "You chased the target! YOU WIN!");
            }
        } else {
            s->computer_score += comp;
            sprintf(s->message, "You: %d | Computer: %d | Computer +%d", num, comp, comp);
            if (s->second_innings && s->computer_score > s->first_innings_score) {
                s->game_phase = 4;
                strcpy(s->message, "Computer chased the target! You lost.");
            }
        }
    }
    
    if (s->is_out && s->game_phase != 4) {
        if (!s->second_innings) {
            s->second_innings = 1;
            s->first_innings_score = s->is_batting ? s->player_score : s->computer_score;
            s->is_batting = !s->is_batting;
            s->is_out = 0;
            s->same_choice_count = 0;
            s->move_count = 0;
            sprintf(s->message, "Innings over! %s Target: %d",
                s->is_batting ? "Now BATTING!" : "Now BOWLING!",
                s->first_innings_score + 1);
        } else {
            s->game_phase = 4;
        }
    }
}

void handle_request(int sock, const char *req) {
    char resp[BUFFER_SIZE];
    char *sid = get_session_cookie(req);
    GameSession *s = sid ? find_session(sid) : NULL;
    if (!s) s = create_session();
    if (!s) { write(sock, "HTTP/1.1 500 Error\r\n\r\n", 22); return; }
    
    char path[256] = "/";
    sscanf(req, "GET %255s", path);
    
    if (strcmp(path, "/") == 0) {
        s->game_phase = 0;
        build_page_menu(resp, s);
    }
    else if (strcmp(path, "/help") == 0) build_page_help(resp, s);
    else if (strcmp(path, "/start") == 0) { reset_game(s); build_page_toss(resp, s); }
    else if (strcmp(path, "/reset") == 0) { s->game_phase = 0; strcpy(s->message, "Game reset!"); build_page_menu(resp, s); }
    else if (strncmp(path, "/diff/", 6) == 0) {
        int d = atoi(path + 6);
        if (d >= 1 && d <= 3) { s->difficulty = d; sprintf(s->message, "Difficulty: %s", d==1?"Easy":d==2?"Medium":"Hard"); }
        build_page_menu(resp, s);
    }
    else if (strncmp(path, "/toss/", 6) == 0) {
        handle_toss(s, path + 6);
        if (s->game_phase == 2) build_page_choose(resp, s);
        else build_page_game(resp, s);
    }
    else if (strncmp(path, "/choose/", 8) == 0) { handle_choose(s, path + 8); build_page_game(resp, s); }
    else if (strncmp(path, "/play/", 6) == 0) {
        int n = atoi(path + 6);
        if (n >= 0 && n <= 10) handle_play(s, n);
        if (s->game_phase == 4) build_page_gameover(resp, s);
        else build_page_game(resp, s);
    }
    else build_page_menu(resp, s);
    
    write(sock, resp, strlen(resp));
}

void *client_thread(void *arg) {
    int sock = *(int*)arg;
    free(arg);
    char buf[BUFFER_SIZE];
    ssize_t n = read(sock, buf, BUFFER_SIZE - 1);
    if (n > 0) { buf[n] = '\0'; handle_request(sock, buf); }
    close(sock);
    return NULL;
}

int main(void) {
    int server_fd;
    struct sockaddr_in addr;
    
    srand(time(NULL));
    memset(sessions, 0, sizeof(sessions));
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════╗\n");
    printf("║     HAND CRICKET GAME - WEB SERVER            ║\n");
    printf("╠═══════════════════════════════════════════════╣\n");
    printf("║  Open: http://localhost:%d                   ║\n", PORT);
    printf("║  Press Ctrl+C to stop                         ║\n");
    printf("╚═══════════════════════════════════════════════╝\n\n");
    
    while (1) {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int *sock = malloc(sizeof(int));
        *sock = accept(server_fd, (struct sockaddr*)&client, &len);
        if (*sock >= 0) {
            pthread_t tid;
            pthread_create(&tid, NULL, client_thread, sock);
            pthread_detach(tid);
        } else free(sock);
    }
    
    return 0;
}
