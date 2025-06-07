#include <fcntl.h>
#include <linux/uinput.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <sys/time.h>
#include <sys/ioctl.h>

void emit(int fd, uint16_t type, uint16_t code, int32_t value) {
    struct input_event ev {};
    gettimeofday(&ev.time, nullptr);
    ev.type = type;
    ev.code = code;
    ev.value = value;
    write(fd, &ev, sizeof(ev));
}

void click(int fd, uint16_t btn) {
    emit(fd, EV_KEY, btn, 1); // press
    emit(fd, EV_SYN, SYN_REPORT, 0);
    usleep(1000);             // hold 1ms

    emit(fd, EV_KEY, btn, 0); // release
    emit(fd, EV_SYN, SYN_REPORT, 0);
}

int setup_uinput_mouse() {
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("open /dev/uinput");
        return -1;
    }

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);

    struct uinput_user_dev uidev {};
    memset(&uidev, 0, sizeof(uidev));
    strcpy(uidev.name, "virtual_double_click_mouse");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = 0x1234;
    uidev.id.product = 0x5678;
    uidev.id.version = 1;

    write(fd, &uidev, sizeof(uidev));
    if (ioctl(fd, UI_DEV_CREATE) < 0) {
        perror("ioctl UI_DEV_CREATE");
        close(fd);
        return -1;
    }

    sleep(1); // Give time to register device
    return fd;
}

int main() {
    // CHANGE THIS to your actual mouse device from evtest:
    const char* dev = "/dev/input/event3";

    int fd = setup_uinput_mouse();
    if (fd < 0) return 1;

    int input_fd = open(dev, O_RDONLY);
    if (input_fd < 0) {
        perror("open input device");
        ioctl(fd, UI_DEV_DESTROY);
        close(fd);
        return 1;
    }

    std::cout << "Running kernel-level double-click injector.\n";
    std::cout << "Listening on " << dev << std::endl;

    struct input_event ev;
    while (true) {
        ssize_t n = read(input_fd, &ev, sizeof(ev));
        if (n != sizeof(ev)) continue;

        // Check for mouse button press events only
        if (ev.type == EV_KEY && ev.value == 1) {
            if (ev.code == BTN_LEFT || ev.code == BTN_RIGHT) {
                // Spawn thread to send double click after delay
                std::thread([fd, code = ev.code]() {
                    usleep(15000); // 15 ms delay between clicks
                    click(fd, code);
                }).detach();
            }
        }
    }

    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
    return 0;
}
