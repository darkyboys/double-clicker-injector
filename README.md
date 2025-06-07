# 🖱️ Double Click Injector - Linux (X11) CPS Booster

> Give your budget mouse superpowers.  
> Designed for competitive PvP players using **Arch Linux** or any Linux with **`uinput` support**.

---

## ⚡ What is this?

A simple C++ tool that listens to your real mouse **left-clicks** and injects **additional virtual clicks** using the Linux `uinput` interface.  
You get **higher CPS (clicks per second)** — perfect for Minecraft PvP and bridging, especially on Bedrock Edition.

No double-click mouse? No problem.

---

## 🎮 Features

- ✅ **Custom CPS** rate (default: 10)
- ✅ **Dynamic timing adjustment** based on CPS
- ✅ Uses safe `uinput` interface — no kernel patches or hacks
- ✅ Real-time injection only when you click
- ✅ CLI arguments for easy control
- ✅ MIT licensed — use it your way

---

## 📦 Installation

```bash
sudo pacman -Syu gcc linux-headers
````

Then clone and compile:

```bash
git clone https://github.com/YOUR_USERNAME/double-click-injector.git
cd double-click-injector
g++ main.cc -o double_click_injector -std=c++14 -lpthread
```

> 🧠 Note: Replace `main.cc` with your filename if different.

---

## 🚀 Usage

Find your mouse input device:

```bash
sudo evtest
```

Then run the tool:

```bash
sudo ./double_click_injector -d /dev/input/event3 -c 15
```

* `-d` = Device path (required)
* `-c` = CPS rate (optional, default is 10)

> Try between **12–20 CPS** for realistic results. Going too high may get flagged as cheating.

---

## ⚠️ Disclaimer

This tool is for **educational purposes** and fair PvP enhancement.
**Don't abuse it** — using this on competitive servers might break the rules.

---

## 🧑‍💻 Author

**GHGLTGGAMER**
Made for fellow gamers who don’t want to spend ₹4000 on a “butterfly-clicking” mouse.

---

## 📄 License

[MIT](LICENSE)

> Written in 2025. Happy PvPing! 🏹🔥
