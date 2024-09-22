#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "user.h"

void displayUserInfo() {
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

    FILE* envPipe = popen("env", "r");
    if (envPipe) {
        char envBuffer[256];
        char user[256] = "";
        char hostname[256] = "";
        char homeFolder[256] = "";
        char shell[256] = "";

        while (fgets(envBuffer, sizeof(envBuffer), envPipe)) {
            // Trim newline character if present
            envBuffer[strcspn(envBuffer, "\n")] = 0;

            // Parse the environment variable
            if (strncmp(envBuffer, "USER=", 5) == 0) {
                strncpy(user, envBuffer + 5, sizeof(user) - 1);
            } else if (strncmp(envBuffer, "HOSTNAME=", 9) == 0) {
                strncpy(hostname, envBuffer + 9, sizeof(hostname) - 1);
            } else if (strncmp(envBuffer, "HOME=", 5) == 0) {
                strncpy(homeFolder, envBuffer + 5, sizeof(homeFolder) - 1);
            } else if (strncmp(envBuffer, "SHELL=", 6) == 0) {
                strncpy(shell, envBuffer + 6, sizeof(shell) - 1);
            }
        }
        pclose(envPipe);

        mvwprintw(pad, line++, 1, "User: %s", user);
        mvwprintw(pad, line++, 1, "Hostname: %s", hostname);
        mvwprintw(pad, line++, 1, "Home Folder: %s", homeFolder);
        mvwprintw(pad, line++, 1, "Shell: %s", shell); // Add shell information
    } else {
        mvwprintw(pad, line++, 1, "Failed to retrieve environment information");
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
