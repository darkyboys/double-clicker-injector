/*
 * MIT License
 *
 * Copyright (c) 2025 GHGLTGGAMER
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

// Written by ghgltggamer
// Happy PvP for non double clicker mice users , Tip don't goo insaane like a 100 cps that's crazy enough to make servers ban your existance from there land entirely without even telling you.

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <cstring>
#include <cstdlib>
#include <chrono>

void emit_event(int fd, uint16_t type, uint16_t code, int32_t value) {
    struct input_event ie;
    memset(&ie, 0, sizeof(ie));
    ie.type = type;
    ie.code = code;
    ie.value = value;
    // Use steady_clock for timestamp
    auto now = std::chrono::steady_clock::now();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    ie.time.tv_sec = us / 1000000;
    ie.time.tv_usec = us % 1000000;

    if (write(fd, &ie, sizeof(ie)) < 0) {
        perror("Error writing event");
        exit(1);
    }
}

void emit_click(int fd, int button) {
    emit_event(fd, EV_KEY, button, 1); // press
    emit_event(fd, EV_SYN, SYN_REPORT, 0);

    emit_event(fd, EV_KEY, button, 0); // release
    emit_event(fd, EV_SYN, SYN_REPORT, 0);
}

int main(int argc, char* argv[]) {
    const char* device = nullptr;
    int cps = 10;  // default clicks per second

    // Parse CLI args
    for (int i = 1; i < argc; ++i) {
        if ((strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--device") == 0) && i + 1 < argc) {
            device = argv[++i];
        } else if ((strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--cps") == 0) && i + 1 < argc) {
            cps = std::atoi(argv[++i]);
            if (cps <= 0) {
                std::cerr << "CPS must be positive\n";
                return 1;
            }
        } else {
            std::cerr << "Unknown argument: " << argv[i] << "\n";
            return 1;
        }
    }

    if (!device) {
        std::cerr << "Usage: " << argv[0] << " -d <device_path> [-c <clicks_per_second>]\n";
        return 1;
    }

    int uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinput_fd < 0) {
        perror("Failed to open /dev/uinput");
        return 1;
    }

    // Enable mouse button events
    if (ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY) < 0 ||
        ioctl(uinput_fd, UI_SET_KEYBIT, BTN_LEFT) < 0 ||
        ioctl(uinput_fd, UI_SET_KEYBIT, BTN_RIGHT) < 0) {
        perror("ioctl failed");
    close(uinput_fd);
    return 1;
        }

        if (ioctl(uinput_fd, UI_SET_EVBIT, EV_SYN) < 0) {
            perror("ioctl failed");
            close(uinput_fd);
            return 1;
        }

        struct uinput_user_dev uidev;
        memset(&uidev, 0, sizeof(uidev));
        snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "double_click_injector");
        uidev.id.bustype = BUS_USB;
        uidev.id.vendor = 0x1234;
        uidev.id.product = 0x5678;
        uidev.id.version = 1;

        if (write(uinput_fd, &uidev, sizeof(uidev)) < 0) {
            perror("Failed to write uinput_user_dev");
            close(uinput_fd);
            return 1;
        }

        if (ioctl(uinput_fd, UI_DEV_CREATE) < 0) {
            perror("Failed to create uinput device");
            close(uinput_fd);
            return 1;
        }

        std::cout << "Double click injector running on device: " << device << " with CPS: " << cps << "\n";

        // Calculate delay in microseconds between virtual clicks
        unsigned int delay_us = 1000000 / cps;

        // We'll listen to real mouse events and inject virtual clicks on left button press
        int real_fd = open(device, O_RDONLY);
        if (real_fd < 0) {
            perror("Failed to open input device");
            ioctl(uinput_fd, UI_DEV_DESTROY);
            close(uinput_fd);
            return 1;
        }

        struct input_event ev;
        while (true) {
            ssize_t n = read(real_fd, &ev, sizeof(ev));
            if (n == (ssize_t)sizeof(ev)) {
                if (ev.type == EV_KEY && ev.code == BTN_LEFT && ev.value == 1) {
                    // Physical left click detected, inject double click with delay
                    emit_click(uinput_fd, BTN_LEFT);
                    usleep(delay_us);
                    emit_click(uinput_fd, BTN_LEFT);
                    std::cout << "Injected double click with delay " << delay_us / 1000 << " ms\n";
                }
            }
        }

        ioctl(uinput_fd, UI_DEV_DESTROY);
        close(uinput_fd);
        close(real_fd);

        return 0;
}
