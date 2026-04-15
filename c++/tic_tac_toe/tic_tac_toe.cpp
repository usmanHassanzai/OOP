#include <gtk/gtk.h>
#include <string>
#include <iostream>
using namespace std;

// ===================== Player Class =====================
class Player {
private:
    string name;
    char symbol;
    int score;

public:
    Player(string n = "", char s = ' ') : name(n), symbol(s), score(0) {}

    string getName() { return name; }
    char getSymbol() { return symbol; }
    int getScore() { return score; }
    void addWin() { score++; }
};

// ===================== Board Class =====================
class Board {
private:
    char cells[3][3];

public:
    Board() {
        reset();
    }

    void reset() {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                cells[i][j] = ' ';
    }

    bool placeMove(int row, int col, char symbol) {
        if (cells[row][col] == ' ') {
            cells[row][col] = symbol;
            return true;
        }
        return false;
    }

    char getCell(int row, int col) {
        return cells[row][col];
    }

    bool checkWin(char symbol) {
        // check rows & columns
        for (int i = 0; i < 3; i++) {
            if (cells[i][0] == symbol && cells[i][1] == symbol && cells[i][2] == symbol)
                return true;
            if (cells[0][i] == symbol && cells[1][i] == symbol && cells[2][i] == symbol)
                return true;
        }
        // check diagonals
        if (cells[0][0] == symbol && cells[1][1] == symbol && cells[2][2] == symbol)
            return true;
        if (cells[0][2] == symbol && cells[1][1] == symbol && cells[2][0] == symbol)
            return true;
        return false;
    }

    bool isFull() {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                if (cells[i][j] == ' ')
                    return false;
        return true;
    }
};

// ===================== Game Class =====================
class Game {
private:
    Board board;
    Player player1;
    Player player2;
    Player* currentPlayer;
    bool gameOver;
    int draws;

    // GTK widgets
    GtkWidget* window;
    GtkWidget* buttons[3][3];
    GtkWidget* statusLabel;
    GtkWidget* scoreLabel;

public:
    Game() {
        player1 = Player("Player 1", 'X');
        player2 = Player("Player 2", 'O');
        currentPlayer = &player1;
        gameOver = false;
        draws = 0;
    }

    void switchPlayer() {
        if (currentPlayer == &player1)
            currentPlayer = &player2;
        else
            currentPlayer = &player1;
    }

    void updateStatus(const string& msg) {
        string markup = "<span font='16' weight='bold'>" + msg + "</span>";
        gtk_label_set_markup(GTK_LABEL(statusLabel), markup.c_str());
    }

    void updateScoreBoard() {
        string text = "  X: " + to_string(player1.getScore()) +
                      "    Draws: " + to_string(draws) +
                      "    O: " + to_string(player2.getScore()) + "  ";
        string markup = "<span font='13'>" + text + "</span>";
        gtk_label_set_markup(GTK_LABEL(scoreLabel), markup.c_str());
    }

    void updateButtonStyle(int row, int col) {
        char sym = board.getCell(row, col);
        string label;
        if (sym == 'X')
            label = "<span font='28' weight='bold' color='#6C63FF'>X</span>";
        else
            label = "<span font='28' weight='bold' color='#FF6584'>O</span>";

        GtkWidget* child = gtk_bin_get_child(GTK_BIN(buttons[row][col]));
        gtk_label_set_markup(GTK_LABEL(child), label.c_str());
    }

    void handleClick(int row, int col) {
        if (gameOver) return;

        if (board.placeMove(row, col, currentPlayer->getSymbol())) {
            updateButtonStyle(row, col);

            if (board.checkWin(currentPlayer->getSymbol())) {
                gameOver = true;
                currentPlayer->addWin();
                string msg = currentPlayer->getName() + " (" +
                             currentPlayer->getSymbol() + ") Wins! 🎉";
                updateStatus(msg);
                updateScoreBoard();
                return;
            }

            if (board.isFull()) {
                gameOver = true;
                draws++;
                updateStatus("It's a Draw! 🤝");
                updateScoreBoard();
                return;
            }

            switchPlayer();
            string msg = currentPlayer->getName() + " (" +
                         currentPlayer->getSymbol() + ")'s Turn";
            updateStatus(msg);
        }
    }

    void resetGame() {
        board.reset();
        gameOver = false;
        currentPlayer = &player1;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                GtkWidget* child = gtk_bin_get_child(GTK_BIN(buttons[i][j]));
                gtk_label_set_markup(GTK_LABEL(child),
                    "<span font='28' weight='bold' color='#555'> </span>");
            }
        }
        updateStatus("Player 1 (X)'s Turn");
    }

    // Static callback wrappers for GTK (GTK needs C-style function pointers)
    static void onCellClicked(GtkWidget* widget, gpointer data);
    static void onResetClicked(GtkWidget* widget, gpointer data);

    void buildUI(GtkApplication* app) {
        // --- Main Window ---
        window = gtk_application_window_new(app);
        gtk_window_set_title(GTK_WINDOW(window), "Tic Tac Toe - OOP C++");
        gtk_window_set_default_size(GTK_WINDOW(window), 420, 520);
        gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

        // Dark background style
        GtkCssProvider* css = gtk_css_provider_new();
        string cssData =
            "window { background-color: #1a1a2e; }"
            "button { background: #16213e; border: 2px solid #0f3460; "
            "         border-radius: 10px; min-width: 110px; min-height: 110px; "
            "         transition: 200ms; }"
            "button:hover { background: #1f3a6e; border-color: #6C63FF; }"
            "#resetBtn { background: linear-gradient(135deg, #6C63FF, #FF6584); "
            "            color: white; border: none; border-radius: 8px; "
            "            padding: 10px 30px; font-size: 14px; font-weight: bold; }"
            "#resetBtn:hover { opacity: 0.9; }";
        gtk_css_provider_load_from_data(css, cssData.c_str(), -1, NULL);
        gtk_style_context_add_provider_for_screen(
            gdk_screen_get_default(),
            GTK_STYLE_PROVIDER(css),
            GTK_STYLE_PROVIDER_PRIORITY_USER);

        // --- Main vertical box ---
        GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
        gtk_container_add(GTK_CONTAINER(window), vbox);

        // --- Title ---
        GtkWidget* title = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(title),
            "<span font='22' weight='bold' color='#6C63FF'>Tic Tac Toe</span>");
        gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 5);

        // --- Status label ---
        statusLabel = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(statusLabel),
            "<span font='16' weight='bold' color='#e0e0e0'>Player 1 (X)'s Turn</span>");
        gtk_box_pack_start(GTK_BOX(vbox), statusLabel, FALSE, FALSE, 5);

        // --- Score label ---
        scoreLabel = gtk_label_new(NULL);
        updateScoreBoard();
        gtk_box_pack_start(GTK_BOX(vbox), scoreLabel, FALSE, FALSE, 5);

        // --- 3x3 Grid of buttons ---
        GtkWidget* grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
        gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
        gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, FALSE, 10);

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                buttons[i][j] = gtk_button_new_with_label(" ");

                // Replace the default label with a markup-capable one
                GtkWidget* child = gtk_bin_get_child(GTK_BIN(buttons[i][j]));
                gtk_label_set_markup(GTK_LABEL(child),
                    "<span font='28' weight='bold' color='#555'> </span>");

                // Pack row and col into one int: row * 10 + col
                int* pos = new int(i * 10 + j);
                g_signal_connect(buttons[i][j], "clicked",
                    G_CALLBACK(onCellClicked), pos);

                gtk_grid_attach(GTK_GRID(grid), buttons[i][j], j, i, 1, 1);
            }
        }

        // --- Reset button ---
        GtkWidget* resetBtn = gtk_button_new_with_label("New Game");
        gtk_widget_set_name(resetBtn, "resetBtn");
        gtk_widget_set_halign(resetBtn, GTK_ALIGN_CENTER);
        g_signal_connect(resetBtn, "clicked", G_CALLBACK(onResetClicked), NULL);
        gtk_box_pack_start(GTK_BOX(vbox), resetBtn, FALSE, FALSE, 10);

        gtk_widget_show_all(window);
    }
};

// We need a global pointer so GTK callbacks can access the game
Game* gameInstance = nullptr;

void Game::onCellClicked(GtkWidget* widget, gpointer data) {
    int pos = *((int*)data);
    int row = pos / 10;
    int col = pos % 10;
    gameInstance->handleClick(row, col);
}

void Game::onResetClicked(GtkWidget* widget, gpointer data) {
    gameInstance->resetGame();
}

// GTK app activation callback
static void activate(GtkApplication* app, gpointer userData) {
    gameInstance->buildUI(app);
}

// ===================== Main =====================
int main(int argc, char* argv[]) {
    Game game;
    gameInstance = &game;

    GtkApplication* app = gtk_application_new("com.tictactoe.oop",
                                               G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
