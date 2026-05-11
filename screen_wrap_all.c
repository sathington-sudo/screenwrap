#include <X11/Xlib.h>
#include <X11/extensions/Xinerama.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    // 1. Connect to the X Server
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Cannot open display. Are you running an X11 session?\n");
        return 1;
    }

    // 2. Check for Xinerama (Multi-monitor support extension)
    int event_base, error_base;
    if (!XineramaQueryExtension(display, &event_base, &error_base) || !XineramaIsActive(display)) {
        fprintf(stderr, "Xinerama not active. Per-monitor wrapping requires Xinerama.\n");
        XCloseDisplay(display);
        return 1;
    }

    Window root = DefaultRootWindow(display);
    int screen_count;
    XineramaScreenInfo *screens = XineramaQueryScreens(display, &screen_count);

    if (screen_count < 2) {
        printf("Only one monitor detected. Vertical wrapping will stay on this monitor.\n");
    }

    printf("Cross-Monitor Screen wrap started. Found %d monitors.\n", screen_count);

    Window root_return, child_return;
    int root_x, root_y, win_x, win_y;
    unsigned int mask_return;

    // 3. Main tracking loop
    while (1) {
        // Get the current global mouse coordinates
        XQueryPointer(display, root, &root_return, &child_return,
                      &root_x, &root_y, &win_x, &win_y, &mask_return);

        // Find which screen the mouse is currently on
        for (int i = 0; i < screen_count; i++) {
            if (root_x >= screens[i].x_org && root_x < (screens[i].x_org + screens[i].width) &&
                root_y >= screens[i].y_org && root_y < (screens[i].y_org + screens[i].height)) {
                
                int target_x = root_x;
                int target_y = root_y;
                int moved = 0;

                // Vertical Wrap with Monitor Swap
                if (root_y <= screens[i].y_org) { // Hit the TOP edge
                    int other = (i + 1) % screen_count; 
                    
                    // Proportional mapping to safely land on the other monitor, regardless of width differences
                    float x_ratio = (float)(root_x - screens[i].x_org) / screens[i].width;
                    target_x = screens[other].x_org + (int)(x_ratio * screens[other].width);
                    
                    // Warp to the BOTTOM of the OTHER monitor
                    target_y = screens[other].y_org + screens[other].height - 2;
                    moved = 1;
                } else if (root_y >= screens[i].y_org + screens[i].height - 1) { // Hit the BOTTOM edge
                    int other = (i + 1) % screen_count;
                    
                    // Proportional mapping 
                    float x_ratio = (float)(root_x - screens[i].x_org) / screens[i].width;
                    target_x = screens[other].x_org + (int)(x_ratio * screens[other].width);
                    
                    // Warp to the TOP of the OTHER monitor
                    target_y = screens[other].y_org + 1;
                    moved = 1;
                }

                // Horizontal Wrap (Global bounding box)
                int total_width = DisplayWidth(display, DefaultScreen(display));
                if (root_x <= 0) { // Hit absolute LEFT edge
                    target_x = total_width - 2;
                    moved = 1;
                } else if (root_x >= total_width - 1) { // Hit absolute RIGHT edge
                    target_x = 1;
                    moved = 1;
                }

                // Apply the warp if a boundary was hit
                if (moved) {
                    XWarpPointer(display, None, root, 0, 0, 0, 0, target_x, target_y);
                    XFlush(display); // Force the X server to execute the move immediately
                }
                
                break; // Break the screen-checking loop since we found our active screen
            }
        }
        usleep(10000); // Sleep for 10ms to prevent high CPU usage
    }

    // Cleanup (though unreachable in this infinite loop)
    XFree(screens);
    XCloseDisplay(display);
    return 0;
}
