#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define i2c_Address 0x3c

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//input buttons
const int buttonUpPin = 5;
const int buttonDownPin = 4;
const int buttonLeftPin = 3;
const int buttonRightPin = 2;

//map dimensions
const int mapWidth = 8;
const int mapHeight = 8;

//player initial position
int playerX = 4;
int playerY = 5;

char gameMap[mapHeight][mapWidth] = {
  {'_', '_', '_', '_', '_', '_', '_', '_'},
  {'_', '_', '_', '_', '_', '_', '_', '_'},
  {'_', '_', '_', '_', '_', '_', '_', '_'},
  {'_', '_', '_', '_', '_', '_', '_', '_'},
  {'_', '_', '_', '_', '_', '_', '_', '_'},
  {'_', '_', '_', '_', '_', '_', '_', '_'},
  {'_', '_', '_', '_', '_', '_', '_', '_'},
  {'_', '_', '_', '_', '_', '_', '_', '_'}
};

//bool isChest = false;
//bool chestHasWeapon = false;

struct Enemy {
  int x, y;
  char symbol;
  int health;
  int damage;
  bool alive;
};

Enemy enemies[4];
Enemy boss[1];

bool gameOver = false;
bool gameStarted = true;
bool gameWon = false;
int difficulty = 1;
int levelProgress = 1;
int stage = 1;

bool playerTurn = true;
int hpPotCount = 0;
int playerHP = 100;
int playerDamage = 5;
String playerWeapon = "Stk";

void setup() {
  //Initialize
  delay(250);
  randomSeed(analogRead(0));
  display.begin(i2c_Address, true);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  pinMode(buttonUpPin, INPUT_PULLUP);
  pinMode(buttonDownPin, INPUT_PULLUP);
  pinMode(buttonLeftPin, INPUT_PULLUP);
  pinMode(buttonRightPin, INPUT_PULLUP);

  //draw the initial map
  drawMap();
  boss[0] = {0,0,'X', 0, 0, false};
  for (int i = 0; i < 4; i++) {
    enemies[i] = {0,0,'X', 0, 0, false};
  }
}

void loop() {
  if (gameWon) {
    //gameWon, wanna try again screen
    display.clearDisplay();
    display.setCursor(10, 20);
    display.print("You Win!");
    display.display();
  }
  else if (!gameStarted) {
    //game start screen
  }
  else if (gameOver) {
    //game over try again screen
    display.clearDisplay();
    display.setCursor(10, 20);
    display.print("Game Over!");
    display.display();
  }
  else {
    if (playerTurn) {
      playersMove();
    }
    else {
      enemiesMove();
      playerTurn = true;
    }
    drawMap();
  }
}

void drawMap() {
  display.clearDisplay();
  for (int y = 0; y < mapHeight; y++) {
    for (int x = 0; x < mapWidth; x++) {
      display.setCursor(x * 12, y * 8);
      display.print(gameMap[y][x]);
    }
  }
  int statX = mapWidth * 12 + 2;
  int statY = 0;

  display.setCursor(statX, statY);
  display.print(playerHP);
  statY += 8;

  display.setCursor(statX, statY);
  display.print(hpPotCount);
  statY += 8;

  display.setCursor(statX, statY);
  display.print(playerDamage);
  statY += 8;

  display.setCursor(statX, statY);
  display.print(playerWeapon);
  statY += 8;

  display.setCursor(statX, statY);
  display.print(difficulty);
  statY += 8;

  display.setCursor(statX, statY);
  display.print(levelProgress);
  statY += 8;

  display.setCursor(statX, statY);
  display.print(stage);
  display.display();
}

void playersMove() {
  gameMap[playerY][playerX] = '_';
  //player makes a move
  if (digitalRead(buttonUpPin) == LOW) {
    if (playerY != 0) {
      if (gameMap[playerY-1][playerX] == '_') {
        move(0, -1);
      }
      else {
        checkObstacle(0, -1);
      }
      playerTurn = false;
    }
    else {
      playerY = mapHeight-1;
      generateNewMap();
    }
    delay(50);
  }
  if (digitalRead(buttonDownPin) == LOW) {
    if (playerY != mapHeight-1) {
      if (gameMap[playerY+1][playerX] == '_') {
        move(0, 1);
      }
      else {
        checkObstacle(0, 1);
      }
      playerTurn = false;
    }
    else {
      playerY = 0;
      generateNewMap();
    }
    delay(50);
  }
  if (digitalRead(buttonLeftPin) == LOW) {
    if (playerX != 0) {
      if (gameMap[playerY][playerX-1] == '_') {
        move(-1, 0);
      }
      else {
        checkObstacle(-1, 0);
      }
      playerTurn = false;
    }
    else {
      playerX = mapWidth-1;
      generateNewMap();
    }
    delay(50);
  }
  if (digitalRead(buttonRightPin) == LOW) {
    if (playerX != mapWidth-1) {
      if (gameMap[playerY][playerX+1] == '_') {
        move(1, 0);
      }
      else {
        checkObstacle(1, 0);
      }
      playerTurn = false;
    }
    else {
      playerX = 0;
      generateNewMap();
    }
    delay(50);
  }
  gameMap[playerY][playerX] = 'P';
}

void move(int x, int y) {
  playerX += x;
  playerY += y;
}

void checkObstacle(int x, int y) {
  char thatTile = gameMap[playerY+y][playerX+x];
  if (thatTile == 'B' || thatTile == 'T') {
    //you wasted a turn.
  }
  if (thatTile == 'C') {
    playerHP -= 10;
    if (playerHP <= 0) {
      playerDeath();
    }
  }
  /* NOT IMPLEMENTED :(
  if (thatTile == 'H') {
    //player opens chest
  }
  */
  if (thatTile == 'L') {
    playerDeath();
  }
  if (thatTile == 'W') {
    takeWeapon(playerX+x, playerY+y);
    move(x, y);
  }
  else {
    hitThatEnemy(playerX+x, playerY+y);
  }
}

void hitThatEnemy(int x, int y) {
  if (boss[0].alive) {
    boss[0].health -= playerDamage;
    if (boss[0].health <= 0) {
      if (boss[0].symbol != 'D') {
        hpPotCount++;
        difficulty = 1;
        levelProgress = -1;
        stage++;
        gameMap[boss[0].y][boss[0].x] = '_';
      }
      else {
        gameWon = true;
      }
      boss[0].alive = false;
    }
  }
  else {
    for (int i = 0; i < 4; i++) {
      if (enemies[i].alive && enemies[i].x == x && enemies[i].y == y) {
        enemies[i].health -= playerDamage;
        if (enemies[i].health <= 0) {
          enemies[i].alive = false;
          gameMap[enemies[i].y][enemies[i].x] = '_';
          //drop
          if (stage == 1) {
            int probability = (difficulty * 10) + ((levelProgress / 2) * 5);
            int randomChance1 = random(0, 100);
            int randomChance2 = random(0, 100);
            if (randomChance1 < probability) {
              hpPotCount++;
            }
            if (randomChance2 < probability) {
              spawnWeapon(x, y);
            }
          }
          else if (stage == 2) {
            int probability = (difficulty * 15) + ((levelProgress / 2) * 5);
            int randomChance1 = random(0, 100);
            int randomChance2 = random(0, 100);
            if (randomChance1 < probability) {
              hpPotCount++;
            }
            if (randomChance2 < probability) {
              spawnWeapon(x, y);
            }
          }
          else {
            int probability = (difficulty * 20) + ((levelProgress / 2) * 5);
            int randomChance1 = random(0, 100);
            int randomChance2 = random(0, 100);
            if (randomChance1 < probability) {
              hpPotCount++;
            }
            if (randomChance2 < probability) {
              spawnWeapon(x, y);
            }
          }
        }
      }
    }
  }

}

void spawnWeapon(int x, int y) {
  gameMap[y][x] = 'W';
}

void takeWeapon(int x, int y) {
  if (stage == 1) {
      playerWeapon = "Swd";
      playerDamage = 8;
  }
  if (stage == 2) {
    if (levelProgress == 1) {
      playerWeapon = "PSk";
      playerDamage = 10;
    }
    else {
      playerWeapon = "Yyo";
      playerDamage = 13;
    }
  }
  else {
    if (levelProgress == 1) {
      playerWeapon = "SSk";
      playerDamage = 15;
    }
    else {
      playerWeapon = "MSd";
      playerDamage = 20;
    }
  }
  gameMap[y][x] = '_';
}

void playerDeath() {
  if (hpPotCount > 0) {
    playerHP = 100;
    hpPotCount--;
    //A death denied, for now.
  }
  else {
    gameOver = true;
  }
}

void generateNewMap() {
  clearCurrentMap();
  killCurrentEnemies();
  if (levelProgress < 21) {
    levelProgress++;
  }
  difficulty = (levelProgress / 7) + 1;
  if (difficulty > 3) {
    difficulty = 3;
  }
  int remainder = levelProgress%7;
  if (levelProgress != 0) {
    if (remainder == 0) {
      generateBoss();
    }
    else {
      int enemyCount = difficulty + (random(0, 100) < (levelProgress * 5) % 100 ? 1 : 0);
      int obstacleCount = random(0, 7);
      generateEnemy(enemyCount);
      generateObstacle(obstacleCount);
    }
  }
  else {
    gameMap[4][4] = 'W';
  }

}

void generateBoss() {
  if (stage == 1) {
    boss[0] = {4, 4, 'O', 35, 10, true};
  }
  else if (stage == 2) {
    boss[0] = {4, 4, 'S', 50, 20, true};
  }
  else if (stage == 3) {
    boss[0] = {4, 4, 'D', 100, 25, true};
  }
}

void clearCurrentMap() {
  for (int i = 0; i < mapHeight; i++) {
    for (int j = 0; j < mapWidth; j++) {
      gameMap[i][j] = '_';
    }
  }
}

void killCurrentEnemies() {
  for (int i = 0; i < 4; i++) {
    enemies[i].alive = false;
  }
  boss[0].alive = false;
}

void generateEnemy(int enemyCount) {
  for (int i = 0; i < enemyCount; i++) {
    int x, y;
    do {
      x = random(1, 7);
      y = random(1, 7);
    } while (gameMap[y][x] != '_');

    int enemyNum = random(0,2);
    if (stage == 1) {
      if (enemyNum == 0) {
        enemies[i] = {x, y, 'E', 5, 10, true};
        gameMap[y][x] = 'E';
      }
      else {
        enemies[i] = {x, y, 'G', 10, 5, true}; 
        gameMap[y][x] = 'G';
      }
    }
    else if (stage == 2) {
      if (enemyNum == 0) {
        enemies[i] = {x, y, 'A', 10, 15, true}; 
        gameMap[y][x] = 'A';
      }
      else {
        enemies[i] = {x, y, 'M', 15, 10, true}; 
        gameMap[y][x] = 'M';
      }
    }
    else {
      if (enemyNum == 0) {
        enemies[i] = {x, y, 'F', 20, 20, true}; 
        gameMap[y][x] = 'F';
      }
      else {
        enemies[i] = {x, y, 'I', 15, 15, true}; 
        gameMap[y][x] = 'I';
      }
    }
  }
}

void generateObstacle(int obstacleCount) {
  for (int i = 0; i < obstacleCount; i++) {
    int x, y;
    do {
      x = random(1, 7);
      y = random(1, 7);
    } while (gameMap[y][x] != '_');

    int obsNum = random(0,2);
    if (stage == 1) {
      if (obsNum == 0) {
        gameMap[y][x] = 'B';
      }
      else {
        gameMap[y][x] = 'T';
      }
    }
    else if (stage == 2) {
      if (obsNum == 0) {
        gameMap[y][x] = 'B';
      }
      else {
        gameMap[y][x] = 'C';
      }
    }
    else {
      if (obsNum == 0) {
        gameMap[y][x] = 'B';
      }
      else {
        gameMap[y][x] = 'L';
      }
    }

  }
}

void enemiesMove() {
  if (boss[0].alive) {
    gameMap[boss[0].y][boss[0].x] = '_';
    int dx = playerX - boss[0].x;
    int dy = playerY - boss[0].y;
    if ((abs(dx) == 1 && dy == 0) || (abs(dy) == 1 && dx == 0)) {
      playerHP -= boss[0].damage;
      if (playerHP <= 0) {
        playerDeath();
      }
    }
    else {
      if (abs(dx) > abs(dy)) {
        if (dx > 0) {
          boss[0].x++;
        } else {
          boss[0].x--;
        }
      } else {
        if (dy > 0) {
          boss[0].y++;
        } else {
          boss[0].y--;
        }
      }
    }
    gameMap[boss[0].y][boss[0].x] = boss[0].symbol;
  }
  else {
    //standard enemies
    for (int i = 0; i < 4; i++) {
      if (enemies[i].alive) {
        gameMap[enemies[i].y][enemies[i].x] = '_';
        int dx = playerX - enemies[i].x;
        int dy = playerY - enemies[i].y;
        if ((abs(dx) == 1 && dy == 0) || (abs(dy) == 1 && dx == 0)) {
          playerHP -=  enemies[i].damage;
          if (playerHP <= 0) {
            playerDeath();
          }
        }
        else {
          if (abs(dx) > abs(dy)) {
            if (dx > 0) {
              if (gameMap[enemies[i].y][enemies[i].x+1] == '_') {
                enemies[i].x++;
              }
              
            } else {
              enemies[i].x--;
            }
          } else {
            if (dy > 0) {
              enemies[i].y++;
            } else {
              enemies[i].y--;
            }
          }
        }
        gameMap[enemies[i].y][enemies[i].x] = enemies[i].symbol;
      }
    }
  }
}