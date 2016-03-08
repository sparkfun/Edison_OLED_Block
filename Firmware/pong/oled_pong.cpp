/******************************************************************************
* pong.cpp
* Play a game of pong on the Edison OLED Block!
* Jim Lindblom @ SparkFun Electronics
* Original Creation Date: January 8, 2014
*
* This sketch uses the Edison OLED library to draw a 3-D projected
* cube, and rotate it along all three axes.
*
* This code is beerware; if you see me (or any other SparkFun employee) at the
* local, and you've found our code helpful, please buy us a round!
*
* Distributed as-is; no warranty is given.
******************************************************************************/

#include <iostream>
#include "oled/Edison_OLED.h"
#include "gpio/gpio.h"
#include "math.h"
#include <unistd.h> // for usleep
#include <stdlib.h> // Gives us atoi
#include <stdio.h>

using namespace std;

// Function prototypes:
void setupOLED();
void startScreen();
void updatePaddlePositions();
float constrainPosition(float position);
void moveBall();
void drawGame();
void drawScore(int player1, int player2);
void drawPaddle(int x, int y);
void drawBall(int x, int y);
int checkWin();
void drawWin(int player);
void cleanUp();

// Define an edOLED object:
edOLED oled;

// Pin definitions:
// All buttons have pull-up resistors on-board, so just declare
// them as regular INPUT's
gpio BUTTON_UP(47, INPUT);
gpio BUTTON_DOWN(44, INPUT);
gpio BUTTON_LEFT(165, INPUT);
gpio BUTTON_RIGHT(45, INPUT);
gpio BUTTON_SELECT(48, INPUT);
gpio BUTTON_A(49, INPUT);
gpio BUTTON_B(46, INPUT);

// Game Variables:
int scoreToWin = 10;
int playerScore = 0;
int player2Score = 0;

const float paddleWidth = LCDWIDTH/16.0;
const float paddleHeight = LCDHEIGHT/3.0;
const float halfPaddleWidth = paddleWidth/2.0;
const float halfPaddleHeight = paddleHeight/2.0;

float player1PosX = 1.0 + halfPaddleWidth;
float player1PosY = 0.0;
float player2PosX = LCDWIDTH - 1.0 - halfPaddleWidth;
float player2PosY = 0.0;
float enemyVelY = 0.5;

const float ballRadius = 2.0;
const float ballSpeedX = 1.0;
float ballPosX = LCDWIDTH/2.0;
float ballPosY = LCDHEIGHT/2.0;
float ballVelX = -1.0 * ballSpeedX;
float ballVelY = 0;

enum {
	PLAYER_1_WIN = 1,
	PLAYER_2_WIN = 2
};
enum {
	SINGLE_PLAYER,
	MULTI_PLAYER
};
int playMode = SINGLE_PLAYER;

int main(int argc, char * argv[])
{
	if (argc == 2)
	{
		scoreToWin = atoi(argv[1]);
	}
	printf("Playing to %d\r\n", scoreToWin);

	setupOLED();
	startScreen();

	while (1)
	{
		updatePaddlePositions();
		moveBall();
		drawGame();
		if (checkWin())
		{
			drawWin(checkWin());
			cleanUp();
			return 0;
		}
	}
}

void setupOLED()
{
	oled.begin();
	oled.clear(PAGE);
	oled.display();
	oled.setFontType(0);
}

void startScreen()
{
	// Reset all game variables:
	player2Score = 0;
	playerScore = 0;
	player2PosY = 0.0;
	ballPosX = LCDWIDTH/2.0;
	ballPosY = LCDHEIGHT/2.0;
	ballVelX = -1.0 * ballSpeedX;
	ballVelY = 0.0;

	// Draw the splash screen:
	oled.clear(PAGE);
	oled.setCursor(14, 5);
	oled.print("Press A");
	oled.setCursor(2, 13);
	oled.print("for single");
	oled.setCursor(14, 30);
	oled.print("Press B");
	oled.setCursor(6, 38);
	oled.print("for multi");
	// Call display to actually draw it on the OLED:
	oled.display();

	// Wait for either button A or B to be pressed:
	while ((BUTTON_A.pinRead() == HIGH) && (BUTTON_B.pinRead() == HIGH))
		;
	// If button A is pressed, play single player
	if (BUTTON_A.pinRead() == LOW)
		playMode = SINGLE_PLAYER;
	// If button B is pressed, play mutli-player
	else if (BUTTON_B.pinRead() == LOW)
		playMode = MULTI_PLAYER;
}

// Update the positions of the paddles:
void updatePaddlePositions()
{
	// Update player 1's paddle:
	if (BUTTON_UP.pinRead() == LOW)
	{
		player1PosY--;
	}
	if (BUTTON_DOWN.pinRead() == LOW)
	{
		player1PosY++;
	}
	player1PosY = constrainPosition(player1PosY);

	// Move player 2 paddle:
	if (playMode == SINGLE_PLAYER)
	{
		// Update AI's paddle:
		// Follow along with the ball's position:
		if (player2PosY < ballPosY)
		{
			player2PosY += enemyVelY;
		}
		else if(player2PosY > ballPosY)
		{
			player2PosY -= enemyVelY;
		}
	}
	else if (playMode == MULTI_PLAYER)
	{
		if (BUTTON_A.pinRead() == LOW)
		{
			player2PosY--;
		}
		if (BUTTON_B.pinRead() == LOW)
		{
			player2PosY++;
		}
	}
	player2PosY = constrainPosition(player2PosY);
}

// Constrain a paddle's position to within the display's border
float constrainPosition(float position)
{
	float newPaddlePosY = position;

	if (position - halfPaddleHeight < 0)
	{
		newPaddlePosY = halfPaddleHeight;
	}
	else if (position + halfPaddleHeight > LCDHEIGHT)
	{
		newPaddlePosY = LCDHEIGHT - halfPaddleHeight;
	}

	return newPaddlePosY;
}

// Move the ball and re-calculate its position:
void moveBall()
{
	ballPosY += ballVelY;
	ballPosX += ballVelX;

	// Top and bottom wall collisions
	if (ballPosY < ballRadius)
	{
		ballPosY = ballRadius;
		ballVelY *= -1.0;
	}
	else if (ballPosY > LCDHEIGHT - ballRadius)
	{
		ballPosY = LCDHEIGHT - ballRadius;
		ballVelY *= -1.0;
	}

	// Left and right wall collisions
	if (ballPosX < ballRadius)
	{
		ballPosX = ballRadius;
		ballVelX = ballSpeedX;
		player2Score++;
	}
	else if (ballPosX > LCDWIDTH - ballRadius)
	{
		ballPosX = LCDWIDTH - ballRadius;
		ballVelX *= -1.0 * ballSpeedX;
		playerScore++;
	}

	// Paddle collisions
	if (ballPosX < player1PosX + ballRadius + halfPaddleWidth)
	{
		if (ballPosY > player1PosY - halfPaddleHeight - ballRadius &&
				ballPosY < player1PosY + halfPaddleHeight + ballRadius)
		{
			ballVelX = ballSpeedX;
			ballVelY = 2.0 * (ballPosY - player1PosY) / halfPaddleHeight;
		}
	}
	else if (ballPosX > player2PosX - ballRadius - halfPaddleWidth)
	{
		if (ballPosY > player2PosY - halfPaddleHeight - ballRadius &&
				ballPosY < player2PosY + halfPaddleHeight + ballRadius)
		{
			ballVelX = -1.0 * ballSpeedX;
			ballVelY = 2.0 * (ballPosY - player2PosY) / halfPaddleHeight;
		}
	}
}

// Draw the paddles, ball and score:
void drawGame()
{
	oled.clear(PAGE);

	drawScore(playerScore, player2Score);
	drawPaddle(player1PosX, player1PosY);
	drawPaddle(player2PosX, player2PosY);
	drawBall(ballPosX, ballPosY);

	oled.display();
}

// Draw the two score integers on the screen
void drawScore(int player1, int player2)
{
	oled.setCursor(10, 2);
	oled.print(player1);
	oled.setCursor(50, 2);
	oled.print(player2);
}

// Draw a paddle, given it's x and y coord's
void drawPaddle(int x, int y)
{
	oled.rect(x - halfPaddleWidth,
			y - halfPaddleHeight,
			paddleWidth,
			paddleHeight);
}

// Draw a ball, give it's x and y coords
void drawBall(int x, int y)
{
	oled.circle(x, y, 2);
}

// Check if either player has won.
// Returns:
//	0 - Neither player has won.
//  1 - Player 1 has won
//  2 - Player 2 has won
int checkWin()
{
	if (playerScore >= scoreToWin)
	{
		return PLAYER_1_WIN;
	}
	else if (player2Score >= scoreToWin)
	{
		return PLAYER_2_WIN;
	}

	return 0;
}

// Draw the win screen.
// Keep it up for 5 seconds.
// Then go back to the splash screen.
void drawWin(int player)
{
	oled.setCursor(10, 2);
	oled.clear(PAGE);
	if (player == PLAYER_1_WIN)
	{
		oled.print("Player 1");
	}
	else if (player == PLAYER_2_WIN)
	{
		oled.print("Player 2");
	}
	oled.setCursor(20, 12);
	oled.print("Wins!");
	oled.display();

	usleep(5000000);
}

void cleanUp()
{
	oled.clear(PAGE);
	oled.display();
}
