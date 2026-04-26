#include <X11/Xlib.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    // Connect to the X server
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Cannot open display. Are you running X11?\n");
        return 1;
    }

    Window root = DefaultRootWindow(display);
    
    // Get the total combined width of your monitors
    int width = DisplayWidth(display, DefaultScreen(display));

    Window root_return, child_return;
    int root_x, root_y, win_x, win_y;
    unsigned int mask_return;

    printf("Screen wrap started. Total width: %d. Press Ctrl+C to exit.\n", width);

    // Infinite polling loop
    while (1) {
        // Query the current mouse position
        XQueryPointer(display, root, &root_return, &child_return,
                      &root_x, &root_y, &win_x, &win_y, &mask_return);

        // If hitting the left edge, warp to the right edge
        if (root_x <= 0) {
            XWarpPointer(display, None, root, 0, 0, 0, 0, width - 2, root_y);
            XFlush(display); // Force the command to execute immediately
        } 
        // If hitting the right edge, warp to the left edge
        else if (root_x >= width - 1) {
            XWarpPointer(display, None, root, 0, 0, 0, 0, 1, root_y);
            XFlush(display);
        }

        // Sleep for 10 milliseconds to prevent pegging the CPU at 100%
        usleep(10000); 
    }

    XCloseDisplay(display);
    return 0;
}
