#include <ncurses.h>
#include "swinfo/swinfo.h"
#include "hwinfo/hwinfo.h"

int main() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Create menu items
    char* menu_items[] = {"Software Information", "Hardware Information", "Exit"};
    int num_items = sizeof(menu_items) / sizeof(menu_items[0]);
    int current_item = 0;

    // Display menu
    while (true) {
        clear();

        // Draw frame
        box(stdscr, 0, 0);

        // Highlight the current item
        for (int i = 0; i < num_items; i++) {
            if (i == current_item) {
                attron(A_REVERSE);
            }
            mvprintw(i + 2, 3, "%s", menu_items[i]); // Adjusted position to leave space
            attroff(A_REVERSE);
        }

        // Get user input
        int ch = getch();
        switch (ch) {
            case KEY_UP:
                current_item--;
                if (current_item < 0) {
                    current_item = num_items - 1;
                }
                break;
            case KEY_DOWN:
                current_item++;
                if (current_item >= num_items) {
                    current_item = 0;
                }
                break;
            case 10:  // Enter key
                if (current_item == 0) {
                    displaySoftwareInfo();
                } else if (current_item == 1) {
                    displayHardwareInfo();
                } else if (current_item == 2) {
                    endwin();
                    return 0;
                }
                break;
        }

        refresh();
    }

    // Clean up ncurses
    endwin();

    return 0;
}
