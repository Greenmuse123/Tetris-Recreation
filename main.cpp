#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath> // For rotation
#include <algorithm>

int score = 0;
sf::Font font;

// Define the game board size
const int WIDTH = 10;
const int HEIGHT = 20;
std::vector<std::vector<int>> board(HEIGHT, std::vector<int>(WIDTH, 0));

// Define Tetrominoes shapes
std::vector<std::vector<std::vector<int>>> tetrominoes = {
    // I shape
    {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // T shape
    {
        {0, 1, 0},
        {1, 1, 1},
        {0, 0, 0}
    },
    // O shape
    {
        {1, 1},
        {1, 1}
    },
    // S shape
    {
        {0, 1, 1},
        {1, 1, 0},
        {0, 0, 0}
    },
    // Z shape
    {
        {1, 1, 0},
        {0, 1, 1},
        {0, 0, 0}
    },
    // L shape
    {
        {0, 0, 1},
        {1, 1, 1},
        {0, 0, 0}
    },
    // J shape
    {
        {1, 0, 0},
        {1, 1, 1},
        {0, 0, 0}
    }
};



bool checkCollision(const std::vector<std::vector<int>>& tetromino, int newRow, int newCol) {
    for (int y = 0; y < tetromino.size(); y++) {
        for (int x = 0; x < tetromino[y].size(); x++) {
            // Skip empty cells
            if (tetromino[y][x] == 0) continue;

            // Calculate the coordinates on the board for the current cell
            int boardX = newCol + x;
            int boardY = newRow + y;

            // Check boundaries
            if (boardX < 0 || boardX >= WIDTH || boardY < 0 || boardY >= HEIGHT) {
                return true; // Collision detected with walls
            }

            // Check for collision with existing blocks on the board
            if (board[boardY][boardX] != 0) {
                return true; // Collision detected with existing blocks
            }
        }
    }

    return false; // No collision detected
}
void drawScore(sf::RenderWindow& window) {
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setCharacterSize(24); // Example size
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(20, HEIGHT * 20); // Example position, adjust as needed

    window.draw(scoreText);
}


// Current Tetromino position and shape
std::vector<std::vector<int>> currentTetromino;
int tetrominoRow = 0, tetrominoCol = WIDTH / 2;
void spawnNewTetromino() {
    currentTetromino = tetrominoes[rand() % tetrominoes.size()];
    tetrominoRow = 0;
    tetrominoCol = WIDTH / 2 - 1; // Adjust based on the Tetromino width if necessary
    // Check if the new Tetromino can be placed, if not, game over
    if (checkCollision(currentTetromino, tetrominoRow, tetrominoCol)) {
        std::cout << "Game Over!" << std::endl;
        // Here you can end the game or reset it
    }
}


void initializeGame() {
    // Initialize the random seed
    srand(static_cast<unsigned int>(time(nullptr)));

    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
    }



    // Initialize game board
    board = std::vector<std::vector<int>>(HEIGHT, std::vector<int>(WIDTH, 0));

    // Spawn the first Tetromino
    spawnNewTetromino();
}



void rotateTetromino() {
    if (currentTetromino == tetrominoes[2]) return; // Skip rotation for O shape

    std::vector<std::vector<int>> rotatedTetromino;
    int rows = currentTetromino.size();
    int cols = currentTetromino[0].size();

    // Initialize the rotated Tetromino with flipped dimensions
    rotatedTetromino.resize(cols, std::vector<int>(rows, 0));

    // Perform the rotation (transpose and reverse rows)
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            rotatedTetromino[x][rows - y - 1] = currentTetromino[y][x];
        }
    }

    // Check for collisions with the rotated Tetromino without changing its position
    if (!checkCollision(rotatedTetromino, tetrominoRow, tetrominoCol)) {
        // If no collision, update the current Tetromino to its rotated state
        currentTetromino = rotatedTetromino;
    }
    else {
        // Optional: Handle collision (e.g., attempt wall kicks or simply do not rotate)
    }
}


void clearLines() {
    int linesCleared = 0;
    for (int row = 0; row < HEIGHT; ++row) {
        bool lineIsFull = true;
        for (int col = 0; col < WIDTH; ++col) {
            if (board[row][col] == 0) {
                lineIsFull = false; // Found an empty cell, so the line is not full
                break;
            }
        }

        if (lineIsFull) {
            linesCleared++; // Increment for each line cleared
            // Move all rows above this one down
            for (int moveRow = row; moveRow > 0; --moveRow) {
                for (int col = 0; col < WIDTH; ++col) {
                    board[moveRow][col] = board[moveRow - 1][col];
                }
            }

            // Clear the top row since everything moved down
            for (int col = 0; col < WIDTH; ++col) {
                board[0][col] = 0;
            }
        }
    }

    // Update the score based on lines cleared
    score += linesCleared * 100; // Example scoring system
}



void drawBoard(sf::RenderWindow& window) {
    // Set up sizes
    int tileSize = 20; // Size of a tile
    sf::RectangleShape tile(sf::Vector2f(tileSize - 2, tileSize - 2)); // Slightly smaller to create a grid effect

    // Draw the board
    for (int row = 0; row < HEIGHT; ++row) {
        for (int col = 0; col < WIDTH; ++col) {
            tile.setPosition(col * tileSize, row * tileSize);
            tile.setFillColor(board[row][col] ? sf::Color::Red : sf::Color::White);
            window.draw(tile);
        }
    }

    // Draw the current Tetromino
    for (size_t row = 0; row < currentTetromino.size(); ++row) {
        for (size_t col = 0; col < currentTetromino[row].size(); ++col) {
            if (currentTetromino[row][col]) {
                tile.setPosition((tetrominoCol + col) * tileSize, (tetrominoRow + row) * tileSize);
                tile.setFillColor(sf::Color::Green);
                window.draw(tile);
            }
        }
    }
}

void moveTetromino(int dx, int dy) {
    int newCol = tetrominoCol + dx;
    int newRow = tetrominoRow + dy;

    if (!checkCollision(currentTetromino, newRow, newCol)) {
        tetrominoRow = newRow;
        tetrominoCol = newCol;
    }
}




void gameLoop() {
    sf::RenderWindow window(sf::VideoMode(WIDTH * 20, HEIGHT * 20), "Tetris Game");
    sf::Clock clock;
    float timer = 0, delay = 0.5; // Delay controls the Tetromino fall speed

    while (window.isOpen()) {
        float time = clock.restart().asSeconds();
        timer += time;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up) rotateTetromino();
                if (event.key.code == sf::Keyboard::Left) moveTetromino(-1, 0);
                if (event.key.code == sf::Keyboard::Right) moveTetromino(1, 0);
                if (event.key.code == sf::Keyboard::Down) moveTetromino(0, 1);
            }
        }

        // Move Tetromino down automatically
        if (timer > delay) {
            if (!checkCollision(currentTetromino, tetrominoRow + 1, tetrominoCol)) {
                tetrominoRow++;
            }
            else {
                // Integrate Tetromino into the board
                for (size_t row = 0; row < currentTetromino.size(); ++row) {
                    for (size_t col = 0; col < currentTetromino[row].size(); ++col) {
                        if (currentTetromino[row][col]) {
                            board[tetrominoRow + row][tetrominoCol + col] = 1; // Mark the cell as filled
                        }
                    }
                }

                // Check for and clear any completed lines
                clearLines();

                // Spawn a new Tetromino
                spawnNewTetromino();
            }
            timer = 0;
        }

        window.clear(sf::Color::Black);
        drawBoard(window);
        drawScore(window);
        window.display();
    }
}



int main() {
    initializeGame();
    gameLoop();
    return 0;
}
