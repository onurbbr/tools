#include "system.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void displaySystemInfo()
{
  initscr();
  noecho();
  curs_set(FALSE);
  keypad(stdscr, TRUE);

  int rows, cols;
  getmaxyx(stdscr, rows, cols); // Get the dimensions of the terminal window

  int windowHeight = (rows * 8) / 10; // Height of the main window
  int windowWidth = (cols * 8) / 10;  // Width of the main window

  // Create a centered main window
  WINDOW *mainWin = newwin(windowHeight, windowWidth, (rows - windowHeight) / 2,
                           (cols - windowWidth) / 2);
  box(mainWin, 0, 0); // Draw a box around the main window
  wrefresh(mainWin);

  // Create a window to hold the pad and add a box around it
  int padWinHeight = windowHeight - 4; // Adjust for border and title
  int padWinWidth = windowWidth - 4;   // Adjust for border
  WINDOW *padWin =
      newwin(padWinHeight + 1, padWinWidth, (rows - windowHeight) / 2 + 1,
             (cols - windowWidth) / 2 + 2);
  box(padWin, 0, 0);
  wrefresh(padWin);

  // Create a pad to hold the content
  int padHeight = 100;            // Adjust this value as needed
  int padWidth = padWinWidth - 2; // Adjust for border
  WINDOW *pad = newpad(padHeight, padWidth);

  int line = 0;
  mvwprintw(pad, line++, 1, "System Information ");
  line++;

  mvwprintw(pad, line++, 1, "Distro Information: ");
  line++;

  FILE *systemPipe = popen("cat /etc/os-release | grep -w \"NAME\"", "r");
  if (systemPipe)
  {
    char systemBuffer[256];
    char osName[256] = "";

    while (fgets(systemBuffer, sizeof(systemBuffer), systemPipe))
    {
      // Trim newline character if present
      systemBuffer[strcspn(systemBuffer, "\n")] = 0;

      // Parse the OS name
      if (strncmp(systemBuffer, "NAME=", 5) == 0)
      {
        strncpy(osName, systemBuffer + 6, sizeof(osName) - 1);
        // Remove surrounding quotes if present
        if (osName[0] == '"' && osName[strlen(osName) - 1] == '"')
        {
          memmove(osName, osName + 1, strlen(osName) - 2);
          osName[strlen(osName) - 2] = '\0';
        }
        else if (osName[strlen(osName) - 1] == '"')
        {
          osName[strlen(osName) - 1] = '\0';
        }
      }
    }

    // Print system information within the pad
    mvwprintw(pad, line++, 1, "OS Name: %s", osName);

    int rpmCount = 0;
    int debCount = 0;
    int pacmanCount = 0;
    int flatpakCount = 0;
    int snapCount = 0;

    if (strstr(osName, "Fedora") != NULL || strstr(osName, "Red Hat") != NULL ||
        strstr(osName, "openSUSE") != NULL)
    {
      FILE *rpmPipe = popen("rpm -qa | wc -l", "r");
      if (rpmPipe)
      {
        char rpmBuffer[256];

        if (fgets(rpmBuffer, sizeof(rpmBuffer), rpmPipe))
        {
          rpmCount = atoi(rpmBuffer);
        }
        pclose(rpmPipe);
      }
    }
    else if (strstr(osName, "Debian") != NULL ||
             strstr(osName, "Ubuntu") != NULL)
    {
      FILE *debPipe = popen("dpkg -l | wc -l", "r");
      if (debPipe)
      {
        char debBuffer[256];

        if (fgets(debBuffer, sizeof(debBuffer), debPipe))
        {
          debCount = atoi(debBuffer);
        }
        pclose(debPipe);
      }
    }
    else if (strstr(osName, "Arch") != NULL)
    {
      FILE *pacmanPipe = popen("pacman -Q | wc -l", "r");
      if (pacmanPipe)
      {
        char pacmanBuffer[256];

        if (fgets(pacmanBuffer, sizeof(pacmanBuffer), pacmanPipe))
        {
          pacmanCount = atoi(pacmanBuffer);
        }
        pclose(pacmanPipe);
      }
    }

    if (system("/bin/sh -c '[ -f /usr/bin/flatpak ]'") == 0)
    {
      FILE *flatpakPipe = popen("flatpak list --app | wc -l", "r");
      if (flatpakPipe)
      {
        char flatpakBuffer[256];

        if (fgets(flatpakBuffer, sizeof(flatpakBuffer), flatpakPipe))
        {
          flatpakCount = atoi(flatpakBuffer);
        }
        pclose(flatpakPipe);
      }
    }

    if (system("/bin/sh -c '[ -f /usr/bin/snap ]'") == 0)
    {
      FILE *snapPipe =
          popen("snap list | awk '{print $1}' | tail -n +2 | wc -l", "r");
      if (snapPipe)
      {
        char snapBuffer[256];

        if (fgets(snapBuffer, sizeof(snapBuffer), snapPipe))
        {
          snapCount = atoi(snapBuffer);
        }
        pclose(snapPipe);
      }
    }

    if (flatpakCount == 0 && snapCount == 0)
    {
      if (strstr(osName, "Fedora") != NULL ||
          strstr(osName, "Red Hat") != NULL ||
          strstr(osName, "openSUSE") != NULL)
      {
        mvwprintw(pad, line++, 1, "Packages: %d (rpm)", rpmCount);
      }
      else if (strstr(osName, "Debian") != NULL ||
               strstr(osName, "Ubuntu") != NULL)
      {
        mvwprintw(pad, line++, 1, "Packages: %d (dpkg)", debCount);
      }
      else if (strstr(osName, "Arch") != NULL)
      {
        mvwprintw(pad, line++, 1, "Packages: %d (pacman)", pacmanCount);
      }
    }
    else if (flatpakCount == 0)
    {
      if (strstr(osName, "Fedora") != NULL ||
          strstr(osName, "Red Hat") != NULL ||
          strstr(osName, "openSUSE") != NULL)
      {
        mvwprintw(pad, line++, 1, "Packages: %d (rpm), %d (snap)", rpmCount,
                  snapCount);
      }
      else if (strstr(osName, "Debian") != NULL ||
               strstr(osName, "Ubuntu") != NULL)
      {
        mvwprintw(pad, line++, 1, "Packages: %d (dpkg), %d (snap)", debCount,
                  snapCount);
      }
      else if (strstr(osName, "Arch") != NULL)
      {
        mvwprintw(pad, line++, 1, "Packages: %d (pacman), %d (snap)",
                  pacmanCount, snapCount);
      }
    }
    else if (snapCount == 0)
    {
      if (strstr(osName, "Fedora") != NULL ||
          strstr(osName, "Red Hat") != NULL ||
          strstr(osName, "openSUSE") != NULL)
      {
        mvwprintw(pad, line++, 1, "Packages: %d (rpm), %d (flatpak)", rpmCount,
                  flatpakCount);
      }
      else if (strstr(osName, "Debian") != NULL ||
               strstr(osName, "Ubuntu") != NULL)
      {
        mvwprintw(pad, line++, 1, "Packages: %d (dpkg), %d (flatpak)", debCount,
                  flatpakCount);
      }
      else if (strstr(osName, "Arch") != NULL)
      {
        mvwprintw(pad, line++, 1, "Packages: %d (pacman), %d (flatpak)",
                  pacmanCount, flatpakCount);
      }
    }
    else
    {
      if (strstr(osName, "Fedora") != NULL ||
          strstr(osName, "Red Hat") != NULL ||
          strstr(osName, "openSUSE") != NULL)
      {
        mvwprintw(pad, line++, 1, "Packages: %d (rpm), %d (flatpak), %d (snap)",
                  rpmCount, flatpakCount, snapCount);
      }
      else if (strstr(osName, "Debian") != NULL ||
               strstr(osName, "Ubuntu") != NULL)
      {
        mvwprintw(pad, line++, 1,
                  "Packages: %d (dpkg), %d (flatpak), %d (snap)", debCount,
                  flatpakCount, snapCount);
      }
      else if (strstr(osName, "Arch") != NULL)
      {
        mvwprintw(pad, line++, 1,
                  "Packages: %d (pacman), %d (flatpak), %d (snap)", pacmanCount,
                  flatpakCount, snapCount);
      }
    }

    line++;
    mvwprintw(pad, line++, 1, "Network Information:");
    line++;

    FILE *networkPipe = popen(
        "ip link show | grep \"state UP\" | awk '{print $2}' | sed 's/:$//'", "r");
    if (networkPipe)
    {
      char adapter[256] = "";
      fgets(adapter, sizeof(adapter), networkPipe);
      adapter[strcspn(adapter, "\n")] = 0;

      // nmcli kontrolü yapılıyor
      FILE *nmcliCheck = popen("which nmcli > /dev/null 2>&1", "r");
      if (nmcliCheck)
      {
        int nmcliExists = pclose(nmcliCheck); // nmcli komutunun var olup olmadığını kontrol eder

        if (nmcliExists == 0) // nmcli mevcutsa
        {
          char nmcliCommand[512];
          snprintf(nmcliCommand, sizeof(nmcliCommand), "nmcli device show %s", adapter);
          FILE *nmcliPipe = popen(nmcliCommand, "r");

          if (nmcliPipe)
          {
            char nmcliBuffer[256];
            char device[256] = "";
            char type[256] = "";
            char hwaddr[256] = "";
            char state[256] = "";
            char connection[256] = "";
            char ipAddress[256] = "";

            // nmcli çıktısını okuma ve ayrıştırma
            while (fgets(nmcliBuffer, sizeof(nmcliBuffer), nmcliPipe))
            {
              nmcliBuffer[strcspn(nmcliBuffer, "\n")] = 0; // Satırdaki yeni satır karakterini kaldır

              // İlgili bilgileri ayrıştır
              if (strncmp(nmcliBuffer, "GENERAL.DEVICE:", 15) == 0)
              {
                sscanf(nmcliBuffer, "GENERAL.DEVICE: %s", device);
              }
              else if (strncmp(nmcliBuffer, "GENERAL.TYPE:", 13) == 0)
              {
                sscanf(nmcliBuffer, "GENERAL.TYPE: %s", type);
              }
              else if (strncmp(nmcliBuffer, "GENERAL.HWADDR:", 15) == 0)
              {
                sscanf(nmcliBuffer, "GENERAL.HWADDR: %s", hwaddr);
              }
              else if (strncmp(nmcliBuffer, "GENERAL.STATE:", 14) == 0)
              {
                sscanf(nmcliBuffer, "GENERAL.STATE: %s", state);
              }
              else if (strncmp(nmcliBuffer, "GENERAL.CONNECTION:", 19) == 0)
              {
                sscanf(nmcliBuffer, "GENERAL.CONNECTION: %[^,]", connection);
              }
              else if (strncmp(nmcliBuffer, "IP4.ADDRESS[1]:", 15) == 0)
              {
                sscanf(nmcliBuffer, "IP4.ADDRESS[1]: %[^/]", ipAddress);
              }
            }
            pclose(nmcliPipe);

            if (strcmp(type, "wifi") == 0)
            {
              mvwprintw(pad, line++, 1, "Device: %s", device[0] ? device : "N/A");
              mvwprintw(pad, line++, 1, "Adaptor Type: %s", type[0] ? type : "N/A");
              mvwprintw(pad, line++, 1, "MAC Address: %s", hwaddr[0] ? hwaddr : "N/A");
              mvwprintw(pad, line++, 1, "Connection: %s", connection[0] ? connection : "N/A");
              mvwprintw(pad, line++, 1, "Quality: %s", state[0] ? state : "N/A");
              mvwprintw(pad, line++, 1, "IP Address: %s", ipAddress[0] ? ipAddress : "N/A");
            }
            else if (strcmp(type, "ethernet") == 0)
            {
              mvwprintw(pad, line++, 1, "Device: %s", device[0] ? device : "N/A");
              mvwprintw(pad, line++, 1, "Type: %s", type[0] ? type : "N/A");
              mvwprintw(pad, line++, 1, "MAC Address: %s", hwaddr[0] ? hwaddr : "N/A");
              mvwprintw(pad, line++, 1, "Quality: %s", state[0] ? state : "N/A");
            }
            else
            {
              mvwprintw(pad, line++, 1, "Unknown device type: %s", type);
            }
          }
          else
          {
            mvwprintw(pad, line++, 1, "Failed to retrieve nmcli information for adapter");
          }
        }
        else
        {
          mvwprintw(pad, line++, 1, "nmcli is not installed on this system");
        }
      }
      else
      {
        mvwprintw(pad, line++, 1, "Failed to check nmcli availability");
      }

      pclose(networkPipe);
    }
    else
    {
      mvwprintw(pad, line++, 1, "Failed to retrieve adapter information");
    }

    line++;
    mvwprintw(pad, line++, 1, "Security Information:");
    line++;

    FILE *securityPipe = popen("sh -c 'if which sestatus > /dev/null 2>&1; then if sestatus | grep \"SELinux status:\" | awk \"{print $2}\" | grep -q \"enabled\"; then echo \"selinux\"; else echo \"none\"; fi; elif [ -f /sys/module/apparmor/parameters/enabled ] && [ \"$(cat /sys/module/apparmor/parameters/enabled)\" = \"Y\" ]; then echo \"apparmor\"; else echo \"none\"; fi'", "r");

    if (securityPipe)
    {
      char securityBuffer[256];
      fgets(securityBuffer, sizeof(securityBuffer), securityPipe);
      pclose(securityPipe);

      // Yeni satır karakterini kes
      securityBuffer[strcspn(securityBuffer, "\n")] = 0;

      // Güvenlik bilgisi ile ilgili olarak "mvwprintw" kullan
      if (strcmp(securityBuffer, "selinux") == 0)
      {
        char policyName[256];
        char currentMode[256];
        char memoryProtection[256];
        char isEnabled[256];
        char selinuxBuffer[256];

        FILE *selinuxPipe = popen("sestatus", "r");

        if (selinuxPipe)
        {
          while (fgets(selinuxBuffer, sizeof(selinuxBuffer), selinuxPipe))
          {
            if (strncmp(selinuxBuffer, "Current mode:", 13) == 0)
              sscanf(selinuxBuffer, "Current mode: %s", currentMode);
            else if (strncmp(selinuxBuffer, "Memory protection checking:", 24) == 0)
              sscanf(selinuxBuffer, "Memory protection checking: %[^,]", memoryProtection);
            else if (strncmp(selinuxBuffer, "Loaded policy name:", 19) == 0)
              sscanf(selinuxBuffer, "Loaded policy name: %[^,]", policyName);
            else if (strncmp(selinuxBuffer, "SELinux status:", 15) == 0)
              sscanf(selinuxBuffer, "SELinux status: %[^,]", isEnabled);
          }

          pclose(selinuxPipe);

          mvwprintw(pad, line++, 1, "Security Module: %s", securityBuffer);
          mvwprintw(pad, line++, 1, "Module Status: %s", isEnabled);
          mvwprintw(pad, line++, 1, "Current mode: %s", currentMode);
          mvwprintw(pad, line++, 1, "Memory protection: %s", memoryProtection);
          mvwprintw(pad, line++, 1, "Policy name: %s", policyName);
        }
        else
          mvwprintw(pad, line++, 1, "Failed to check SELinux status");
      }
      else if (strcmp(securityBuffer, "apparmor") == 0)
      {
        mvwprintw(pad, line++, 1, "Security: AppArmor is enabled");
      }
      else
        mvwprintw(pad, line++, 1, "Security: None");
    }
    else
      mvwprintw(pad, line++, 1, "Failed to check security stack");

    // Refresh the pad to show the changes within padWin
    prefresh(pad, 0, 0, (rows - windowHeight) / 2 + 2,
             (cols - windowWidth) / 2 + 3,
             (rows - windowHeight) / 2 + padWinHeight - 2,
             (cols - windowWidth) / 2 + padWinWidth - 3);

    // Print instructions at the bottom of mainWin
    mvwprintw(mainWin, windowHeight - 2, 2, "Press 'b' to go back");
    wrefresh(mainWin);

    // Handle scrolling
    int ch;
    int padPos = 0;

    while (1)
    {
      ch = getch();
      if (ch == 'b' || ch == 'B')
      { // Press 'B' to go back
        break;
      }
      switch (ch)
      {
      case KEY_UP:
        if (padPos > 0)
          padPos--;
        break;
      case KEY_DOWN:
        if (padPos < line - padWinHeight + 2)
          padPos++;
        break;
      }
      prefresh(pad, padPos, 0, (rows - windowHeight) / 2 + 2,
               (cols - windowWidth) / 2 + 3,
               (rows - windowHeight) / 2 + padWinHeight - 2,
               (cols - windowWidth) / 2 + padWinWidth - 3);
    }

    delwin(pad);
    delwin(padWin);
    delwin(mainWin);

    endwin(); // End ncurses
  }
}
