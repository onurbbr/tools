#include "storage.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

void displayStorageInfo() {
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
    mvwprintw(pad, line++, 1, "Storage Information: ");
    line++;

    FILE *storagePipe = popen("lsblk --fs -J -o LABEL,FSTYPE,SIZE,FSAVAIL,MOUNTPOINT | jq -r '.blockdevices[] | select(.mountpoint != null and (.mountpoint | test(\"^(\\/$|\\/mnt(\\/|$)|\\/boot(\\/|$)|\\/efi(\\/|$)|\\/media(\\/|$)|\\/run\\/media(\\/|$))\"))) | \"Mountpoint: \\(.mountpoint), Size: \\(.size), fsavail: \\(.fsavail), Label: \\(.label), Fstype: \\(.fstype)\"'", "r");
    if (storagePipe) {
        char buffer[256];
        int partitionCount = 1;
        while (fgets(buffer, sizeof(buffer), storagePipe)) {
            // Trim newline character if present
            buffer[strcspn(buffer, "\n")] = 0;
    
            // Parse the line using strtok with comma as delimiter
            char *mountpoint = strtok(buffer, ",");
            char *size = strtok(NULL, ",");
            char *fsavail = strtok(NULL, ",");
            char *label = strtok(NULL, ",");
            char *fstype = strtok(NULL, ",");
    
            // Remove the field names from the values
            mountpoint = strchr(mountpoint, ':') + 2;
            size = strchr(size, ':') + 2;
            fsavail = strchr(fsavail, ':') + 2;
            label = strchr(label, ':') + 2;
            fstype = strchr(fstype, ':') + 2;
    
            // Print the partition information
            mvwprintw(pad, line++, 1, "Partition %d:", partitionCount++);
            mvwprintw(pad, line++, 1, "Name: %s", label);
            mvwprintw(pad, line++, 1, "Filesystem Type: %s", fstype);
            mvwprintw(pad, line++, 1, "Disk Size: %s", size);
            mvwprintw(pad, line++, 1, "Free Space: %s", fsavail);
            mvwprintw(pad, line++, 1, "Mount Point: %s", mountpoint);
            line++; // Add an empty line between partitions
        }
        pclose(storagePipe);
    } else {
        mvwprintw(pad, line++, 1, "Failed to retrieve Storage information.");
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