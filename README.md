# 🖱️ Double Click Injector - Linux (X11) CPS Booster

> Give your budget mouse superpowers.  
> Designed for competitive PvP players using **Arch Linux** or any Linux with **`uinput` support**.

---

## ⚡ What is this?

A simple C++ tool that listens to your real mouse **left-clicks** and injects **additional virtual clicks** using the Linux `uinput` interface.  
You get **higher CPS (clicks per second)** — perfect for Minecraft PvP and bridging, especially on Bedrock Edition.

🧷 No double-click mouse? No problem.

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

> 🧠 Replace `main.cc` with your actual source filename if different.

---

## 🚀 Usage

First, find your mouse input device:

```bash
sudo evtest
```

Then run:

```bash
sudo ./double_click_injector -d /dev/input/event3 -c 15
```

* `-d` → Device path (required)
* `-c` → CPS rate (optional, default is 10)

> 🏁 **Best CPS range:** 12–20.
> Higher CPS is possible but may get flagged as cheating depending on server rules.

---

## ⚠️ Disclaimer

This tool is for **educational purposes** and fair PvP enhancement.

### 🧠 *In Simple Language*

> Don't be crazy 🗿. Don't go with **insane CPS** like 100.
> Yeah, it’s doable — but it’ll turn your CPU and memory into sausages 🌭,
> and you'll basically be telling servers:
>
> > “Hey, I’m the biggest hacker ever existed, now ban me.”
>
> So they *will* slap you 💀.
>
> Instead, just vibe, listen to a phonk beat, stick to **15–20 CPS**,
> play PvP, and that’s it.
>
> Your single-clicking \$5 mouse is now a butterfly-clicking **god**.
>
> No need for a ₹9999999999 mouse just for CPS 🤭.
>
> And also due to MIT License feel free to do anything , Like anything you want to the project as once you get the source you are the owner not me ;) so don't blame me or contrubutors that "Hey these guys made me crazy at cps now ban there mojang id too 🗿" We are not laiable for any changed as every open source project says.

---

## 🧑‍💻 Author

**GHGLTGGAMER**
Made for fellow gamers who don’t want to spend ₹4000 on a “butterfly-clicking” mouse.

---

## 📄 License

[MIT](LICENSE)

> Written in 2025. Happy PvPing! 🏹
