screen_wrap.c is for left and right wrap only.  screen_wrap_all.c is for up and down as well So if you want to do left and right and up and down, use the commands below but compile screen_wrap_all.c

X11 Screen Wrap

A lightweight, efficient utility for Debian 13 ("Trixie") and other X11-based Linux distributions that enables "infinite loop" mouse movement across dual monitors.
In a standard side-by-side monitor setup, the desktop environment acts as a bounded rectangle. This program removes those boundaries: when your cursor hits the far left or right edge of your combined display area, it instantly warps to the opposite side, allowing for continuous, circular navigation.

Features

Seamless Wrapping: Supports both left-to-right and right-to-left screen edge transitions.
Low Overhead: Written in C with a 10ms polling interval to ensure responsiveness without high CPU usage.
Dynamic Width Detection: Automatically calculates the total width of your combined X11 display.

Prerequisites

Display Server Requirement

You must be running an X11/Xorg session. This utility will not work on Wayland because Wayland’s security model prevents applications from programmatically "warping" the mouse pointer or querying its global position.
To check your current session, run:

echo $XDG_SESSION_TYPE

Development Libraries
To compile the source code on Debian, install the necessary build tools and X11 headers:

sudo apt update && sudo apt install build-essential libx11-dev

Compilation

Use the GNU Compiler Collection (gcc) to build the executable and link the X11 library:

gcc -o screen_wrap screen_wrap.c -lX11

Usage

Manual ExecutionRun the program directly from your terminal to test functionality:

./screen_wrap

Press Ctrl+C in the terminal to stop the program.Background Execution
To run the utility in the background (so it persists after you close your terminal), use nohup:  

nohup ./screen_wrap > /dev/null 2>&1 &

Stopping the Background ProcessTo terminate the program while it is running in the background:  pkill screen_wrap

How it Works

The program utilizes the Xlib library to interface with the X Server. It enters a loop that:Queries the current pointer coordinates using XQueryPointer.Checks if the root_x coordinate is at 0 (far left) or Width - 1 (far right).Invokes XWarpPointer to teleport the cursor to the relative coordinate on the opposite edge.Calls XFlush to ensure the warp command is processed immediately by the display server.

License

This project is open-source and available under the MIT License.
