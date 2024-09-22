#include "gpu.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void displayGpuInfo() {
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
    mvwprintw(pad, line++, 1, "GPU Information: ");
    line++;

    FILE* gpuPipe = popen("nvidia-smi --query-gpu=name,driver_version,memory.total,memory.used,memory.free,utilization.gpu,utilization.memory,temperature.gpu --format=csv,noheader,nounits", "r");    
    if (gpuPipe) {
        char gpuBuffer[256];
        char gpuVendor[256] = "NVIDIA"; // Assuming NVIDIA as the vendor
        char gpuModel[256] = "";
        char driverVersion[256] = "";
        int memoryTotal = 0;
        int memoryUsed = 0;
        int memoryFree = 0;
        int utilizationGpu = 0;
        int utilizationMemory = 0;
        int temperatureGpu = 0;
    
        while (fgets(gpuBuffer, sizeof(gpuBuffer), gpuPipe)) {
            // Trim newline character if present
            gpuBuffer[strcspn(gpuBuffer, "\n")] = 0;
    
            // Parse the CSV line
            sscanf(gpuBuffer, "%[^,], %[^,], %d, %d, %d, %d, %d, %d",
                   gpuModel, driverVersion, &memoryTotal, &memoryUsed, &memoryFree,
                   &utilizationGpu, &utilizationMemory, &temperatureGpu);
        }
        pclose(gpuPipe);
    
        // Remove "NVIDIA " prefix from gpuModel if present
        if (strncmp(gpuModel, "NVIDIA ", 7) == 0) {
            memmove(gpuModel, gpuModel + 7, strlen(gpuModel) - 6);
        }
    
        // Print GPU information within the pad
        mvwprintw(pad, line++, 1, "GPU Vendor: %s", gpuVendor);
        mvwprintw(pad, line++, 1, "GPU Model: %s", gpuModel);
        mvwprintw(pad, line++, 1, "Driver Version: %s", driverVersion);
        mvwprintw(pad, line++, 1, "Total Memory: %d MiB", memoryTotal);
        mvwprintw(pad, line++, 1, "Used Memory: %d MiB", memoryUsed);
        mvwprintw(pad, line++, 1, "Free Memory: %d MiB", memoryFree);
        mvwprintw(pad, line++, 1, "GPU Utilization: %d%%", utilizationGpu);
        mvwprintw(pad, line++, 1, "Memory Utilization: %d%%", utilizationMemory);
        mvwprintw(pad, line++, 1, "GPU Temperature: %d°C", temperatureGpu);
    } else {
        mvwprintw(pad, line++, 1, "Failed to retrieve GPU information");
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