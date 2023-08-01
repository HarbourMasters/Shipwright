#include <menu.h>
#include <pthread.h>
#include <time.h>

int timeSyncEnabled = 0;
pthread_t timeSyncThreadId;

void syncWithRealTime() {
    // Get the current real-world time
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Convert the real-world time to the in-game time
    // We assume that each hour corresponds to 1000 in-game time units
    // and each minute corresponds to approximately 16.67 in-game time units
    int gameTime = (tm.tm_hour * 1000) + (tm.tm_min * (1000 / 60));

    // Update other game mechanics as necessary...
    // For example, if the game has a day-night cycle, you might update the lighting conditions here
}

void* timeSyncThread(void* arg) {
    struct timespec delay;
    delay.tv_sec = 1; 
    delay.tv_nsec = 0;

    while (1) {
        syncWithRealTime();
        nanosleep(&delay, NULL);
    }

    return NULL;
}

void startTimeSync() {
    pthread_create(&timeSyncThreadId, NULL, timeSyncThread, NULL);
}

void stopTimeSync() {
    pthread_cancel(timeSyncThreadId);
}

int main() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Initialize items
    ITEM** items = (ITEM **)calloc(2, sizeof(ITEM *));
    items[0] = new_item("Time Sync", "Toggle real-world time sync");
    items[1] = NULL;

    // Create menu
    MENU* menu = new_menu((ITEM **)items);
    mvprintw(LINES - 2, 0, "Press <ENTER> to toggle time sync and 'q' to quit");
    post_menu(menu);
    refresh();

    // Main loop
    int c;
    while((c = getch()) != 'q') {   
        switch(c) {   
            case KEY_DOWN:
                menu_driver(menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(menu, REQ_UP_ITEM);
                break;
            case 10: // Enter key
                if (item_name(current_item(menu)) == "Time Sync") {
                    if (timeSyncEnabled) {
                        stopTimeSync();
                    } else {
                        startTimeSync();
                    }
                    timeSyncEnabled = !timeSyncEnabled;
                }
                break;
        }   
    }

    // Clean up and exit
    unpost_menu(menu);
    free_menu(menu);
    for(int i = 0; i < 1; ++i)
        free_item(items[i]);
    endwin();
}
