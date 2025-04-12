#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_SPEED 5
#define ASTEROID_SPEED 3
#define MAX_ASTEROIDS 5
#define STAR_COUNT 100
#define LIFE_MESSAGE_DURATION 2.0f // 2 seconds

typedef struct {
    Rectangle rect;
    Vector2 speed;
    bool active;
} Asteroid;

typedef struct {
    Vector2 position;
} Star;

// Function to reset the game state
void ResetGame(Rectangle *player, Asteroid asteroids[], int *score, int *lives, float *speedMultiplier, int *scoreSinceLastLifeUp, float *lifeMessageTimer) {
    *score = 0;
    *lives = 3;
    *speedMultiplier = 1.0f;
    *scoreSinceLastLifeUp = 0;
    *lifeMessageTimer = 0.0f;
    player->x = SCREEN_WIDTH / 2 - 25;

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        asteroids[i].rect.y = -50;
        asteroids[i].rect.x = rand() % SCREEN_WIDTH;
    }
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Spaceship vs Asteroid");
    SetTargetFPS(60);

    Texture2D playerTexture = LoadTexture("spaceship.png");
    Texture2D asteroidTexture = LoadTexture("asteroid.png");

    Rectangle player = { SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 60, 50, 50 };
    Asteroid asteroids[MAX_ASTEROIDS];
    Star stars[STAR_COUNT];

    int score = 0;
    int lives = 3;
    float speedMultiplier = 1.0f;
    int scoreSinceLastLifeUp = 0;
    float lifeMessageTimer = 0.0f;

    srand(time(NULL));
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        asteroids[i].rect = (Rectangle){ rand() % SCREEN_WIDTH, -50, 40, 40 };
        asteroids[i].speed = (Vector2){ 0, ASTEROID_SPEED };
        asteroids[i].active = true;
    }

    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i].position = (Vector2){ rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT };
    }

    bool gameOver = false;
    bool gameStarted = false;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        if (!gameStarted) {
            if (IsKeyPressed(KEY_ENTER)) {
                gameStarted = true;
                gameOver = false;
                ResetGame(&player, asteroids, &score, &lives, &speedMultiplier, &scoreSinceLastLifeUp, &lifeMessageTimer);
            }
        } else if (gameOver) {
            if (IsKeyPressed(KEY_ENTER)) {
                gameOver = false;
                ResetGame(&player, asteroids, &score, &lives, &speedMultiplier, &scoreSinceLastLifeUp, &lifeMessageTimer);
            }
        } else {
            // Player movement with mouse
            int mouseX = GetMouseX();
            player.x = mouseX - player.width / 2;

            if (player.x < 0) player.x = 0;
            if (player.x > SCREEN_WIDTH - player.width) player.x = SCREEN_WIDTH - player.width;

            // Update life message timer
            if (lifeMessageTimer > 0.0f) {
                lifeMessageTimer -= deltaTime;
            }

            // Asteroids update
            for (int i = 0; i < MAX_ASTEROIDS; i++) {
                if (asteroids[i].active) {
                    asteroids[i].rect.y += asteroids[i].speed.y * speedMultiplier;

                    if (asteroids[i].rect.y > SCREEN_HEIGHT) {
                        asteroids[i].rect.y = -50;
                        asteroids[i].rect.x = rand() % SCREEN_WIDTH;

                        score += 10;
                        scoreSinceLastLifeUp += 10;

                        // Increase speed with score
                        speedMultiplier = 1.0f + (score / 200.0f);

                        // Life up every 200 points
                        if (scoreSinceLastLifeUp >= 200) {
                            lives++;
                            scoreSinceLastLifeUp = 0;
                            lifeMessageTimer = LIFE_MESSAGE_DURATION; // Trigger the message
                        }
                    }

                    if (CheckCollisionRecs(player, asteroids[i].rect)) {
                        lives--;
                        if (lives <= 0) {
                            gameOver = true;
                        } else {
                            asteroids[i].rect.y = -50;
                            asteroids[i].rect.x = rand() % SCREEN_WIDTH;
                        }
                    }
                }
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < STAR_COUNT; i++) {
            DrawPixel(stars[i].position.x, stars[i].position.y, WHITE);
        }

        DrawText("Viraj's Game", SCREEN_WIDTH / 2 - 100, 20, 30, LIGHTGRAY);

        if (!gameStarted) {
            DrawText("Press ENTER to Start", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 20, 30, YELLOW);
        } else if (!gameOver) {
            DrawText(TextFormat("Score: %d", score), 20, 20, 20, WHITE);
            DrawText(TextFormat("Lives: %d", lives), 20, 50, 20, RED);

            // Draw life gained message
            if (lifeMessageTimer > 0.0f) {
                DrawText("+1 Life!", SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 - 100, 40, GREEN);
            }

            DrawTexture(playerTexture, player.x, player.y, WHITE);

            for (int i = 0; i < MAX_ASTEROIDS; i++) {
                if (asteroids[i].active) {
                    DrawTexture(asteroidTexture, asteroids[i].rect.x, asteroids[i].rect.y, WHITE);
                }
            }
        } else {
            DrawText("GAME OVER", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 40, 40, RED);
            DrawText(TextFormat("Final Score: %d", score), SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 + 20, 30, WHITE);
            DrawText("Press ENTER to Restart", SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 + 60, 30, GREEN);
        }

        EndDrawing();
    }

    UnloadTexture(playerTexture);
    UnloadTexture(asteroidTexture);
    CloseWindow();

    return 0;
}
