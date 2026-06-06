# Tic-Tac-Toe (Kółko i Krzyżyk) - ESP32 & LCD I2C

An interactive Tic-Tac-Toe game implemented on the **ESP32 microcontroller**, utilizing a **4x4 Matrix Keypad** for player input and a **20x4 LCD Display (I2C)** for the visual board representation.

## 📺 Project Demonstration
See the project in action on YouTube:
👉 [Watch the Video Demonstration](https://youtube.com/shorts/P2gXw24iPWE)

---

## 🛠️ Hardware Requirements
* **Microcontroller:** ESP32 (e.g., ESP32 NodeMCU)
* **Display:** LCD 20x4 with an I2C interface adapter (PCF8574)
* **Input:** 4x4 Membrane Matrix Keypad
* **Connecting Wires & Breadboard**

### Pin Mapping

#### ⌨️ 4x4 Keypad Connection
| Keypad Pin | ESP32 GPIO |
|------------|------------|
| Row 1      | GPIO 13    |
| Row 2      | GPIO 14    |
| Row 3      | GPIO 27    |
| Row 4      | GPIO 26    |
| Column 1   | GPIO 25    |
| Column 2   | GPIO 33    |
| Column 3   | GPIO 32    |
| Column 4   | GPIO 17    |

#### 📺 LCD 20x4 I2C Connection
| LCD I2C Pin | ESP32 GPIO |
|-------------|------------|
| VCC         | 5V         |
| GND         | GND        |
| SDA         | GPIO 21    |
| SCL         | GPIO 22    |

---

## 🕹️ Game Rules & Controls
1. **Starting the Game:** The game boots up automatically with player **'X'** making the first move.
2. **Making a Move:** Use numeric keys **1 to 9** on the keypad to select a position on the 3x3 grid:
1 | 2 | 3
4 | 5 | 6
7 | 8 | 9
3. **Resetting the Game:** Press the **`*`** key at any moment to clear the board, reset scores, and start a new match.
4. **Winning/Draw:** The system continuously evaluates the board state. If a player completes a row, column, or diagonal, the winner is declared immediately on the screen, and further moves are locked until a reset. If all spaces are filled with no winner, a **DRAW!** is announced.

---

## 💻 Software & Libraries Used
This project is written in C++ using the Arduino core framework. It relies on the following libraries:
* `Wire.h` (Built-in I2C communication)
* `LiquidCrystal_I2C` by Frank de Brabander (LCD management)
* `Keypad` by Mark Stanley, Alexander Brevig (Matrix keypad scanning)

---

## 👥 Author
* **GitHub:** [@Freshovsky23](https://github.com/Freshovsky23)
* **Email:** Heilman.Wiktor23@gmail.com