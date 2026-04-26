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
    
    // Get the total combined width and height of your desktop
    int width = DisplayWidth(display, DefaultScreen(display));
    int height = DisplayHeight(display, DefaultScreen(display));

    Window root_return, child_return;
    int root_x, root_y, win_x, win_y;
    unsigned int mask_return;

    printf("4-Way Screen wrap started.\n");
    printf("Resolution: %dx%d. Press Ctrl+C to exit.\n", width, height);

    while (1) {
        XQueryPointer(display, root, &root_return, &child_return,
                      &root_x, &root_y, &win_x, &win_y, &mask_return);

        int target_x = root_x;
        int target_y = root_y;
        int moved = 0;

        // Horizontal Wrap (Left/Right)
        if (root_x <= 0) {
            target_x = width - 2;
            moved = 1;
        } else if (root_x >= width - 1) {
            target_x = 1;
            moved = 1;
        }

        // Vertical Wrap (Up/Down)
        if (root_y <= 0) {
            target_y = height - 2;
            moved = 1;
        } else if (root_y >= height - 1) {
            target_y = 1;
            moved = 1;
        }

        // Only warp if a boundary was hit
        if (moved) {
            XWarpPointer(display, None, root, 0, 0, 0, 0, target_x, target_y);
            XFlush(display);
        }

        usleep(10000); 
    }

    XCloseDisplay(display);
    return 0;
}
