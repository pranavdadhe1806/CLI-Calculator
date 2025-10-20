# ⚙️ CLI Calculator (C++)
A modular, command-line-based calculator built in **C++**, developed version-by-version to simulate real-world software engineering practices.  
This project focuses on clean code architecture, scalability, and incremental feature releases — evolving from basic arithmetic to advanced expression parsing.

---

## 🚀 Current Status — *Pre-Release (v0.9)*

✅ **CLI interface complete:**  
- Dynamic input (unlimited operands)  
- Menu-driven design  
- Continuous operation until exit  
- Project structure ready for modular expansion  

🧩 **Next Step (v1.0):**  
Implement core arithmetic logic — addition, subtraction, multiplication, division (in `calculator.cpp`).

---

## 🧱 Project Structure


```
CLI-Calculator/
├── src/                  # Source code (.cpp)
│   ├── main.cpp          # CLI logic and program loop
│   ├── calculator.cpp    # Core arithmetic logic
│   ├── history.cpp       # Operation history (future)
│   ├── utils.cpp         # Helper utilities
│   └── input_parser.cpp  # Expression parsing (future)
│
├── include/
│   └── calculator/       # Header files (.h)
│       ├── calculator.h
│       ├── history.h
│       └── utils.h
│
├── scripts/              # Build & automation scripts
│   ├── build.sh
│   ├── run.sh
│   └── format.sh
│
├── tests/                # Unit tests (future)
│   ├── test_calculator.cpp
│   ├── test_utils.cpp
│   └── test_history.cpp
│
├── Makefile              # Build automation
├── .gitignore            # Git ignore file
└── README.md             # Project documentation
```

**Legend:**
- Folders end with `/`
- Future features/modules are marked as such

---

## 🧮 Planned Version Roadmap

| Version | Status | Description |
|----------|---------|-------------|
| **v0.9** | ✅ Current | CLI input & structure ready |
| **v1.0** | 🔜 Next | Core arithmetic (add, sub, mul, div) |
| **v1.1** | 🧠 Planned | Input validation & UX improvements |
| **v2.0** | 🚀 Planned | Powers, roots, and percentage operations |
| **v3.0** | 🧩 Future | Trigonometric & logarithmic functions |
| **v4.0** | ⚙️ Future | Expression parsing & history persistence |
| **v5.0** | 🌐 Final | Optional GUI & advanced features |

---

## 🛠️ Build and Run Instructions

### 🔧 Prerequisites
- C++17 (or newer) compiler — e.g., `g++`, `clang++`, or MSVC
- `make` utility (optional, for using the Makefile)
- Git (for version control)

### 🏗️ Build using Makefile
```bash
make

▶️ Run
make run

🧰 Manual build (without Makefile)
g++ -std=c++17 -Iinclude src/*.cpp -o calculator
./calculator
```

---

## 🧩 Development Philosophy

This project is developed incrementally, version by version —
each new release adds new modules or improvements while maintaining backward compatibility.

**Core principles:**
v1.0 → first stable release
v1.1 → small improvements
v2.0 → new feature category (e.g., scientific functions)
git checkout -b feature-name
git commit -m "Add new feature"
git push origin feature-name
— Project Philosophy

---

## 🧠 Technologies Used

| Category         | Tool/Technology                |
|------------------|-------------------------------|
| Language         | C++17                         |
| Build System     | GNU Make                      |
| Version Control  | Git + GitHub                  |
| Testing (Future) | Catch2 / GoogleTest (planned) |
| Formatting       | clang-format (scripts/format.sh) |

---

## 🧩 Example Future Features

Once fully developed, the calculator will include:
- Power and root calculations
- Percentage functions
- Trigonometric and logarithmic operations
- Expression parsing (e.g., `12 + 3 * (4 - 1)`)
- History saving and file persistence
- Optional GUI (Qt / SFML)

---

## 🏷️ Versioning

This project follows semantic versioning:

**MAJOR.MINOR.PATCH**

| Part   | Meaning                                 |
|--------|-----------------------------------------|
| MAJOR  | Large feature set or design overhaul    |
| MINOR  | New features without breaking structure |
| PATCH  | Bug fixes or small improvements         |

**Examples:**
- `v1.0` → first stable release
- `v1.1` → small improvements
- `v2.0` → new feature category (e.g., scientific functions)

---

## 🧑‍💻 Author

**Pranav Dadhe**  
[GitHub Profile](https://github.com/pranavdadhe1806)

---

## 🧾 License

This project is open-source under the MIT License.  
You’re free to use, modify, and distribute it with attribution.

---

## ⭐ How to Contribute (Later)

Once the core features are ready:
1. Fork the repository
2. Create a feature branch:
	```bash
	git checkout -b feature-name
	```
3. Commit and push changes:
	```bash
	git commit -m "Add new feature"
	git push origin feature-name
	```
4. Open a Pull Request on GitHub

---

## 🏁 Quick Summary

| Stage                  | Progress                  |
|------------------------|--------------------------|
| ✅ CLI Built           | Completed (`main.cpp`)    |
| 🧮 Arithmetic Engine   | Next step (`calculator.cpp`)|
| 🧠 Validation + UI     | Planned                   |
| 🧰 Advanced Features   | Future                    |
| 🚀 GitHub Integration  | Completed                 |

---

> “Every line of code adds clarity — not complexity.”  
> — Project Philosophy

---

**Version:** v0.9 (Pre-release)  
**Maintained by:** Pranav Dadhe

---

## 🧭 Road to Version 1.0

**Core Arithmetic Coming Next!**