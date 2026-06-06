#include <Arduino.h>           // Include the core Arduino framework libraries
#include <Wire.h>              // Include the I2C communication library for LCD control
#include <LiquidCrystal_I2C.h> // Include the library for I2C Liquid Crystal Displays
#include <Keypad.h>            // Include the matrix keypad interface library

// --- 4x4 MEMBRANE KEYPAD CONFIGURATION ---
const byte ROWS = 4;           // Define the number of rows on the keypad matrix
const byte COLS = 4;           // Define the number of columns on the keypad matrix
char keys[ROWS][COLS] = {      // Define the key map layout corresponding to the physical matrix
  {'1','2','3','A'},           // Row 0 key definitions (1-3 for grid selection, A is unused)
  {'4','5','6','B'},           // Row 1 key definitions (4-6 for grid selection, B is unused)
  {'7','8','9','C'},           // Row 2 key definitions (7-9 for grid selection, C is unused)
  {'*','0','#','D'}            // Row 3 key definitions (* acts as Reset, others are unused)
};                             // End of keys multi-dimensional array definition

byte rowPins[ROWS] = {13, 14, 27, 26}; // ESP32 GPIO pins connected to the keypad row lines
byte colPins[COLS] = {25, 33, 32, 17}; // ESP32 GPIO pins connected to the keypad column lines

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // Initialize the Keypad driver instance
LiquidCrystal_I2C lcd(0x27, 20, 4);      // Initialize the LCD instance (I2C address 0x27, 20 columns, 4 rows)

// --- GAME STATE (Global Variables) ---
char tab[9] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}; // Global array representing the 3x3 board (initially empty spaces)
char aktualnyGracz = 'X';                                    // Track the currently active player, 'X' always starts
bool graAktywna = true;                                      // State flag to block player moves once a game is over

// Function to check the current board state and determine the status of the game
char sprawdzstan(char plansza[9]) { // Receives the board array as an argument
    const int kombinacje[8][3] = {  // Multi-dimensional array storing all 8 possible winning line indexes
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // Horizontal row winning combinations
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // Vertical column winning combinations
        {0, 4, 8}, {2, 4, 6}             // Diagonal winning combinations
    };                              // End of winning combinations matrix

    // 1. Check for a winner
    for (int i = 0; i < 8; i++) {   // Iterate through all 8 possible winning combinations
        if (plansza[kombinacje[i][0]] == plansza[kombinacje[i][1]] && // Check if first index matches the second index
            plansza[kombinacje[i][1]] == plansza[kombinacje[i][2]] && // Check if second index matches the third index
            plansza[kombinacje[i][0]] != ' ') {                       // Ensure the matched cells are not empty spaces
            return plansza[kombinacje[i][0]]; // Return the winning player's character ('X' or 'O')
        }                           // End of conditional statement for matching line
    }                               // End of win-checking loop

    // 2. Check if the game is still ongoing
    for (int i = 0; i < 9; i++) {   // Iterate through all 9 board positions
        if (plansza[i] == ' ') return ' '; // If any empty cell remains, return space to continue the game
    }                               // End of loop looking for empty spaces

    return 'R'; // If no winning line is found and no empty spaces remain, return 'R' for Draw (Remis)
}               // End of sprawdzstan function

// Function to reset the game state variables and redraw the LCD layout
void resetujGre() {                 // Defines the reset function
    // Restore initial values of game state variables
    for (int i = 0; i < 9; i++) {   // Loop through all elements of the game board array
        tab[i] = ' ';               // Clear each position by setting it back to an empty space
    }                               // End of array clearing loop
    aktualnyGracz = 'X';            // Set the current starting player back to 'X'
    graAktywna = true;              // Set the game active flag back to true to unlock inputs

    // Clear and redraw the visual board template on the LCD
    lcd.clear();                    // Clear all text currently displayed on the LCD screen
    lcd.setCursor(0, 0); lcd.print("Tic Tac Toe play:  X"); // Set cursor to top-left and print game title with starting player
    lcd.setCursor(0, 1); lcd.print("        _|_|_");         // Move to line 1 and print top grid line representation
    lcd.setCursor(0, 2); lcd.print("        _|_|_");         // Move to line 2 and print middle grid line representation
    lcd.setCursor(0, 3); lcd.print("*=Reset  | | ");         // Move to line 3, print control hint and bottom grid lines
}                                   // End of resetujGre function

void setup() {                      // Standard Arduino initialization function
  Serial.begin(115200);             // Open serial communication port at 115200 baud rate for debugging
  
  lcd.init();                       // Initialize the I2C LCD hardware module
  lcd.backlight();                  // Turn on the LCD screen backlight LED
  
  // First boot screen initialization
  resetujGre();                     // Execute the reset function to initialize board data and render layout
}                                   // End of setup function

void gra() {                        // Core game loop execution logic function
    char key = keypad.getKey();     // Poll the matrix keypad to capture any currently pressed key

    // RESET REACTION: If * is pressed, reset the game instantly at any given time
    if (key == '*') {               // Conditional check for the reset key command
        resetujGre();               // Invoke the game and display reset function
        return;                     // Terminate current gra() execution to skip further logic in this main loop iteration
    }                               // End of reset condition handling

    // Process inputs only if a numeric key '1'-'9' is pressed and the game is active
    if (key >= '1' && key <= '9' && graAktywna) { // Evaluates key bounds and active game state flag
        int indeks = key - '1';     // Calculate 0-8 array index by subtracting character code '1' from pressed key

        // Check if the selected board position is empty
        if (tab[indeks] == ' ') {   // Enter block only if chosen grid position contains an empty space
            tab[indeks] = aktualnyGracz; // Store the active player's move inside the ESP32 board array memory

            // Mathematical translation of 1D index (0-8) to 2D LCD coordinates (rows/columns)
            int wierszLcd = 1 + (indeks / 3);       // Calculate the matching LCD target row coordinate (rows 1-3)
            int kolumnaLcd = 8 + 2 * (indeks % 3);  // Calculate the matching LCD target column coordinate
            
            lcd.setCursor(kolumnaLcd, wierszLcd);   // Position the LCD hardware cursor to computed coordinates
            lcd.print(aktualnyGracz);               // Render the active player's character ('X' or 'O') on the LCD board

            // GAME STATE ANALYSIS SWITCH
            switch (sprawdzstan(tab)) { // Pass the current board layout to evaluate the match status
                case 'X':           // Block triggered if player 'X' satisfies win requirements
                    lcd.setCursor(0, 0); lcd.print("WON PLAYER X!       "); // Print victory notification for player X on line 0
                    graAktywna = false;             // Flip the active flag to false to lock further keypad inputs
                    break;          // Exit the switch block execution

                case 'O':           // Block triggered if player 'O' satisfies win requirements
                    lcd.setCursor(0, 0); lcd.print("WON PLAYER O!       "); // Print victory notification for player O on line 0
                    graAktywna = false;             // Flip the active flag to false to lock further keypad inputs
                    break;          // Exit the switch block execution

                case 'R':           // Block triggered if board is full and no winning matches exist (Draw)
                    lcd.setCursor(0, 0); lcd.print("DRAW!               "); // Print tie game notice on line 0 overriding player info
                    graAktywna = false;             // Flip the active flag to false to lock further keypad inputs
                    break;          // Exit the switch block execution

                default:            // Triggered if sprawdzstan returns ' ' (meaning game continues)
                    // Alternate active player flag value using a ternary conditional operator
                    aktualnyGracz = (aktualnyGracz == 'X') ? 'O' : 'X'; // Switch to 'O' if current was 'X', else switch to 'X'
                    lcd.setCursor(19, 0); lcd.print(aktualnyGracz);     // Refresh the active player display status character on LCD
                    break;          // Exit the switch block execution
            }                       // End of game state switch structure
        }                           // End of empty cell validation condition block
    }                               // End of key evaluation condition block
}                                   // End of main gra function block

void loop() {                       // Standard infinite loop structure of the Arduino environment
    gra();                          // Repeatedly call the game logic module function indefinitely
}                                   // End of loop function block