#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

using namespace std;

const int WIDTH = 40;
const int HEIGHT = 20;

// Function to set terminal to non-blocking mode
void setNonBlockingInput() {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag &= ~(ICANON | ECHO);
    ttystate.c_cc[VMIN] = 0;
    ttystate.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

// Function to restore terminal to normal mode
void restoreInput() {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

// Check if key is pressed
int kbhit() {
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

class Bullet {
public:
    int x, y;
    bool active;
    
    Bullet(int startX, int startY) : x(startX), y(startY), active(true) {}
    
    void move() {
        y--;
        if (y < 0) active = false;
    }
};

class Enemy {
public:
    int x, y;
    bool active;
    
    Enemy(int startX, int startY) : x(startX), y(startY), active(true) {}
    
    void move() {
        y++;
        if (y >= HEIGHT) active = false;
    }
};

class Game {
private:
    int playerX;
    vector<Bullet> bullets;
    vector<Enemy> enemies;
    int score;
    int lives;
    bool gameOver;
    int frameCount;
    int enemySpawnRate;
    int enemyMoveInterval;
    
public:
    Game() {
        playerX = WIDTH / 2;
        score = 0;
        lives = 3;
        gameOver = false;
        frameCount = 0;
        enemySpawnRate = 45;  // Spawn enemy every 45 frames initially
        enemyMoveInterval = 2; // Enemies move every N frames (lower = faster)
    }
    
    void handleInput() {
        while (kbhit()) {
            char ch = getchar();
            
            if (ch == 'a' || ch == 'A') {
                if (playerX > 0) playerX--;
            }
            else if (ch == 'd' || ch == 'D') {
                if (playerX < WIDTH - 1) playerX++;
            }
            else if (ch == ' ') {
                shoot();
            }
            else if (ch == 'q' || ch == 'Q') {
                gameOver = true;
            }
        }
    }
    
    void shoot() {
        bullets.push_back(Bullet(playerX, HEIGHT - 2));
    }
    
    void spawnEnemy() {
        int x = rand() % WIDTH;
        enemies.push_back(Enemy(x, 0));
    }
    
    void update() {
        frameCount++;
        
        // Spawn enemies
        if (frameCount % enemySpawnRate == 0) {
            spawnEnemy();
            // Increase difficulty over time
            if (enemySpawnRate > 20) {
                enemySpawnRate--;
            }
        }
        
        // Move bullets
        for (auto& bullet : bullets) {
            if (bullet.active) {
                bullet.move();
            }
        }
        
        // Move enemies (slower than bullets for better playability)
        bool moveEnemiesThisFrame = (frameCount % enemyMoveInterval == 0);
        for (auto& enemy : enemies) {
            if (enemy.active) {
                if (moveEnemiesThisFrame) {
                    enemy.move();
                }
                
                // Check if enemy reached player's row
                if (enemy.y >= HEIGHT - 1) {
                    if (enemy.x == playerX) {
                        lives--;
                        if (lives <= 0) {
                            gameOver = true;
                        }
                    }
                    enemy.active = false;
                }
            }
        }
        
        // Check collisions
        for (auto& bullet : bullets) {
            if (!bullet.active) continue;
            
            for (auto& enemy : enemies) {
                if (!enemy.active) continue;
                
                if (bullet.x == enemy.x && bullet.y == enemy.y) {
                    bullet.active = false;
                    enemy.active = false;
                    score += 10;
                    break;
                }
            }
        }
        
        // Remove inactive bullets and enemies
        bullets.erase(
            remove_if(bullets.begin(), bullets.end(), 
                     [](const Bullet& b) { return !b.active; }),
            bullets.end()
        );
        
        enemies.erase(
            remove_if(enemies.begin(), enemies.end(), 
                     [](const Enemy& e) { return !e.active; }),
            enemies.end()
        );
    }
    
    void draw() {
        // Clear screen
        cout << "\033[2J\033[H";
        
        // Top border
        for (int i = 0; i < WIDTH + 2; i++) cout << "═";
        cout << endl;
        
        // Game area
        for (int y = 0; y < HEIGHT; y++) {
            cout << "║";
            for (int x = 0; x < WIDTH; x++) {
                bool drawn = false;
                
                // Draw player
                if (y == HEIGHT - 1 && x == playerX) {
                    cout << "▲";
                    drawn = true;
                }
                
                // Draw bullets
                if (!drawn) {
                    for (const auto& bullet : bullets) {
                        if (bullet.active && bullet.x == x && bullet.y == y) {
                            cout << "│";
                            drawn = true;
                            break;
                        }
                    }
                }
                
                // Draw enemies
                if (!drawn) {
                    for (const auto& enemy : enemies) {
                        if (enemy.active && enemy.x == x && enemy.y == y) {
                            cout << "█";
                            drawn = true;
                            break;
                        }
                    }
                }
                
                if (!drawn) cout << " ";
            }
            cout << "║" << endl;
        }
        
        // Bottom border
        for (int i = 0; i < WIDTH + 2; i++) cout << "═";
        cout << endl;
        
        // Status
        cout << "Lives: ";
        for (int i = 0; i < lives; i++) cout << "♥ ";
        cout << " | Score: " << score;
        cout << " | Enemies: " << enemies.size();
        cout << endl;
        
        cout << "Controls: A/D = Move | SPACE = Shoot | Q = Quit" << endl;
    }
    
    bool isGameOver() {
        return gameOver;
    }
    
    int getScore() {
        return score;
    }
    
    void run() {
        setNonBlockingInput();
        
        while (!isGameOver()) {
            handleInput();
            update();
            draw();
            
            // Control game speed (about 33 FPS for smoother scrolling)
            usleep(30000);  // 30ms delay
        }
        
        restoreInput();
    }
};

int main() {
    srand(time(0));
    
    cout << "╔════════════════════════════════════╗" << endl;
    cout << "║      SPACE SHOOTER EXTREME!        ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    cout << "\nDefend against endless waves!" << endl;
    cout << "\nControls:" << endl;
    cout << "  A/D - Move left/right" << endl;
    cout << "  SPACE - Shoot" << endl;
    cout << "  Q - Quit" << endl;
    cout << "\nPress Enter to start...";
    cin.get();
    
    Game game;
    game.run();
    
    // Game over screen
    cout << "\033[2J\033[H";
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║          GAME OVER!                ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    cout << "\nFinal Score: " << game.getScore() << endl;
    cout << "\nThanks for playing!" << endl;
    
    return 0;
}
