#pragma once

#include <bitset>
#include <cstdlib>
#include <cwchar>
#include <map>
#include <string>
#include <string_view>
#include <tuple>

#include <windows.h>

#include "shapes.hpp"
#include "string.hpp"

class Paddle : public Drawable,
               public Animatable<bool, bool>,
               public Animatable<float> {
public:
  Paddle(float x, float y, float width, float height, auto &&color)
      : x{x}, y{y}, width{width}, height{height},
        color{std::forward<decltype(color)>(color)} {}

  virtual void update(bool up, bool down) override {

    if (up && y < 1 - (height / 2)) {
      y += speed;
      moving = true;
      return;
    } else if (down && y > -1 + (height / 2)) {
      y -= speed;
      moving = true;
    } else {
      moving = false;
    }
    rct.update({x, y}, width, height);
    return;
  }

  virtual void update(float aspect) override {
    if (x > 0) {
      rct.update({0.9 * aspect, y}, width, height);
      x = 0.9 * aspect;
    } else {
      rct.update({-0.9 * aspect, y}, width, height);
      x = -0.9 * aspect;
    }
    return;
  }

  virtual void draw() override { rct.draw(); }

  std::tuple<float, float> getPos() { return {x, y}; }
  std::tuple<float, float> getSize() { return {width, height}; }
  bool isMoving() { return moving; }

private:
  float x, y, width, height;
  glm::vec3 color;
  float speed = 0.015;
  bool moving = false;

  Shapes::Rectangle rct{glm::vec2{x, y}, width, height, color};
};

class Ball : public Drawable,
             public Animatable<>,
             public Animatable<Paddle &, Paddle &>,
             public Animatable<float, float, glm::vec2>,
             public Animatable<float, float> {

public:
  Ball(float x, float y, float width, auto &&speed)
      : rect{glm::vec2{x, y}, width, width, glm::vec3{0.2, 0.2, 0.2}},
        speed{std::forward<decltype(speed)>(speed)}, x{x}, y{y}, width{width} {}

  virtual void draw() override {
    rect.update(glm::vec2{x, y}, width, width);
    rect.draw();
  }

  virtual void update() override {
    x += speed.x;
    y += speed.y;
    if (y + (width / 2) > 1 || y - (width / 2) < -1) {
      speed.y *= -1;
    };
  }

  virtual void update(Paddle &rPaddle, Paddle &lPaddle) override {
    auto &&[lx, ly] = lPaddle.getPos();
    auto &&[lw, lh] = lPaddle.getSize();

    auto &&[rx, ry] = rPaddle.getPos();
    auto &&[rw, rh] = rPaddle.getSize();

    auto topY = y + (width / 2);
    auto bottomY = y - (width / 2);

    // left paddle
    auto isBallInLeftPaddleXrange = x - (width / 2) < lx + (lw / 2);

    auto isBallInLeftPaddleYrange =
        ly - (lh / 2) < topY && topY < ly + (lh / 2) ||
        ly - (lh / 2) < bottomY && bottomY < ly + (lh / 2);

    auto isBallInRightPaddleXrange = rx - (rw / 2) < x + (width / 2);

    auto isBallInRightPaddleYrange =
        ry - (rh / 2) < topY && topY < ry + (rh / 2) ||
        ry - (rh / 2) < bottomY && bottomY < ry + (rh / 2);

    if (isBallInLeftPaddleXrange && isBallInLeftPaddleYrange) {
      if (lPaddle.isMoving()) {
        speed.y += getRandomf(-0.005, 0.005);
      }

      if (speed.x < 0.0f) {
        speed.x *= -1;
      }
    }

    if (isBallInRightPaddleXrange && isBallInRightPaddleYrange) {
      if (rPaddle.isMoving()) {
        speed.y += getRandomf(-0.005, 0.005);
      }
      if (speed.x > 0.0f) {
        speed.x *= -1;
      }
    }
  }

  virtual void update(float x, float y, glm::vec2 speed) override {
    this->x = x;
    this->y = y;
    this->speed = speed;
  }

  virtual void update(float x, float y) override {
    this->x = x;
    this->y = y;
  }

  std::tuple<float, float> getPos() { return {x, y}; }

private:
  Shapes::Rectangle rect;
  float x, y;
  float width;

  glm::vec2 speed;

  inline float getRandomf(float min, float max) {
    return min + (max - min) * ((float)rand() / (float)RAND_MAX);
  }
};

class Pong : public Drawable, public Animatable<> {
private:
  enum gameState { beginGame, gamePlaying, attackPlayer, goal, over };

  gameState currentGameState = gameState::beginGame;

public:
  Pong(std::wstring playerNameL, std::wstring playerNameR)
      : rPlayer(playerNameR), lPlayer(playerNameL),
        leftPlayerScoreStr("../fonts/NotoSansJP-Bold.otf", L"0",
                           Ft2Wrap::getPoint(10, 0), glm::vec3{0.2, 0.2, 0.2},
                           glm::vec2{-0.75, 0.8}, String::center),
        rightPlayerScoreStr("../fonts/NotoSansJP-Bold.otf", L"0",
                            Ft2Wrap::getPoint(10, 0), glm::vec3{0.2, 0.2, 0.2},
                            glm::vec2{0.75, 0.8}, String::center),
        msgGAMEOVER("../fonts/NotoSansJP-Bold.otf", L"",
                    Ft2Wrap::getPoint(12, 0), glm::vec3{0.2, 0.2, 0.2},
                    glm::vec2{0, 0.2}, String::center),
        msgRestart("../fonts/NotoSansJP-Bold.otf", L"", Ft2Wrap::getPoint(5, 0),
                   glm::vec3{0.2, 0.2, 0.2}, glm::vec2{0, -0.15},
                   String::center) {
    player[playerNameL] = 0;
    player[playerNameR] = 0;
  }

  virtual void update() override {
    using enum Pong::gameState;
    switch (currentGameState) {
    case beginGame:
      begin();
      break;
    case gamePlaying:
      playing();
      break;
    case attackPlayer:
      playerAttack();
      break;
    case goal:
      ballGoaled();
      break;
    case over:
      gameOver();
    }
  }
  virtual void draw() override {
    for (auto &e : objects) {
      e->draw();
    }
  }

private:
  void playing() {
    ball.update(rPaddle, lPaddle);
    ball.update();
    movePaddle();

    auto &&ballX = std::get<0>(ball.getPos());
    auto &&rPaddleX = std::get<0>(rPaddle.getPos());
    auto &&lPaddleX = std::get<0>(lPaddle.getPos());

    if (rPaddleX < ballX) {
      setGameState(goal);
    } else if (ballX < lPaddleX) {
      setGameState(goal);
    }
  }

  void begin() {
    movePaddle();
    bool spacePressed = keyPresssed(VK_SPACE);
    if (spacePressed) {
      float v = ((float)rand() / (float)RAND_MAX) * 2;
      if (v > 1.0f) {
        ball.update(0, 0, glm::vec2{ballSpeed, 0});
      } else {
        ball.update(0, 0, glm::vec2{-ballSpeed, 0});
      }
      ball.update();
      setGameState(gamePlaying);
    }
  }

  void gameOver() {
    msgGAMEOVER.update(L"GAMEOVER");
    msgRestart.update(L"(press space to restart game)");
    ball.update(0, 0);

    if (keyPresssed(VK_SPACE)) {
      msgGAMEOVER.update(L"");
      msgRestart.update(L"");
      setGameState(beginGame);
      for (auto &&e : player) {
        auto &&[_, score] = e;
        score = 0;
      }
      rightPlayerScoreStr.update(std::to_wstring(player[rPlayer]));
      leftPlayerScoreStr.update(std::to_wstring(player[lPlayer]));
    }
  }

  void playerAttack() {
    movePaddle();
    auto &&[ballX, ballY] = ball.getPos();
    float dx = 0.15;
    if (ballX < 0.0f) { // goaled to left side
      auto &&[px, py] = lPaddle.getPos();
      ball.update(px + dx, py);
      if (keyPresssed(VK_SPACE)) {
        ball.update(px + dx, py, glm::vec2{ballSpeed, 0});
        setGameState(gamePlaying);
      }
    } else if (0.0f < ballX) { // goaled to right side
      auto &&[px, py] = rPaddle.getPos();
      ball.update(px - dx, py);
      if (keyPresssed(VK_SPACE)) {
        ball.update(px - dx, py, glm::vec2{-ballSpeed, 0});
        setGameState(gamePlaying);
      }
    }
  }

  void movePaddle() {
    bool oPressed = keyPresssed(0x4F); // up rirght paddle O
    bool lPressed = keyPresssed(0x4C); // down right paddle L

    bool ePressed = keyPresssed(0x51); // up left paddle Q
    bool dPressed = keyPresssed(0x41); // down left paddle A

    rPaddle.update(oPressed, lPressed);
    lPaddle.update(ePressed, dPressed);

    for (auto &e : {&rPaddle, &lPaddle}) {
      e->update(Window::getAspect());
    }
  }

  void ballGoaled() {
    movePaddle();
    auto &&[ballX, ballY] = ball.getPos();
    float dx = 0.15;
    if (ballX < 0.0f) { // goaled to left side
      player[rPlayer] += 1;
      rightPlayerScoreStr.update(std::to_wstring(player[rPlayer]));

    } else if (0.0f < ballX) { // goaled to right side
      player[lPlayer] += 1;
      leftPlayerScoreStr.update(std::to_wstring(player[lPlayer]));
    }
    if (player[lPlayer] < matchPoint && player[rPlayer] < matchPoint) {
      setGameState(attackPlayer);
    } else {
      setGameState(over);
    }
  }

  inline void setGameState(Pong::gameState state) { currentGameState = state; }

  inline bool keyPresssed(int virtualKey) {
    return GetKeyState(virtualKey) & 0x8000;
  }

  float ballSpeed = 0.02;
  float paddleWidth = 0.1;
  float paddleHeight = 0.3;
  float ballWidth = 0.15;

  const int matchPoint = 12;

  glm::vec3 paddleColor{0.2, 0.2, 0.2};

  Ball ball{0, 0, ballWidth, glm::vec2{ballSpeed, 0}};

  Paddle rPaddle{0.9f * Window::getAspect(), 0, paddleWidth, paddleHeight,
                 paddleColor};
  Paddle lPaddle{-0.9f * Window::getAspect(), 0, paddleWidth, paddleHeight,
                 paddleColor};
  String leftPlayerScoreStr, rightPlayerScoreStr;
  String msgGAMEOVER;
  String msgRestart;

  Drawable *objects[7]{&ball,
                       &rPaddle,
                       &lPaddle,
                       &leftPlayerScoreStr,
                       &rightPlayerScoreStr,
                       &msgGAMEOVER,
                       &msgRestart};

  std::wstring lPlayer, rPlayer;

  std::map<std::wstring, int> player;
};
