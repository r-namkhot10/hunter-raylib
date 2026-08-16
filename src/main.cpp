#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include "raylib.h"
#include "resource_dir.h"

struct Vec2 
{
    float x, y;

    Vec2(float x, float y) : x(x), y(y) {}

    Vec2 operator+(const Vec2& other) const {
        return { x + other.x, y + other.y };
    }

    Vec2 operator-(const Vec2& other) const {
        return { x - other.x, y - other.y };
    }

    Vec2 operator*(float k) const {
        return { x * k, y * k };
    }

    float length() const {
        return std::sqrt(x * x + y * y);
    }

    Vec2 normalize() const {
        float len = length();
        if (len == 0.0f)
            return { 0.0f, 0.0f };
        return { x / len, y / len };
    }

    float distanceTo(const Vec2& other) const {
        return (*this - other).length();
    }
};

class Hunter 
{
private:
    Vec2 position;
    float speed;

public:
    Hunter(Vec2 startPos, float startSpeed) : position(startPos), speed(startSpeed) {}

    void moveToward(const Vec2& target, const float dt) {
        Vec2 direction = (target - position).normalize();
        position = position + direction * speed * dt;
    }


    void increaseSpeed(float amount) {
        speed += amount;
        if (speed < 0.0f) {
            speed = 0.0f;
        }

        if (speed > 400.0f) {
            speed = 400.0f;
        }
    }

    Vec2 getPosition() const {
        return position;
    }

    float getSpeed() const {
        return speed;
    }

    bool hasCaught(const Vec2& target, const int playerSize) const {
        return position.distanceTo(target) < playerSize;
    }
};

int main() 
{
    std::vector<Hunter> hunters;
    hunters.push_back(Hunter(Vec2(1500.0f, 0.0f), 240.0f));
    hunters.push_back(Hunter(Vec2(500.0f, 1000.0f), 200.0f));

    const int SCREEN_W = 1500;
    const int SCREEN_H = 900;
    const int WORLD_W = 2000;
    const int WORLD_H = 2000;
    const int PLAYER_SIZE = 40;
    const int HUNTER_SIZE = 50;

    InitWindow(SCREEN_W, SCREEN_H, "red");
    SetTargetFPS(60);

	Vec2 playerPos(WORLD_W / 2.0f - PLAYER_SIZE / 2.0f, WORLD_H / 2.0f - PLAYER_SIZE / 2.0f);
    float speed = 300.0f;

    Camera2D camera = {};
    camera.offset = Vector2{ SCREEN_W / 2.0f, SCREEN_H / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

	float timer = 0.0f;
	int turn = 0;
    bool huntersFaster = false;
    float notificationTimer = 0.0f;

    enum GameState {
        WAIT_FOR_START,
        PLAYING, 
        GAME_OVER, 
        WIN 
    };
	GameState state = WAIT_FOR_START;

    while (state == WAIT_FOR_START && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (IsKeyPressed(KEY_ENTER)) { state = PLAYING; }
        DrawText("Welcome to the Hunter Game!\n"
            "Your goal is to avoid the hunters for as long as possible.\n"
            "The hunters will move towards you each turn.\n"
            "You can move in four directions: up, down, left, and right.\n"
            "The game ends when a hunter catches you.\n\n"
            "***How to Play***\n"
            "Controls: w = up, a = left, s = down, d = right, q = quit\n"
            "Player starts at the center of the world.\n"
            "Hunters start at\n", 40, 40, 20, BLACK);
		for (int i = 0; i < hunters.size(); i++) {
			DrawText(("Hunter " + std::to_string(i + 1) + ": (" + std::to_string((int)hunters[i].getPosition().x) + ", " + std::to_string((int)hunters[i].getPosition().y) + ")\n").c_str(), 40, 260 + (i * 25), 20, BLACK);
		}
        DrawText("Press Enter to start...", 40, 300 + (hunters.size() * 25), 20, BLACK);
        EndDrawing();
    }

    while (!WindowShouldClose()) 
    {
        float dt = GetFrameTime();

        if (state == PLAYING) {
		timer += dt;
            if (huntersFaster) {
                notificationTimer -= dt;
                if (notificationTimer <= 0.0f) {
                    huntersFaster = false;
                }
            }

            if (timer >= 10.0f) {
                timer = 0.0f;
                turn++;
                for (int i = 0; i < hunters.size(); i++) {
                    hunters[i].increaseSpeed(30.0f);
                }
                huntersFaster = true;
                notificationTimer = 3.0f;
            }

            Vec2 dir(0.0f, 0.0f);
            if (IsKeyDown(KEY_D)) dir.x += 1.0f;
            if (IsKeyDown(KEY_A)) dir.x -= 1.0f;
            if (IsKeyDown(KEY_S)) dir.y += 1.0f;
            if (IsKeyDown(KEY_W)) dir.y -= 1.0f;

            dir = dir.normalize();
            playerPos = playerPos + dir * speed * dt;
            if (IsKeyDown(KEY_Q)) break;

            if (playerPos.x < 0.0f) playerPos.x = 0.0f;
            if (playerPos.y < 0.0f) playerPos.y = 0.0f;
            if (playerPos.x > WORLD_W - PLAYER_SIZE) playerPos.x = WORLD_W - PLAYER_SIZE;
            if (playerPos.y > WORLD_H - PLAYER_SIZE) playerPos.y = WORLD_H - PLAYER_SIZE;

            camera.target = Vector2{ playerPos.x, playerPos.y };

            for (int i = 0; i < hunters.size(); i++) {
                hunters[i].moveToward(playerPos, dt);
            }

            bool caught = false;

            for (int i = 0; i < hunters.size(); i++) {
                if (hunters[i].hasCaught(playerPos, PLAYER_SIZE)) {
                    caught = true;
                    break;
                }
            }

            if (caught) {state = GAME_OVER;}
            else if (turn >= 3) {state = WIN;}
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (state == PLAYING) {
            BeginMode2D(camera);
            DrawRectangleLinesEx(Rectangle{ 0.0f, 0.0f, (float)WORLD_W + 20.0f, (float)WORLD_H + 20.0f }, 20.0f, DARKGRAY);

            DrawRectangle((int)playerPos.x, (int)playerPos.y, PLAYER_SIZE, PLAYER_SIZE, RED);
            for (int i = 0; i < hunters.size(); i++) {
                DrawRectangle((int)hunters[i].getPosition().x, (int)hunters[i].getPosition().y, HUNTER_SIZE, HUNTER_SIZE, BLUE);
            }
            EndMode2D();
			DrawText(("Turn: " + std::to_string(turn)).c_str(), 40.0f, SCREEN_H - 40.0f, 40, BLACK); //turn counter
			DrawText(("Time until next turn: " + std::to_string((int)(10.0f - timer)) + " seconds").c_str(), 40.0f, SCREEN_H - 80.0f, 20, BLACK); //timer)

            if (huntersFaster) {
                DrawText("*** The hunters are getting faster! ***", 40, 40, 20, RED);

                for (int i = 0; i < hunters.size(); i++) {
                    DrawText(("Hunter " + std::to_string(i + 1) + ": " + std::to_string((int)hunters[i].getSpeed())).c_str(), 40, 70 + (i * 25), 20, BLACK);
                }
            }
        }

        else if (state == GAME_OVER) {
			DrawText(("*** GAME OVER ***\n"
				"You survived " + std::to_string(turn) + " turns!\n\n"
				"*** Hunter caught you at\n(" + std::to_string((int)playerPos.x) + ", " + std::to_string((int)playerPos.y) + ") ***\n").c_str(), 40.0f, 40.0f, 60, BLACK);
		}
		else if (state == WIN) {
			DrawText("\n*** YOU WIN! ***\n"
				"You survived 3 turns!\n\n", 40.0f, 40.0f, 60, BLACK);
		}
        EndDrawing();
    }
    CloseWindow();
}