#include "cpu.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

void displayCpuInfo() {
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
    mvwprintw(pad, line++, 1, "CPU Information: ");
    line++;

    FILE *cpuPipe = popen("lscpu | grep -E '^Architecture:|^CPU\\(s\\):|^Vendor ID:|^Model name:|^Thread\\(s\\) per core:|^Core\\(s\\) per socket:|^CPU max MHz:|^CPU min MHz:|^Virtualization:' | awk -F: '{print $2}' | sed 's/^ *//'", "r");
    if (cpuPipe) {
        char buffer[256];
        char vendorID[256] = "";
        char architecture[256] = "";
        char cpus[256] = "";
        char modelName[256] = "";
        char threadsPerCore[256] = "";
        char coresPerSocket[256] = "";
        char cpuMaxMHz[256] = "";
        char cpuMinMHz[256] = "";
        char virtualization[256] = "";
        int lineCount = 0;
        while (fgets(buffer, sizeof(buffer), cpuPipe)) {
            // Trim newline character if present
            buffer[strcspn(buffer, "\n")] = 0;

            switch (lineCount) {
                case 0:
                    strcpy(architecture, buffer);
                    break;
                case 1:
                    strcpy(cpus, buffer);
                    break;
                case 2:
                    strcpy(vendorID, buffer);
                    break;
                case 3:
                    strcpy(modelName, buffer);
                    break;
                case 4:
                    strcpy(threadsPerCore, buffer);
                    break;
                case 5:
                    strcpy(coresPerSocket, buffer);
                    break;
                case 6:
                    strcpy(cpuMaxMHz, buffer);
                    break;
                case 7:
                    strcpy(cpuMinMHz, buffer);
                    break;
                case 8:
                    strcpy(virtualization, buffer);
                    break;
            }
            lineCount++;
        }
        pclose(cpuPipe);

        // Extract model and base clock from modelName
        char model[256] = "";
        char baseClock[256] = "";
        sscanf(modelName, "Intel(R) Core(TM) %s CPU @ %s", model, baseClock);

        // Convert base clock to GHz
        double baseClockGHz = atof(baseClock);

        // Convert CPU max and min MHz to GHz
        double cpuMaxGHz = atof(cpuMaxMHz) / 1000.0;
        double cpuMinGHz = atof(cpuMinMHz) / 1000.0;

        // Determine vendor name
        char vendorName[256] = "";
        if (strcmp(vendorID, "GenuineIntel") == 0) {
            strcpy(vendorName, "Intel Corporation");
        } else {
            strcpy(vendorName, vendorID);
        }

        // Print CPU information within the pad
        mvwprintw(pad, line++, 1, "Vendor: %s", vendorName);
        mvwprintw(pad, line++, 1, "Model: %s", model);
        mvwprintw(pad, line++, 1, "Base Clock Speed: %.2f GHz", baseClockGHz);
        mvwprintw(pad, line++, 1, "Min Clock Speed: %.2f GHz", cpuMinGHz);
        mvwprintw(pad, line++, 1, "Max Clock Speed: %.2f GHz", cpuMaxGHz);
        mvwprintw(pad, line++, 1, "Architecture: %s", architecture);
        mvwprintw(pad, line++, 1, "CPU(s): %s", cpus);
        mvwprintw(pad, line++, 1, "Core(s) per socket: %s", coresPerSocket);
        mvwprintw(pad, line++, 1, "Thread(s) per core: %s", threadsPerCore);
        mvwprintw(pad, line++, 1, "Virtualization: %s", virtualization);
    } else {
        mvwprintw(pad, line++, 1, "Failed to retrieve CPU information");
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