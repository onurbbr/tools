#include <ncurses.h>
#include <stdlib.h>
#include "swinfo.h"
#include "user.h"
#include "system.h"
#include "de.h"

// Function to display software information
void displaySoftwareInfo() {
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);

    char *choices[] = {
        "User Information",
        "DE Information",
        "System Information",
        "Back",
        "Exit"
    };
    int choice;
    int highlight = 0;
    int num_choices = sizeof(choices) / sizeof(char*);

    while (1) {
        clear();
        box(stdscr, 0, 0); // Add a box around the menu
        for (int i = 0; i < num_choices; ++i) {
            if (i == highlight) {
                attron(A_REVERSE); // Highlight the current choice
            }
            mvprintw(i + 2, 3, "%s", choices[i]); // Adjust position to fit inside the box
            attroff(A_REVERSE);
        }
        choice = getch();

        switch (choice) {
            case KEY_UP:
                highlight = (highlight - 1 + num_choices) % num_choices;
                break;
            case KEY_DOWN:
                highlight = (highlight + 1) % num_choices;
                break;
            case 10: // Enter key
                if (highlight == 0) {
                    displayUserInfo();
                } else if (highlight == 1) {
                    displayDEInfo();
                } else if (highlight == 2) {
                    displaySystemInfo();
                } else if (highlight == 3) {
                    endwin();
                    return;
                } else if (highlight == 4) {
                    endwin();
                    exit(0);
                }
                break;
            default:
                break;
        }
    }

    endwin();
}