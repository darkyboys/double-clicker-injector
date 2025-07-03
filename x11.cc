#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <cerrno>

void emit_event(int fd, uint16_t type, uint16_t code, int32_t value) {
    struct input_event ie{};
    ie.type = type;
    ie.code = code;
    ie.value = value;

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
    int cps = 10;

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

    std::cout << "Running double-click injector.\n";
    std::cout << "Listening on: " << device << "\n";

    // Open uinput
    int uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinput_fd < 0) {
        perror("Failed to open /dev/uinput");
        return 1;
    }

    if (ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY) < 0 ||
        ioctl(uinput_fd, UI_SET_KEYBIT, BTN_LEFT) < 0 ||
        ioctl(uinput_fd, UI_SET_KEYBIT, BTN_RIGHT) < 0 ||
        ioctl(uinput_fd, UI_SET_EVBIT, EV_SYN) < 0) {
        perror("ioctl failed");
    close(uinput_fd);
    return 1;
        }

        struct uinput_user_dev uidev{};
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

        // Open real device
        int real_fd = open(device, O_RDONLY);
        if (real_fd < 0) {
            perror("Failed to open input device");
            ioctl(uinput_fd, UI_DEV_DESTROY);
            close(uinput_fd);
            return 1;
        }

        // Do not grab device to keep X11 working
        unsigned int max_interval_us = (1000000 / cps) * 1.2; // Add tolerance

        int click_count_left = 0;
        int click_count_right = 0;
        auto last_left_click = std::chrono::steady_clock::now();
        auto last_right_click = std::chrono::steady_clock::now();

        struct input_event ev{};
        while (true) {
            ssize_t n = read(real_fd, &ev, sizeof(ev));
            if (n != sizeof(ev)) continue;

            if (ev.type == EV_KEY && ev.value == 1) { // Only on button press
                auto now = std::chrono::steady_clock::now();

                if (ev.code == BTN_LEFT) {
                    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(now - last_left_click).count();
                    std::cout << "[Debug] LEFT click timing: " << diff << "us\n";

                    if (diff <= max_interval_us) {
                        click_count_left++;
                    } else {
                        click_count_left = 1;
                    }

                    last_left_click = now;

                    if (click_count_left == 2) {
                        std::cout << "Injected LEFT double click\n";
                        emit_click(uinput_fd, BTN_LEFT);
                        usleep(max_interval_us / 2);
                        emit_click(uinput_fd, BTN_LEFT);
                        click_count_left = 0;
                    }

                } else if (ev.code == BTN_RIGHT) {
                    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(now - last_right_click).count();
                    std::cout << "[Debug] RIGHT click timing: " << diff << "us\n";

                    if (diff <= max_interval_us) {
                        click_count_right++;
                    } else {
                        click_count_right = 1;
                    }

                    last_right_click = now;

                    if (click_count_right == 2) {
                        std::cout << "Injected RIGHT double click\n";
                        emit_click(uinput_fd, BTN_RIGHT);
                        usleep(max_interval_us / 2);
                        emit_click(uinput_fd, BTN_RIGHT);
                        click_count_right = 0;
                    }
                }
            }
        }

        // Cleanup (won’t reach here)
        ioctl(uinput_fd, UI_DEV_DESTROY);
        close(uinput_fd);
        close(real_fd);
        return 0;
}
