#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "system.h"

void displaySystemInfo() {
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);

    int rows, cols;
    getmaxyx(stdscr, rows, cols); // Get the dimensions of the terminal window

    int windowHeight = (rows * 8) / 10; // Height of the main window
    int windowWidth = (cols * 8) / 10; // Width of the main window

    // Create a centered main window
    WINDOW *mainWin = newwin(windowHeight, windowWidth, (rows - windowHeight) / 2, (cols - windowWidth) / 2);
    box(mainWin, 0, 0); // Draw a box around the main window
    wrefresh(mainWin);

    // Create a window to hold the pad and add a box around it
    int padWinHeight = windowHeight - 4; // Adjust for border and title
    int padWinWidth = windowWidth - 4; // Adjust for border
    WINDOW *padWin = newwin(padWinHeight + 1, padWinWidth, (rows - windowHeight) / 2 + 1, (cols - windowWidth) / 2 + 2);
    box(padWin, 0, 0);
    wrefresh(padWin);

    // Create a pad to hold the content
    int padHeight = 100; // Adjust this value as needed
    int padWidth = padWinWidth - 2; // Adjust for border
    WINDOW *pad = newpad(padHeight, padWidth);

    int line = 0;
    mvwprintw(pad, line++, 1, "User Information: ");
    line++;

    FILE* systemPipe = popen("cat /etc/os-release | grep -w \"NAME\"", "r");
    if (systemPipe) {
        char systemBuffer[256];
        char osName[256] = "";

        while (fgets(systemBuffer, sizeof(systemBuffer), systemPipe)) {
            // Trim newline character if present
            systemBuffer[strcspn(systemBuffer, "\n")] = 0;

            // Parse the OS name
            if (strncmp(systemBuffer, "NAME=", 5) == 0) {
                strncpy(osName, systemBuffer + 6, sizeof(osName) - 1);
                // Remove surrounding quotes if present
                if (osName[0] == '"' && osName[strlen(osName) - 1] == '"') {
                    memmove(osName, osName + 1, strlen(osName) - 2);
                    osName[strlen(osName) - 2] = '\0';
                } else if (osName[strlen(osName) - 1] == '"') {
                    osName[strlen(osName) - 1] = '\0';
                }
            }
        }
        pclose(systemPipe);

        // Print system information within the pad
        mvwprintw(pad, line++, 1, "OS Name: %s", osName);
    } else {
        mvwprintw(pad, line++, 1, "Failed to retrieve system information");
    }

    // Refresh the pad to show the changes within padWin
    prefresh(pad, 0, 0, (rows - windowHeight) / 2 + 2, (cols - windowWidth) / 2 + 3, (rows - windowHeight) / 2 + padWinHeight - 2, (cols - windowWidth) / 2 + padWinWidth - 3);

    // Print instructions at the bottom of mainWin
    mvwprintw(mainWin, windowHeight - 2, 2, "Press 'b' to go back");
    wrefresh(mainWin);

    // Handle scrolling
    int ch;
    int padPos = 0;

    while (1) {
        ch = getch();
        if (ch == 'b' || ch == 'B') { // Press 'B' to go back
            break;
        }
        switch (ch) {
            case KEY_UP:
                if (padPos > 0) padPos--;
                break;
            case KEY_DOWN:
                if (padPos < line - padWinHeight + 2) padPos++;
                break;
        }
        prefresh(pad, padPos, 0, (rows - windowHeight) / 2 + 2, (cols - windowWidth) / 2 + 3, (rows - windowHeight) / 2 + padWinHeight - 2, (cols - windowWidth) / 2 + padWinWidth - 3);
    }

    delwin(pad);
    delwin(padWin);
    delwin(mainWin);

    endwin(); // End ncurses
}