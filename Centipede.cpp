#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

// Initializing Dimensions.
// resolutionX and resolutionY determine the rendering resolution.
// Don't edit unless required. Use functions on lines 43, 44, 45 for resizing the game window.
const int resolutionX = 1024;
const int resolutionY = 780;

// 1024 780
// 1182 900

const int resYAdjusted = resolutionY-80; // Used to improve readability

const int boxPixelsX = 32;
const int boxPixelsY = 32;

const int gameRows = resYAdjusted / boxPixelsY; // Total rows on grid
const int gameColumns = resolutionX / boxPixelsX; // Total columns on grid

// Initializing GameGrid.
int gameGrid[gameRows][gameColumns] = {};

// The following exist purely for readability.
const int x = 0;
const int y = 1;
const int exists = 2;
const int velocity = 3;
const int mushroomsHit = 4;

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Write your functions declarations here. Some have been written for you. //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

// Drawing functions, draw their respective sprites
void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite);
void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite);
void drawMushroom(RenderWindow& window, Sprite &mushroomSprite);
void drawCentipede(RenderWindow& window, int ** centipedeptr, int length, Sprite centipedeSprite);
void drawScore(RenderWindow& window, int score, Text &scoreText);
void drawLives(RenderWindow& window, int lives, Sprite playerSprite);
void drawFlea(RenderWindow& window, int flea[], Sprite fleaSprite);
void drawScorpion(RenderWindow& window, int scorpion[], Sprite &scorpionSprite);

// Movement functions, time dependent hence use clocks
void moveBullet(float bullet[], sf::Clock& bulletClock);
void movePlayer(float player[], Clock& playerClock, bool &killflag);
void moveCentipede(int ** &centipede, int length, Clock &centipedeClock, double moveSpeed);
void moveFlea(int flea[], Clock &fleaClock);
void moveScorpion(int scorpion[], Clock &scorpionClock);

// Collision Detection
bool checkAABB(float a[4], float b[4]); // Takes 2 arrays, each with x position, y position, width and height to check for collision
void bulletCollision(float bullet[], int ** &centipede, int &length,int flea[], int scorpion[], int &score, int level); // Checks for collision between bullet, centipede, fleas, and scorpions
int playerCollision(float player[]); // Checks for collision between player and mushrooms
bool playerCentipedeCollision(float player[], int ** &centipede, int &length); // Checks for collision between player and centipede
bool playerFleaCollision(float player[], int flea[]); // Checks for collision between player and fleas

// Processing Functions
void shootBullet(float bullet[3], float player[3]); // Shoots bullets
void mushroomGen(int mushroomNumber); // Spawns mushrooms on game grid
void intToStr(int number, char array[]); // Converts integer to char array
bool compareCharArray(char arr1[], char arr2[]); // Compares two char arrays

// Initialiazation Functions
void centipedeInit(int ** &centipedeptr, int &length); // Initializes centipede
void gameInit(int level, int ** centipede, int centipedeLen, int ** &fastCP, int mushroomcount); // Initializes game grid
void headCentipedeSpawn(int ** &centipedeptr, int &length, Clock &spawnClock); // Spawns periodically appearing head centipedes
void leaderboardRead(char names[10][15], int levels[10], int lives[10], int score[10]); // Reads leaderboard from file
void leaderboardWrite(char name[15], int level, int lives, int score); // Writes new scores to file
void fastHeadCentipedeInit(int ** &centipede, int length); // Spawns detatched head at advanced levels
void fleaSpawn(int flea[]); // Spawns flea
void scorpionSpawn(int scorpion[]); // Spawns scorpion
void centipedeDelete(int ** &centipedeptr, int &length); // Deletes the centipede

// Segmentation
void centipedeSplit(int ** &x, int &length, int index, int level); // Splits the centipede at given index
int centipedeDouble(int ** &x, int &length, int index); // Doubles centipede segment

// Screens
int menu(RenderWindow &window, Font indieFont, Music &bgmusic); // Main menu screen
void changeBGM(RenderWindow &window, Font indieFont, Music &bgm); // Change Music Screen
void showLeaderboard(RenderWindow &window, Font indieFont); // Leaderboard screen
int pause(RenderWindow &window, Font indieFont, Music &bgm); // Pause menu
int gameOver(RenderWindow &window, Font indieFont, int score, int lives, int level);

int main()
{
    srand(time(0));

    // Declaring RenderWindow.
    sf::RenderWindow window(sf::VideoMode(resolutionX, resolutionY), "Centipede", sf::Style::Close | sf::Style::Titlebar);

    // Used to resize your window if it's too big or too small. Use according to your needs.
    // window.setSize(sf::Vector2u(640, 640)); // Recommended for 1366x768 (768p) displays.
    // window.setSize(sf::Vector2u(1280, 1280)); // Recommended for 2560x1440 (1440p) displays.
    // window.setSize(sf::Vector2u(1920, 1920)); // Recommended for 3840x2160 (4k) displays.
    window.setSize(Vector2u(1182, 900));

    // Used to position your window on every launch. Use according to your needs.
    window.setPosition(sf::Vector2i(100, 0));

    // Initializing Background Music.
    ifstream song;
    song.open("currentsong.txt");
    char csong[40];
    song >> csong;
    song.close();

    sf::Music bgMusic;
    bgMusic.openFromFile(csong);
    bgMusic.play();
    bgMusic.setVolume(50);

    // Initializing Background.
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    backgroundTexture.loadFromFile("Textures/backgroundtest.jpg");
    backgroundSprite.setTexture(backgroundTexture);
    //backgroundSprite.setColor(sf::Color(255, 255, 255, 255 * 0.20)); // Reduces Opacity to 25%

    // Initializing Player and Player Sprites.
    Clock playerClock;
    float player[2] = {};
    player[x] = (gameColumns / 2) * boxPixelsX;
    player[y] = resolutionY - boxPixelsY;
    sf::Texture playerTexture;
    sf::Sprite playerSprite;
    playerTexture.loadFromFile("Textures/player.png");
    playerSprite.setTexture(playerTexture);
    playerSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

    // Initializing Bullet and Bullet Sprites.
    float bullet[3] = {};
    bullet[x] = player[x];
    bullet[y] = player[y] - boxPixelsY;
    bullet[exists] = false;
    sf::Clock bulletClock;
    sf::Texture bulletTexture;
    sf::Sprite bulletSprite;
    bulletTexture.loadFromFile("Textures/bullet.png");
    bulletSprite.setTexture(bulletTexture);
    bulletSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
    bulletSprite.setOrigin(16, 2);

    // Initializing Centipede
    Clock centipedeClock;
    Sprite centipedeSprite;
    Texture centipedeTexture;
    centipedeTexture.loadFromFile("Textures/centipede.png");
    centipedeSprite.setTexture(centipedeTexture);
    centipedeSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));

    int centipedeLen = 12;
    int ** centipede = NULL;
    centipedeInit(centipede, centipedeLen);

    Clock spawnClock;

    // Initializing Flea
    Clock fleaClock;
    Sprite fleaSprite;
    Texture fleaTexture;
    fleaTexture.loadFromFile("Textures/flea.png");
    fleaSprite.setTexture(fleaTexture);
    fleaSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
    int flea[3];
    flea[x] = 0;
    flea[y] = 0;
    flea[exists] = false;

    // Initializing Scorpion
    Clock scorpionClock;
    Sprite scorpionSprite;
    Texture scorpionTexture;
    scorpionTexture.loadFromFile("Textures/scorpion.png");
    scorpionSprite.setTexture(scorpionTexture);
    scorpionSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));

    int scorpion[4];
    scorpion[x] = 0;
    scorpion[y] = 0;
    scorpion[exists] = false;
    scorpion[3] = 0;


    // Initializing Gameboard
    int level = 1;
    int score = 0;
    int lives = 6;
    double centipededelay = 200;

    int fastCPLen = (level-1);
    int ** fastHeadCentipede = NULL;
    fastHeadCentipedeInit(fastHeadCentipede, fastCPLen);
    Clock fastCPClock;

    // Initializing Mushrooms
    Sprite mushroomSprite;
    Texture mushroomTexture;
    mushroomTexture.loadFromFile("Textures/mushroom.png");
    mushroomSprite.setTexture(mushroomTexture);
    mushroomSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
    int mushroomnumber = rand() % 11 + 20;

    gameInit(level, centipede, centipedeLen, fastHeadCentipede, mushroomnumber);

    Clock collisionClock;

    // Text initialization
    Text scoreText;
    Font indieFont;
    indieFont.loadFromFile("Fonts/IndieFlower-Regular.ttf");
    scoreText.setFont(indieFont);
    scoreText.setFillColor(Color::Black);
    scoreText.setCharacterSize(60);
    scoreText.setOrigin(0, 70);

    bool killflag = false;

    menu(window, indieFont, bgMusic);
    while(window.isOpen()) {

        ///////////////////////////////////////////////////////////////
        //                                                           //
        // Call Your Functions Here. Some have been written for you. //
        // Be vary of the order you call them, SFML draws in order.  //
        //                                                           //
        ///////////////////////////////////////////////////////////////
        window.clear(Color::Transparent);
        window.draw(backgroundSprite);
        drawLives(window,lives, playerSprite);
        bulletCollision(bullet, centipede, centipedeLen, flea, scorpion, score, level);
        if (playerCentipedeCollision(player, centipede, centipedeLen) || playerCentipedeCollision(player, fastHeadCentipede, fastCPLen)){
            lives--;
            centipedeDelete(centipede, centipedeLen);
            centipedeDelete(fastHeadCentipede, fastCPLen);
            centipedeLen = 12 - (level-1);
            fastCPLen = (level-1);
            centipedeInit(centipede, centipedeLen);
            fastHeadCentipedeInit(fastHeadCentipede, fastCPLen);
            gameInit(level, centipede, centipedeLen, fastHeadCentipede, mushroomnumber);
        };
        headCentipedeSpawn(centipede, centipedeLen, spawnClock);
        drawPlayer(window, player, playerSprite);
        drawScore(window, score, scoreText);
        moveCentipede(centipede, centipedeLen,centipedeClock, centipededelay);
        drawCentipede(window, centipede, centipedeLen, centipedeSprite);

        if (flea[exists]){
            if (playerFleaCollision(player, flea)){
                lives--;
                centipedeDelete(centipede, centipedeLen);
                centipedeDelete(fastHeadCentipede, fastCPLen);
                centipedeLen = 12 - (level-1);
                fastCPLen = (level-1);
                centipedeInit(centipede, centipedeLen);
                fastHeadCentipedeInit(fastHeadCentipede, fastCPLen);
                gameInit(level, centipede, centipedeLen, fastHeadCentipede, mushroomnumber);
            }
            moveFlea(flea, fleaClock);
            drawFlea(window, flea, fleaSprite);
        } else {
            fleaSpawn(flea);
        }

        if (scorpion[exists]){
            moveScorpion(scorpion, scorpionClock);
            drawScorpion(window, scorpion, scorpionSprite);
        } else {
            if (scorpionClock.getElapsedTime().asSeconds() > 30){
                scorpionSpawn(scorpion);
            }
        }

        if ((level-1)){
            moveCentipede(fastHeadCentipede, fastCPLen, fastCPClock, (centipededelay/1.2));
            drawCentipede(window, fastHeadCentipede, fastCPLen, centipedeSprite);
            bulletCollision(bullet, fastHeadCentipede, fastCPLen, flea, scorpion, score, level);
        }

        drawMushroom(window,mushroomSprite);
        movePlayer(player, playerClock, killflag);

        if (bullet[exists] == true) {
            moveBullet(bullet, bulletClock);
            drawBullet(window, bullet, bulletSprite);
        }
        if (Keyboard::isKeyPressed(Keyboard::X) || Keyboard::isKeyPressed(Keyboard::Z)){
            shootBullet(bullet, player);
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape)){
            while (Keyboard::isKeyPressed(Keyboard::Escape));
            switch (pause(window, indieFont, bgMusic)){

            };
        }

        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                return 0;
            }
        }

        if (centipedeLen == 0){
            if (!(level-1)){
                if (fastCPLen == 0){
                    level++;
                    centipededelay /= 2;
                    centipedeLen = 11;
                    centipedeInit(centipede, centipedeLen);
                    gameInit(level, centipede, centipedeLen, fastHeadCentipede, mushroomnumber);
                    fastCPLen = (level-1);
                }
            } else {
                level++;
                centipedeLen = 12;

                centipedeInit(centipede, centipedeLen);

                gameInit(level, centipede, centipedeLen, fastHeadCentipede, mushroomnumber);
                fastCPLen = (level-1);
            }
        }

        if (killflag){
            killflag = false;
            lives--;
            centipedeDelete(centipede, centipedeLen);
            centipedeDelete(fastHeadCentipede, fastCPLen);
            centipedeLen = 12 - (level-1);
            fastCPLen = (level-1);
            centipedeInit(centipede, centipedeLen);
            fastHeadCentipedeInit(fastHeadCentipede, fastCPLen);
            gameInit(level, centipede, centipedeLen, fastHeadCentipede, mushroomnumber);
        }

        if (lives == 0){
            score = 0;
            lives = 3;
            level = 1;
            centipedeDelete(centipede, centipedeLen);
            centipedeDelete(fastHeadCentipede, fastCPLen);
            centipedeLen = 12 - (level-1);
            fastCPLen = (level-1);
            centipedeInit(centipede, centipedeLen);
            fastHeadCentipedeInit(fastHeadCentipede, fastCPLen);
            gameInit(level, centipede, centipedeLen, fastHeadCentipede, mushroomnumber);
        }


        window.display();
        window.clear();
    }
}

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// Write your functions definitions here. Some have been written for you. //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

/////////////////////////// Drawing Functions //////////////////////////////

void drawPlayer(sf::RenderWindow& window, float player[], sf::Sprite& playerSprite) {
    playerSprite.setPosition(player[x], player[y]);
    window.draw(playerSprite);
}

void drawBullet(sf::RenderWindow& window, float bullet[], sf::Sprite& bulletSprite) {
    bulletSprite.setPosition(bullet[x], bullet[y]);
    window.draw(bulletSprite);
}

void moveBullet(float bullet[], sf::Clock& bulletClock) {
    if (bulletClock.getElapsedTime().asMilliseconds() < 20)
        return;

    bulletClock.restart();
    bullet[y] -= 10;
    if (bullet[y] < 80)
        bullet[exists] = false;
}

void centipedeInit(int ** &centipedeptr, int &length){
    static int head = 2; // For readablity
    int xoffset = 47;

    // Checking to see if the pointer has been deleted or not

    if (centipedeptr != NULL){
        cout << "Warning, Centipede Pointer dangling, possible memory leak." << endl;
        centipedeptr = NULL;
    }

    // Creating a new 2D array with 12 rows
    centipedeptr = new int * [length];

    // Creating columns
    centipedeptr[0] = new int [5];
    for (int i = 1; i < length; i++){
        centipedeptr[i] = new int [4];
    }


     centipedeptr[0][x] = (rand() % gameColumns) * boxPixelsX;
     centipedeptr[0][y] = ((rand() % gameRows) * boxPixelsY) + 80;



    centipedeptr[0][head] = true;
    centipedeptr[0][velocity] = 2; // (11)_2, first bit indicates horizontal direction, 2nd vertical
    centipedeptr[0][mushroomsHit] = 0;

    for (int i = 1; i < length; i++){
        // Setting coordinates of centipede body off-screen
        centipedeptr[i][x] = -boxPixelsX;
        centipedeptr[i][y] = -boxPixelsY;
        centipedeptr[i][head] = false;
        centipedeptr[i][velocity] = 3;
    }
}

void shootBullet(float bullet[], float player[]){
    if (bullet[exists] == false){
        bullet[x] = player[x] + 16;
        bullet[y] = player[y];
        bullet[exists] = true;
    }
}

void drawCentipede(RenderWindow& window, int ** centipedeptr, int length, Sprite centipedeSprite){
    static int head = 2; // For readability
    for (int i = 0; i < length; i++){
        if (centipedeptr[i][head]){
            centipedeSprite.setTextureRect(IntRect(0,0,boxPixelsX,boxPixelsY));
        } else {
            centipedeSprite.setTextureRect(IntRect(boxPixelsX, 0, boxPixelsX, boxPixelsY));
        }

        centipedeSprite.setPosition(centipedeptr[i][x], centipedeptr[i][y]);
        window.draw(centipedeSprite);
    }
}

void moveCentipede(int ** &centipede, int length, Clock &centipedeClock, double movedelay){
    static int head = 2;
    if (centipedeClock.getElapsedTime().asMilliseconds() < movedelay){
        return ;
    }

    centipedeClock.restart();


    for (int i = length - 1; i >= 0; i--){
        if (centipede[i][head]){
            int projectedposition = centipede[i][x] + ((centipede[i][velocity]&1)?(32):(-32));
            int projectedrow = (centipede[i][y]-80)/boxPixelsY;
            if (projectedrow >= gameRows){
                projectedrow = gameRows - 1;
            }
            if (((centipede[i][velocity]&1) && (projectedposition >= resolutionX)) || ((projectedposition < 0) && !(centipede[i][velocity]&1))){
                centipede[i][y] += ((centipede[i][velocity]&2)?(32):(-32));
                centipede[i][velocity] ^= 1;
            } else if (gameGrid[projectedrow][projectedposition/boxPixelsX] != 0){
                centipede[i][y] += ((centipede[i][velocity]&2)?(32):(-32));
                centipede[i][velocity] ^= 1;
            } else {
                centipede[i][x] = projectedposition;
            }

            if ((centipede[i][y] + ((centipede[i][velocity]&2)?(32):(-32))) >= resolutionY){
                centipede[i][velocity]^=2;
            }
            if (!(centipede[i][velocity]&2)){
                if ((centipede[i][y] + ((centipede[i][velocity]&2)?(32):(-32))) <= (resolutionY- (5*boxPixelsY))){
                    centipede[i][velocity]^=2;
                }
            }
        } else {
            centipede[i][x] = centipede[i - 1][x];
            centipede[i][y] = centipede[i - 1][y];
            centipede[i][velocity] = centipede[i - 1][velocity];
        }
    }
}

void mushroomGen(int mushroomNumber){
    int mushroomrow = 0;
    int mushroomcol = 0;

    int currentmushrooms = 0;
    for (int i = 0; i < gameRows; i++){
        for (int j = 0; j < gameColumns; j++){
            if (gameGrid[i][j] != 0){
                currentmushrooms++;
            }
        }
    }

    for (int i = currentmushrooms; i < mushroomNumber; ){
        mushroomrow = rand() % gameRows;
        mushroomcol = rand() % gameColumns;
        if (gameGrid[mushroomrow][mushroomcol] == 0){
            gameGrid[mushroomrow][mushroomcol] = 2;
            i++;
        }
    }

}

void drawMushroom(RenderWindow& window, Sprite &mushroomSprite){
    for (int i = 0; i < gameRows; i++){
        for (int j = 0; j < gameColumns; j++){
            switch (gameGrid[i][j]){
                case 2:
                    mushroomSprite.setTextureRect(IntRect(0,0,boxPixelsX, boxPixelsY));
                    break;
                case 1:
                    mushroomSprite.setTextureRect(IntRect(boxPixelsX, 0, boxPixelsX, boxPixelsY));
                    break;
                case -1:
                    mushroomSprite.setTextureRect(IntRect(3*boxPixelsX, 0, boxPixelsX, boxPixelsY));
                    break;
                case -2:
                    mushroomSprite.setTextureRect(IntRect(2*boxPixelsX,0,boxPixelsX, boxPixelsY));
                    break;
            }
            if (gameGrid[i][j] != 0){
                mushroomSprite.setPosition(j*boxPixelsX, i*boxPixelsY + 80);
                window.draw(mushroomSprite);
            }
        }
    }
}

void gameInit(int level, int ** centipede, int centipedeLen, int ** &fastCP, int mushroomcount){
    double mushrooms = mushroomcount;

    if (level == 1){
        for (int i = 0; i < gameRows; i++){
            for (int j = 0; j < gameColumns; j++){
                gameGrid[i][j] = 0;
            }
        }
    } else {
        centipede[0][x] = resolutionX - 2 * boxPixelsX;
        centipede[0][y] = 80;
    }

    for (int i = 1; i < level; i++){
        mushrooms *= 1.2;
    }
    mushroomcount = mushrooms;
    mushroomGen(mushroomcount);
    if (level-1){
        fastHeadCentipedeInit(fastCP, (level-1));
    }

}

bool checkAABB(float a[], float b[]){
    static int xmin = 0, ymin = 1, xmax = 2, ymax = 3; // Improve readability

    if (a[xmin] < b[xmax] && a[xmax] > b[xmin] && a[ymin] < b[ymax] && a[ymax] > b[ymin]){
        return true;
    }
    return false;
}

void movePlayer(float player[], Clock& playerClock, bool &killflag){
    if (playerClock.getElapsedTime().asMilliseconds() < 20)
        return;

    playerClock.restart();
    float prevpos = 0;

    if ((Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)) && (player[y] >= (resolutionY - boxPixelsY*5))){
        prevpos = player[y];
        player[y] -= 10;
        if (playerCollision(player)){
            if (playerCollision(player) < 0){
                killflag = true;
            }
            player[y] = prevpos;
        }

    } else if ((Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down)) && (player[y] < resolutionY-boxPixelsY)){
        prevpos = player[y];
        player[y] += 10;
        if (playerCollision(player)){
            if (playerCollision(player) < 0){
                killflag = true;
            }
            player[y] = prevpos;
        }
    }

    if ((Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Right)) && player[x] >= 0){
        prevpos = player[x];
        player[x] -= 10;
        if (playerCollision(player)){
            if (playerCollision(player) < 0){
                killflag = true;
            }
            player[x] = prevpos;
        }
    } else if ((Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Left)) && player[x] <= resolutionX-boxPixelsX){
        prevpos = player[x];
        player[x] += 10;
        if (playerCollision(player)){
            if (playerCollision(player) < 0){
                killflag = true;
            }
            player[x] = prevpos;
        }
    }
}

void bulletCollision(float bullet[], int ** &centipede, int &length,int flea[], int scorpion[], int &score, int level){
    if (bullet[exists]){
        int column = bullet[x] / boxPixelsX;
        int k = 0, j = 0, row = 0;
        while (k * boxPixelsY <= (bullet[y] - 80)) {
            k = j++;
            row = ((j * boxPixelsY - bullet[y] + 80) > (bullet[y] - k * boxPixelsY - 80)) ? k : j;
        }
        if (gameGrid[row][column] == 2) {
            gameGrid[row][column] = 1;
            bullet[exists] = false;
            return;
        } else if (gameGrid[row][column] == 1) {
            gameGrid[row][column] = 0;
            score += 1;
            bullet[exists] = false;
            return;
        } else if (gameGrid[row][column] == -2){
            gameGrid[row][column] = -1;
            bullet[exists] = false;
            return;
        } else if (gameGrid[row][column] == -1) {
            gameGrid[row][column] = 0;
            score += 1;
            bullet[exists] = false;
            return;
        }
        for (int i = 0; i < length; i++){
            if ((centipede[i][x]/boxPixelsX == column)&&((centipede[i][y] - 80)/boxPixelsY == row)){
                score += (centipede[i][2])?(20):(10);
                gameGrid[row][column] = (row>16)?(-2):(2);
                centipedeSplit(centipede,length, i, level);
                bullet[exists] = false;
                return;
            }
        }

        if ((flea[x] == column*boxPixelsX)&&(flea[y]-80 == row*boxPixelsY)){
            flea[exists] = false;
            score += 200;
        }
        if ((scorpion[x] == column*boxPixelsX)&&(scorpion[y]-80 == row*boxPixelsY)){
            scorpion[exists] = false;
            score += 1000;
        }

    }
}

void centipedeSplit(int ** &centipede, int &length, int index, int level){
    static int head = 2;

        int prevnodeX = centipede[index][x];
        int prevnodeY = centipede[index][y];
        delete [] centipede[index];
        if ((index + 1 < length) && (!centipede[index + 1][head])){
            int * tempptr = new int [4];
            for (int i = 0; i < 4; i++){
                tempptr[i] = centipede[index + 1][i];
            }
            delete [] centipede[index + 1];
            centipede[index + 1] = new int [5];
            centipede[index + 1][x] = tempptr[x];
            centipede[index + 1][y] = tempptr[y];
            centipede[index + 1][head] = true;
            centipede[index + 1][velocity] = tempptr[velocity];
            centipede[index + 1][mushroomsHit] = 0;
            delete [] tempptr;
        }
        for (int i = index; i < length - 1; i++){
            centipede[i] = centipede[i+1];
        }
        length--;
}

int playerCollision(float player[]){
    float a[] = {player[x] + 7, player[y] + 1, player[x] + 27, player[y] + 32};
    float b[4];
    for (int i = 16; i < 21; i++){
        for (int j = 0; j < gameColumns; j++){
            if (gameGrid[i][j]){
                b[0] = j*boxPixelsX + 3;
                b[1] = i*boxPixelsY + 80;
                b[2] = b[0] + 26;
                b[3] = b[1] + 32;
                if (checkAABB(a,b)){
                    if (gameGrid[i][j] < 0){
                        return -1;
                    } else {
                        return 1;
                    }
                }
            }
        }
    }
    return false;
}

bool playerCentipedeCollision(float player[], int ** &centipede, int &length){
    float a[4] = {player[x] + 7, player[y] + 1, player[x] + 27, player[y] + 32};
    float b[4];
    for (int i = 0; i < length; i++){
        if (centipede[i][y] >= (resolutionY - 5*boxPixelsY)){
            b[0] = centipede[i][x] + 2;
            b[1] = centipede[i][y] + 2;
            b[2] = b[0] + 29;
            b[3] = b[1] + 28;
            if (checkAABB(a, b)){
                return true;
            }
        }
    }
    return false;
}

void headCentipedeSpawn(int ** &centipedeptr, int &length, Clock &spawnClock){
    if (spawnClock.getElapsedTime().asSeconds() < 6){
        return ;
    }
    spawnClock.restart();
    bool presenceflag = false;

    for (int i = 0; i < length; i++){
        if (centipedeptr[i][y] >= (resolutionY - 5*boxPixelsY)){
            presenceflag = true;
        }
    }

    if (presenceflag){
        int ** tempptr = new int * [length+1];
        for (int i = 0; i < length; i++){
            tempptr[i] = centipedeptr[i];
        }
        tempptr[length] = new int [5];
        tempptr[length][x] = 0;
        tempptr[length][y] = (19*boxPixelsY + 80);
        tempptr[length][2] = true;
        tempptr[length][velocity] = 2;
        tempptr[length][mushroomsHit] = 0;
        delete [] centipedeptr;
        centipedeptr = new int * [length + 1];
        for (int i = 0; i < length + 1; i++){
            centipedeptr[i] = tempptr[i];
        }
        delete [] tempptr;
        length++;
    }
}

void drawScore(RenderWindow& window, int score, Text &scoreText){
    char str[14] = "Score: ";
    int index = 7;
    int nums[6] = {};
    if (!score){
        str[index] = 48;
        index++;
    }
    for (int i = 0; i < 6; i++){
        nums[i] = -1;
    }
    for (int i = 0; score; i++){
        nums[i] = score%10;
        score/=10;
    }
    for (int i = 5; i >= 0; i--){
        if (nums[i] != -1){
            str[index] = char(nums[i] + 48);
            index++;
        }
    }
    scoreText.setString(str);
    scoreText.setPosition(12,80);
    window.draw(scoreText);
}

void drawLives(RenderWindow& window, int lives, Sprite playerSprite){
    int posx = 974, posy = 48;
    for (int i = 0; i < lives; i++){
        playerSprite.setPosition(posx, posy);
        if (i == 2){
            posy -= 32;
            posx = 974;
        } else {
            posx -= 32;
        }
        window.draw(playerSprite);
    }
}

int menu(RenderWindow &window, Font indieFont, Music &bgmusic){
    Texture backgroundTexture;
    backgroundTexture.loadFromFile("Textures/menubackground.png");
    Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    int selection = 1;

    Text menuItem;
    menuItem.setCharacterSize(48);
    menuItem.setFont(indieFont);

    while (window.isOpen()){
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                return 0;
            }
        }

        window.clear(Color::Black);

        window.draw(backgroundSprite);

        menuItem.setPosition(32, 462);
        menuItem.setString("Play");
        menuItem.setFillColor((selection == 1?Color::Blue:Color::Black));
        window.draw(menuItem);

        menuItem.setPosition(32, 535);
        menuItem.setString("Music");
        menuItem.setFillColor((selection == 2?Color::Blue:Color::Black));
        window.draw(menuItem);

        menuItem.setPosition(32, 608);
        menuItem.setString("Leaderboard");
        menuItem.setFillColor((selection == 3?Color::Blue:Color::Black));
        window.draw(menuItem);

        menuItem.setPosition(32, 681);
        menuItem.setString("Quit");
        menuItem.setFillColor((selection == 4?Color::Red:Color::Black));
        window.draw(menuItem);

        if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)){
            while (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W));
            selection--;
            if (selection < 1){
                selection = 4;
            }
        } else if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)){
            while (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S));
            selection++;
            if (selection > 4){
                selection = 1;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Enter)){
            while (Keyboard::isKeyPressed(Keyboard::Enter));
            switch (selection) {
                case 1:
                    return 1;
                    break;
                case 2:
                    changeBGM(window, indieFont, bgmusic);
                    break;
                case 3:
                    showLeaderboard(window, indieFont);
                    break;
                case 4:
                    window.close();
                    break;
            }
        }

        window.display();
    }

    return 1;
}

void showLeaderboard(RenderWindow &window, Font indieFont){
    char names[10][15];
    int levels[10];
    int lives[10];
    int score[10];
    char temp[7] = {'\0'};

    leaderboardRead(names, levels, lives, score);

    Texture background;
    background.loadFromFile("Textures/backgroundtest.jpg");
    Sprite backgroundSprite;
    backgroundSprite.setTexture(background);
    backgroundSprite.setTextureRect(IntRect(0, 0, resolutionX, resolutionY));

    Text drawText;
    drawText.setFont(indieFont);
    drawText.setCharacterSize(32);
    drawText.setFillColor(Color::Black);

    Text titleText;
    titleText.setFont(indieFont);
    titleText.setCharacterSize(48);
    titleText.setFillColor(Color::Black);
    titleText.setPosition(377, 31);

    int xpos = boxPixelsX;
    int ypos = 144;

    while (window.isOpen()){
        if (Keyboard::isKeyPressed(Keyboard::Escape)){
            while (Keyboard::isKeyPressed(Keyboard::Escape));
            return ;
        }

        xpos = boxPixelsX;
        ypos = 144;
        window.clear(Color::Black);
        window.draw(backgroundSprite);
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                return ;
            }
        }

        window.draw(titleText);

        drawText.setString("Name");
        drawText.setPosition(70, 80);
        window.draw(drawText);

        drawText.setString("Level");
        drawText.setPosition(400, 80);
        window.draw(drawText);

        drawText.setString("Lives");
        drawText.setPosition(538, 80);
        window.draw(drawText);

        drawText.setString("Score");
        drawText.setPosition(770, 80);
        window.draw(drawText);

        for (int i = 0; (i < 10) && levels[i]; i++){


            drawText.setPosition(xpos, ypos);
            drawText.setString(char(i + 49));
            window.draw(drawText);


            xpos += 38;

            drawText.setPosition(xpos, ypos);
            drawText.setString(names[i]);
            window.draw(drawText);


            xpos += 352;


            intToStr(levels[i], temp);
            drawText.setPosition(xpos, ypos);
            drawText.setString(temp);
            window.draw(drawText);

            xpos += 140;

            intToStr(lives[i], temp);
            drawText.setPosition(xpos, ypos);
            drawText.setString(temp);
            window.draw(drawText);

            xpos += 215;

            intToStr(score[i], temp);
            drawText.setPosition(xpos, ypos);
            drawText.setString(temp);
            window.draw(drawText);

            xpos = boxPixelsX;
            ypos += 64;


        }
        window.display();

    }

}

void leaderboardRead(char names[10][15], int levels[10], int lives[10], int scores[10]){
    ifstream leaderboard;
    leaderboard.open("scores.txt");

    if (!leaderboard) {
        cout << "Leaderboard file does not exist, initializing..." << endl;

        ofstream leaderboardInit;
        leaderboardInit.open("scores.txt");
        for (int i = 0; i < 10; i++) {
            leaderboardInit << "" << 0 << ' ' << 0 << ' ' << 0 << ' ' << '\n';
        }

    } else {
        int j = 0;
        for (j = 0; !leaderboard.eof(); j++){
            leaderboard >> names[j] >> levels[j] >> lives[j] >> scores[j];
        }
    }

}

void intToStr(int number, char array[]){
    int index = 0;
    int nums[7] = {};
    if (!number){
        array[index] = 48;
        index++;
    }
    for (int i = 0; i < 7; i++){
        nums[i] = -1;
    }
    for (int i = 0; number; i++){
        nums[i] = number%10;
        number/=10;
    }
    for (int i = 6; i >= 0; i--){
        if (nums[i] != -1){
            array[index] = char(nums[i] + 48);
            index++;
        }
    }
    array[index] = '\0';
}

void changeBGM(RenderWindow &window, Font indieFont, Music &bgm){
    ifstream song;
    song.open("currentsong.txt");
    char csong[40];
    song >> csong;
    song.close();


    char songNames[8][30] = {"I'm Fine", "A Matter Of Pride", "Dominance Is My Name", "Duel", "Field Of Hopes",
                            "Roar Of Counterattack", "School Days", "Strike Fear In Their Hearts"};
    char songPaths[8][40] = {"Music/im_fine.ogg", "Music/a_matter_of_pride.ogg", "Music/dominance_is_my_name.ogg", "Music/duel.ogg",
                             "Music/field_of_hopes.ogg", "Music/roar_of_counterattack.ogg", "Music/school_days.ogg", "Music/strike_fear_in_their_hearts.ogg"};
    Text drawText;
    drawText.setFont(indieFont);
    drawText.setFillColor(Color::Black);

    Texture background;
    background.loadFromFile("Textures/pausebg.png");
    Sprite backgroundSprite;
    backgroundSprite.setTexture(background);

    int xoffset = 281;
    int yoffset = 163;
    int selection = 0;
    int choice = 3;

    for (int i = 0; i < 8; i++){
        if (compareCharArray(csong, songPaths[i])){
            selection = i;
            break;
        }
    }

    backgroundSprite.setPosition(xoffset, yoffset);

    while (window.isOpen()){
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                return ;
            }
        }
        window.draw(backgroundSprite);
        drawText.setCharacterSize(48);
        drawText.setPosition(xoffset+96, yoffset+0);
        drawText.setString("Change Music");
        window.draw(drawText);

        drawText.setCharacterSize(40);
        drawText.setPosition(xoffset+48, yoffset+119);
        drawText.setString("Now Playing:");
        window.draw(drawText);

        drawText.setCharacterSize(32);
        drawText.setPosition(xoffset+48, yoffset+197);
        drawText.setString(songNames[selection]);
        window.draw(drawText);

        drawText.setCharacterSize(48);
        drawText.setPosition(xoffset+48, yoffset+275);
        drawText.setString("Next");
        drawText.setFillColor((choice == 1)?Color::Blue:Color::Black);
        window.draw(drawText);

        drawText.setPosition(xoffset+323, yoffset+275);
        drawText.setString("Prev");
        drawText.setFillColor((choice == 2)?Color::Blue:Color::Black);
        window.draw(drawText);

        drawText.setPosition(xoffset+179, yoffset+360);
        drawText.setString("Back");
        drawText.setFillColor((choice == 3)?Color::Blue:Color::Black);
        window.draw(drawText);


        drawText.setFillColor(Color::Black);

        if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)){
            while (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S));

            choice = (choice == 3)?(1):(3);
        } else if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)){
            while (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W));

            choice = (choice != 3)?(3):(1);
        } else if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)){
            while (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A));
            choice = (choice == 1)?(2):(1);
        } else if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)){
            while (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D));
            choice = (choice == 2)?(1):(2);
        }

        if (Keyboard::isKeyPressed(Keyboard::Enter)){
            while (Keyboard::isKeyPressed(Keyboard::Enter));
            switch (choice){
                case 1:
                    selection++;
                    selection = (selection > 7)?(0):(selection);
                    bgm.stop();
                    bgm.openFromFile(songPaths[selection]);
                    bgm.play();
                    bgm.setVolume(50);
                    break;
                case 2:
                    selection--;
                    selection = (selection < 0)?(7):(selection);
                    bgm.stop();
                    bgm.openFromFile(songPaths[selection]);
                    bgm.play();
                    bgm.setVolume(50);
                    break;
                case 3:
                    ofstream musicfile;
                    musicfile.open("currentsong.txt");
                    musicfile << songPaths[selection];
                    musicfile.close();
                    return ;

            }
        }


        window.display();

    }

}

bool compareCharArray(char arr1[], char arr2[]){
    for (int i = 0; arr1[i] && arr2[i]; i++){
        if (arr1[i] != arr2[i]){
            return false;
        }
    }
    return true;
}

void fastHeadCentipedeInit(int ** &centipedeptr, int length){
    static int head = 2; // For readablity

    if (!length){
        return;
    }

    int xoffset = 47;

    // Checking to see if the pointer has been deleted or not

    if (centipedeptr != NULL){
        centipedeptr = NULL;
    }

    // Creating a new 2D array with given rows
    centipedeptr = new int * [length];

    // Creating columns
    for (int i = 0; i < length; i++){
        centipedeptr[i] = new int [5];
        centipedeptr[i][x] = xoffset*boxPixelsX;
        centipedeptr[i][y] = 80;
        centipedeptr[i][head] = true;
        centipedeptr[i][velocity] = 3;
        centipedeptr[i][mushroomsHit] = 0;
        xoffset+=2;
    }
}

void fleaSpawn(int flea[]){
    int mushroomcount = 0;
    for (int i = 16; i < gameRows; i++){
        for (int j = 0; j < gameColumns; j++){
            if (gameGrid[i][j] != 0){
                mushroomcount++;
                if (mushroomcount > 3){
                    return ;
                }
            }
        }
    }

    if (mushroomcount == 3){
        flea[exists] = true;
        flea[x] = (rand() % gameColumns) * boxPixelsX;
        flea[y] = 80;
    }
}

void drawFlea(RenderWindow& window, int flea[], Sprite fleaSprite){
    fleaSprite.setPosition(flea[x], flea[y]);
    window.draw(fleaSprite);
}

void moveFlea(int flea[], Clock &fleaClock){
    if (fleaClock.getElapsedTime().asMilliseconds() < 100){
        return ;
    }

    fleaClock.restart();

    int fleaCol = flea[x] / boxPixelsX;
    int fleaRow = (flea[y] - 80) / boxPixelsY;

    if (gameGrid[fleaRow][fleaCol] == 0){
        gameGrid[fleaRow][fleaCol] = ((rand() % 100 + 1)>70)?(2):(0);
    }

    flea[y] += boxPixelsY;

    if (flea[y] > resolutionY){
        flea[exists] = false;
    }
}

bool playerFleaCollision(float player[], int flea[]){
    if (((flea[y]-80)/boxPixelsY) < 17){
        return false;
    }

    float a[4] = {player[x] + 7, player[y] + 1, player[x] + 27, player[y] + 32};
    float b[4];

    b[0] = flea[x] + 1;
    b[1] = flea[y] + 4;
    b[2] = b[1] + 30;
    b[3] = b[2] + 25;

    if (checkAABB(a, b)){
        return true;
    }
    return false;
}

void scorpionSpawn(int scorpion[]){
    scorpion[exists] = true;
    scorpion[x] = 0;
    scorpion[y] = (rand() % 17)*boxPixelsY + 80;
}

void moveScorpion(int scorpion[], Clock &scorpionClock){
    if (scorpionClock.getElapsedTime().asMilliseconds() < 250){
        return ;
    }

    scorpionClock.restart();

    int row = (scorpion[y] - 80)/boxPixelsY;
    int col = scorpion[x]/boxPixelsX;

    if (gameGrid[row][col] > 0){
        gameGrid[row][col] = -1 * gameGrid[row][col];
    }
    scorpion[3] ^= 1;

    scorpion[x] += boxPixelsX;

    if (scorpion[x] >= resolutionX){
        scorpion[exists] = false;
    }
}

void drawScorpion(RenderWindow& window, int scorpion[], Sprite &scorpionSprite){
    if (scorpion[3]){
        scorpionSprite.setTextureRect(IntRect(boxPixelsX, 0, -boxPixelsX, boxPixelsY));
    } else {
        scorpionSprite.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
    }
    scorpionSprite.setPosition(scorpion[x], scorpion[y]);
    window.draw(scorpionSprite);
}

int pause(RenderWindow &window, Font indieFont, Music &bgm){

    Text titleText;
    titleText.setFont(indieFont);
    titleText.setFillColor(Color::Black);
    titleText.setCharacterSize(48);
    titleText.setString("Paused");
    titleText.setPosition(431, 163);

    Text drawText;
    drawText.setCharacterSize(36);
    drawText.setFont(indieFont);
    drawText.setFillColor(Color::Black);

    Texture background;
    background.loadFromFile("Textures/pausebg.png");
    Sprite backgroundSprite;
    backgroundSprite.setTexture(background);
    backgroundSprite.setPosition(281, 163);

    int selection = 1;

    while (window.isOpen()){
        //window.clear(Color::Transparent);
        window.clear(sf::Color(0, 0, 0, 0));
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                return 0;
            }
        }

        window.draw(backgroundSprite);
        window.draw(titleText);

        drawText.setString("Restart");
        drawText.setFillColor((selection==1)?Color::Blue:Color::Black);
        drawText.setPosition(448, 259);
        window.draw(drawText);

        drawText.setString("Resume");
        drawText.setFillColor((selection==2)?Color::Blue:Color::Black);
        drawText.setPosition(440, 327);
        window.draw(drawText);

        drawText.setString("Change Music");
        drawText.setFillColor((selection==3)?Color::Blue:Color::Black);
        drawText.setPosition(411, 395);
        window.draw(drawText);

        drawText.setString("Leaderboard");
        drawText.setFillColor((selection==4)?Color::Blue:Color::Black);
        drawText.setPosition(413, 461);
        window.draw(drawText);

        drawText.setString("Quit");
        drawText.setFillColor((selection==5)?Color::Red:Color::Black);
        drawText.setPosition(476, 529);
        window.draw(drawText);

        if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)){
            while (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W));
            selection--;
            if (selection < 1){
                selection = 5;
            }
        } else if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)){
            while (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S));
            selection++;
            if (selection > 5){
                selection = 1;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Enter)){
            while (Keyboard::isKeyPressed(Keyboard::Enter));
            switch (selection) {
                case 1:
                    return 1;
                    break;
                case 2:
                    return 0;
                    break;
                case 3:
                    changeBGM(window, indieFont, bgm);
                    break;
                case 4:
                    showLeaderboard(window, indieFont);
                    break;
                case 5:
                    return 2;
                    break;
            }
        }
        window.display();

    }
}

void centipedeDelete(int ** &centipedeptr, int &length){
    if (length == 0){
        return ;
    }

    for (int i = 0; i < length; i++){
        delete [] centipedeptr[i];
    }
    delete [] centipedeptr;
    centipedeptr = NULL;
}

int gameOver(RenderWindow &window, Font indieFont, int score, int lives, int level){
    string name;
    char temp[9];
    int index = 0;

    char scoreText[14] = "Score: ";
    intToStr(score, temp);
    for (int i = 0, j = 7; temp[i] != '\0'; i++, j++){
        scoreText[j] = temp[i];
    }
    char livesText[14] = "Lives: ";
    intToStr(lives, temp);
    for (int i = 0, j = 7; temp[i] != '\0'; i++, j++){
        livesText[j] = temp[i];
    }
    char levelText[14] = "Level: ";
    intToStr(level, temp);
    for (int i = 0, j = 7; temp[i] != '\0'; i++, j++){
        levelText[j] = temp[i];
    }

    Text titleText;
    titleText.setFont(indieFont);
    titleText.setFillColor(Color::Black);
    titleText.setCharacterSize(48);
    titleText.setPosition(401, 163);
    titleText.setString("Game Over");

    Text entryText;
    entryText.setFont(indieFont);
    entryText.setFillColor(Color::Black);
    entryText.setCharacterSize(32);
    entryText.setString("Enter your first name: ");
    entryText.setPosition(313, 270);

    Text nameText;
    nameText.setFont(indieFont);
    nameText.setFillColor(Color::Black);
    nameText.setCharacterSize(40);
    nameText.setString(name);
    nameText.setPosition(313, 316);

    Text itemText;
    itemText.setFont(indieFont);
    itemText.setFillColor(Color::Black);
    itemText.setCharacterSize(32);

    while (window.isOpen()){

        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                return 0;
            } else if (e.type == Event::TextEntered){
                if (e.text.unicode != '\b'){
                    name += char(e.text.unicode);
                }
            }
        }

        window.draw(titleText);
        window.draw(entryText);
        nameText.setString(name);
        window.draw(nameText);

        itemText.setPosition(312, 430);
        itemText.setString(scoreText);
        window.draw(itemText);

        itemText.setPosition(312, 472);
        itemText.setString(livesText);
        window.draw(itemText);

        itemText.setPosition(312, 518);
        itemText.setString(levelText);
        window.draw(itemText);

        window.display();
    }
}
