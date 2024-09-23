#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "de.h"

void displayDEInfo() {
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
    mvwprintw(pad, line++, 1, "Desktop Environment Information: ");
    line++;

    FILE* dePipe = popen("env", "r");
    if (dePipe) {
        char deBuffer[256];
        char sessionType[256] = "";
        char sessionDesktop[256] = "";

        while (fgets(deBuffer, sizeof(deBuffer), dePipe)) {
            // Trim newline character if present
            deBuffer[strcspn(deBuffer, "\n")] = 0;

            // Parse the environment variable line
            sscanf(deBuffer, "XDG_SESSION_TYPE=%s", sessionType);
            sscanf(deBuffer, "XDG_CURRENT_DESKTOP=%s", sessionDesktop);
        }
        pclose(dePipe);

        // Check if XDG_CURRENT_DESKTOP is KDE
        if (strcmp(sessionDesktop, "KDE") == 0) {
            char command[100];
            sprintf(command, "plasmashell --version | awk '{print $2}'");
            FILE* versionPipe = popen(command, "r");
            if (versionPipe) {
                char versionBuffer[256];
                fgets(versionBuffer, sizeof(versionBuffer), versionPipe);
                pclose(versionPipe);
                mvwprintw(pad, line++, 1, "DE: %s", sessionDesktop);
                mvwprintw(pad, line++, 1, "WM: KWin");
                mvwprintw(pad, line++, 1, "%s Version: %s", sessionDesktop, versionBuffer);
                mvwprintw(pad, line++, 1, "Session Type: %s", sessionType);
            }
        } else if (strcmp(sessionDesktop, "GNOME") == 0) {
            char command[100];
            sprintf(command, "gnome-shell --version | awk '{print $3}'");
            FILE* versionPipe = popen(command, "r");
            if (versionPipe) {
                char versionBuffer[256];
                fgets(versionBuffer, sizeof(versionBuffer), versionPipe);
                pclose(versionPipe);
                mvwprintw(pad, line++, 1, "DE: %s", sessionDesktop);
                mvwprintw(pad, line++, 1, "WM: Mutter");
                mvwprintw(pad, line++, 1, "%s Version: %s", sessionDesktop, versionBuffer);
                mvwprintw(pad, line++, 1, "Session Type: %s", sessionType);
            }
        }
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
