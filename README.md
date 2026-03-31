# Spaceship Editor & Breakout

A 2D Arcade Machine project built in C++ and OpenGL. This project consists of two main phases: a pre-game spaceship builder editor and a fully functional implementation of the classic Breakout game.

## 🚀 Key Features

### 1. Spaceship Editor Phase
Before the game starts, players use an interactive editor to build their paddle (spaceship).

* **Drag & Drop Interface:** Players can intuitively build their ship by dragging blocks onto a grid.
* **Easy Modification:** A simple right-click removes blocks from the grid.
* **Constraint Validation:** The game only starts (the Start button turns green) if the built ship meets specific structural constraints, such as block limits and connectivity.

### 2. Breakout Gameplay Phase
Once the ship is built, it is imported into the game as the player's paddle.

* **Classic Mechanics:** The core objective is to eliminate all the bricks on the screen without losing the ball.
* **Life System:** The player starts the game with 3 lives. A life is lost if the ball falls below the paddle.
* **Dynamic Physics:** The ball's bounce angle dynamically changes based on where it makes contact with the horizontal edge of the paddle.
* **Resistant Bricks:** Bricks require multiple hits to be destroyed, with different colors indicating their remaining health.
* **Visual Effects:** When a brick breaks, it features a smooth animation where it shrinks until it completely disappears.

## 🎮 Controls

### Editor Controls
* **Left Mouse Button (Hold):** Drag and drop blocks onto the grid.
* **Right Mouse Button:** Remove a block from the grid.
* **Left Mouse Click:** Press the Start button to begin the game.

### Game Controls
* **Left / Right Arrows (← / →):** Move the paddle horizontally.
* **SPACE:** Launch the ball from the paddle at a 45° angle.

## 💻 Technologies Used
* **Language:** C++
* **Graphics API:** OpenGL
* **Framework:** Custom University 2D Graphics Framework (handling basic windowing, rendering, and input)
* **Mathematics:** GLM (OpenGL Mathematics) for vector calculations, transformations, and AABB collision detection

## ⚙️ Collision & Physics Details
The collision system handles interactions between the circular ball and the rectangular blocks (AABB).

* **Ball-Wall:** Reverses the corresponding velocity component to simulate a perfect bounce.
* **Ball-Brick:** Reverses velocity upon breaking the brick and updates the game score.
* **Ball-Paddle:** Uses custom logic to adjust the reflection angle, giving the player strategic control over the ball's trajectory.
