#include <X11/Xlib.h>
#include <X11/extensions/Xinerama.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Cannot open display. Are you running X11?\n");
        return 1;
    }

    // Check if Xinerama extension is available
    int event_base, error_base;
    if (!XineramaQueryExtension(display, &event_base, &error_base) || !XineramaIsActive(display)) {
        fprintf(stderr, "Xinerama not active. Falling back to global wrap.\n");
        // (Fall back logic could go here, but we want the per-monitor fix)
    }

    Window root = DefaultRootWindow(display);
    int screen_count;
    XineramaScreenInfo *screens = XineramaQueryScreens(display, &screen_count);

    printf("Per-Monitor Screen wrap started. Found %d monitors.\n", screen_count);

    Window root_return, child_return;
    int root_x, root_y, win_x, win_y;
    unsigned int mask_return;

    while (1) {
        XQueryPointer(display, root, &root_return, &child_return,
                      &root_x, &root_y, &win_x, &win_y, &mask_return);

        // Find which monitor the mouse is currently in
        for (int i = 0; i < screen_count; i++) {
            if (root_x >= screens[i].x_org && root_x < (screens[i].x_org + screens[i].width) &&
                root_y >= screens[i].y_org && root_y < (screens[i].y_org + screens[i].height)) {
                
                int target_x = root_x;
                int target_y = root_y;
                int moved = 0;

                // Vertical Wrap (Top/Bottom of THIS monitor)
                if (root_y <= screens[i].y_org) {
                    target_y = screens[i].y_org + screens[i].height - 2;
                    moved = 1;
                } else if (root_y >= screens[i].y_org + screens[i].height - 1) {
                    target_y = screens[i].y_org + 1;
                    moved = 1;
                }

                // Horizontal Wrap (Left/Right edge of the ENTIRE desktop)
                // This ensures you can still wrap from monitor 1 left to monitor 2 right
                int total_width = DisplayWidth(display, DefaultScreen(display));
                if (root_x <= 0) {
                    target_x = total_width - 2;
                    moved = 1;
                } else if (root_x >= total_width - 1) {
                    target_x = 1;
                    moved = 1;
                }

                if (moved) {
                    XWarpPointer(display, None, root, 0, 0, 0, 0, target_x, target_y);
                    XFlush(display);
                }
                break; // Found monitor, no need to check others
            }
        }

        usleep(10000); 
    }

    XFree(screens);
    XCloseDisplay(display);
    return 0;
}
