#include <iostream>
#include <cstdlib>
#include <ctime> 
#include <vector>
#include <string>
#include <SDL.h>
#include "SDL_ttf.h"


using namespace std;

SDL_Event event;
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* surface;
SDL_Texture* texture;
SDL_Rect rtext = { 0,0,0,0 };


class Coordinate {
public:
	int x;
	int y;

};



void init() {
	Coordinate screen;
	//format
	screen.x = 800;
	screen.y = 600;


	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		cout << "SDL_Error: " << SDL_GetError() << endl;
		exit(1);
	}

	if (TTF_Init() == -1) {
		cout << "TTF_Error: " << TTF_GetError() << endl;
		exit(1);
	}

	//Window and renderer
	window = SDL_CreateWindow("Encapsulation Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen.x, screen.y, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);




	//text for FPS
	TTF_Font* font;
	SDL_Color tcolor = { 255, 255, 255, 255 };
	font = TTF_OpenFont("arial.ttf", 25);

	if (!font) {
		cout << "SDL_ttf Error: " << TTF_GetError() << endl;
		exit(1);
	}

	char words[100] = { '\0' };
	strcpy_s(words, "FPS:");

	surface = TTF_RenderText_Solid(font, words, tcolor);
	if (!surface) {
		cout << "SDL_ttf Error: " << TTF_GetError() << endl;
		exit(1);
	}

	texture = SDL_CreateTextureFromSurface(renderer, surface);

	//format text
	rtext.w = 100;
	rtext.h = 45;

	//placement text
	rtext.x = 0 + 10;
	rtext.y = 0;
	SDL_FreeSurface(surface);

	//time
	srand(static_cast<unsigned int>(time(0)));
}


void destroy() {

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_Quit();
	SDL_Quit();
}
void quit() {
	SDL_PollEvent(&event);

	if (event.type == SDL_QUIT) {
		destroy();
	}
}





class Ball : Coordinate {

private: 
	int vx;
	int vy;

	int r;
	int g;
	int b;

	int globalx;
	int globaly;

	int radius;

public:


	//velocity
	int getVx() {return vx;}
	void setVx(int newVx) {vx = newVx;}
	int getVy() { return vy; }
	void setVy(int newVy) { vy = newVy; }


	//initial velocity
	int getGlobalx() { return globalx; }
	void setGlobalx(int newGlobalx) { globalx = newGlobalx; }
	int getGlobaly() { return globaly; }
	void setGlobaly(int newGlobaly) { globaly = newGlobaly; }


	//radius
	int getRadius() { return radius; }
	void setRadius(int newRadius) { radius = newRadius; }

	// R G B
	//r
	int getr() { return r; }
	void setr(int newr) { r = newr; }

	//g
	int getg() { return g; }
	void setg(int newg) { g = newg; }

	//b
	int getb() { return b; }
	void setb(int newb) { b = newb; }


	Ball() {

		//random generation
		radius = rand() % (20 - 10 + 1) + 10; //size

		//start point
		x = rand() % (800 - 2 * radius) + radius;
		y = rand() % (600 - 2 * radius) + radius;


		//start direction velocity
		globalx = rand() % 3 + 1;
		globaly = rand() % 3 + 1;
		vx = (rand() % 2 == 0) ? globalx : -globalx;
		vy = (rand() % 2 == 0) ? globaly : -globaly;

		//color
		r = rand() % 256;
		g = rand() % 256;
		b = rand() % 256;
	}


	//drawing circle function
	void draw_circle(SDL_Renderer* renderer, int x, int y, int radius, SDL_Color color)
	{
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		for (int w = 0; w < radius * 2; w++)
		{
			for (int h = 0; h < radius * 2; h++)
			{
				int dx = radius - w;
				int dy = radius - h;
				if ((dx * dx + dy * dy) <= (radius * radius))
				{
					SDL_RenderDrawPoint(renderer, x + dx, y + dy);
				}
			}
		}
	}

	void collisionBorder() { //always put the ball on the border before bouncing
		if (x - radius < 0) {
			x = radius;
			vx = -vx;
		}
		if (x + radius > 800) {
			x = 800 - radius;
			vx = -vx;
		}
		if (y - radius < 0) {
			y = radius;
			vy = -vy;
		}
		if (y + radius > 600) {
			y = 600 - radius;
			vy = -vy;
		}
	}

	void collisionBall(Ball& ballB) {
		int dx = x - ballB.x;
		int dy = y - ballB.y;
		int distance = sqrt(dx * dx + dy * dy);
		int overlap = radius + ballB.radius - distance; // Calculate the overlap


		//detect if ball is in another ball
		if (overlap > 0) {

			float distanceFactor = 1.0f / distance;
			float nx = dx * distanceFactor;
			float ny = dy * distanceFactor;

			// Unclip balls
			x += nx * overlap / 2;
			y += ny * overlap / 2;
			ballB.x -= nx * overlap / 2;
			ballB.y -= ny * overlap / 2;

			//swap balls velocities to make then "bounce"  eachother
			swap(vx, ballB.vx);
			swap(vy, ballB.vy);
		}
	}

	void ballFunction() {
		SDL_Color rBallColor = { r, g, b, 255 };
		draw_circle(renderer, x, y, radius, rBallColor);

		// move
		x += vx;
		y += vy;

		collisionBorder();
	}

};

int main(int argc, char* argv[]) {

	init();

	vector<Ball> ballVector; //Init infinite ball vecor

	//init 
	Uint32 ballTime = 0;


	while (true) {

		quit();

		//background
		SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
		SDL_RenderClear(renderer);


		Uint32 currentTime = SDL_GetTicks();
		if (currentTime - ballTime >= 1000) { //1000 = 1s, check if the timer reaches 1 seconds
			ballVector.emplace_back(); // add 1 more index to vector Balls
			ballTime = currentTime; // apply cooldown
		}


		for (int i = 0; i < ballVector.size(); i++) {
			for (int j = i + 1; j < ballVector.size(); j++) {
				ballVector[i].collisionBall(ballVector[j]);
			}
		}


		//display every ball
		for (Ball& ball : ballVector) {
			ball.ballFunction();
		}


		//display text
		SDL_RenderCopy(renderer, texture, NULL, &rtext);

		//Update 
		SDL_RenderPresent(renderer);

	}

	destroy();
}