#include <iostream>
#include <SDL.h>
#include <vector>
#include <string>

#define _USE_MATH_DEFINES
#include <math.h>

#include <Eigen/Dense>

#include <SDL_ttf.h>

using namespace Eigen;

struct Node {
	std::string name;
	SDL_Rect rect;
	SDL_Surface* text_surf;
	SDL_Texture* text_texture;
};

struct Connection {
	Node* n1;
	Node* n2;
	float value;
};
	
void draw_line(SDL_Renderer* renderer, Node* n1, Node* n2) {
	double src_x = n1->rect.x + n1->rect.w / 2;
	double src_y = n1->rect.y + n1->rect.h / 2;

	double dst_x = n2->rect.x + n2->rect.w / 2;
	double dst_y = n2->rect.y + n2->rect.h / 2;
	if (n1 != n2) {
		double angle = atan((src_y - dst_y) / (src_x - dst_x));

		double additor = 0;
		if (dst_x > src_x) {
			additor = M_PI;
		}

		double nsrc_x = src_x - (n1->rect.w / 2) * cos(angle - additor + M_PI / 4);
		double nsrc_y = src_y - (n1->rect.h / 2) * sin(angle - additor + M_PI / 4);
		double ndst_x = dst_x - (n2->rect.w / 2) * cos(angle - additor - M_PI);
		double ndst_y = dst_y - (n2->rect.h / 2) * sin(angle - additor - M_PI);

		SDL_RenderDrawLine(renderer, nsrc_x, nsrc_y, ndst_x, ndst_y);
		float r = 15;
		SDL_RenderDrawLine(renderer, ndst_x, ndst_y, ndst_x + r * cos(-M_PI / 4 + angle + additor), ndst_y + r * sin(-M_PI / 4 + angle + additor));
		SDL_RenderDrawLine(renderer, ndst_x, ndst_y, ndst_x + r * cos(M_PI / 4 + angle + additor), ndst_y + r * sin(M_PI / 4 + angle + additor));
	}
	else {
	}
}

int main(int argc, char* argv[]){

	const int GRAPH_SIZE = 7;

	std::vector<Node> graph;
	std::vector<Connection> connections;

	int node_size = 48;
	for (int i = 0; i < GRAPH_SIZE; i++) {
		graph.push_back(Node{ std::string{ (char)('A' + i)}, SDL_Rect{0,0,node_size,node_size}});
	}

	int graph_size = graph.size();
	float r = 256;
	float center_x = 1280/2;
	float center_y = 720/2;

	for (int i = 0; i < graph_size; i++) {
		graph[i].rect.x = center_x + r * cos((2 * M_PI / graph_size) * i) - node_size/2;
		graph[i].rect.y = center_y + r * sin((2 * M_PI / graph_size) * i) - node_size / 2;
	}


	//MATRIX

	Eigen::MatrixXd mat(GRAPH_SIZE, GRAPH_SIZE);
	
	for (int i = 0; i < GRAPH_SIZE; i++) {
		for (int j = 0; j < GRAPH_SIZE; j++) {
			mat(i, j) = i * j+1;
			Connection c = { &graph[i], &graph[j], mat(i,j) };
			connections.push_back(c);
		}
	}

	//--------------


	// SDL

	TTF_Init();
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_CreateWindowAndRenderer(1280, 720, 0, &window, &renderer);

	// SDL_TEXT

	TTF_Font* font = TTF_OpenFont("C:/Windows/Fonts/ariblk.ttf", 27);


	for (Node& node : graph) {
		node.text_surf = TTF_RenderText_Solid(font, node.name.c_str(), SDL_Color{255,0,0,255});
		node.text_texture = SDL_CreateTextureFromSurface(renderer, node.text_surf);
	}

	// --------------

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		SDL_WaitEvent(&e);
		switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		SDL_RenderDrawPoint(renderer, center_x, center_y);


		for (Node& node : graph) {
			SDL_RenderDrawRect(renderer, &node.rect);
			SDL_RenderCopy(renderer, node.text_texture, NULL, &node.rect);
		}

		for (Connection & conn : connections) {
			if (conn.value != 0) {
				draw_line(renderer, conn.n1, conn.n2);
			}
		}

		SDL_RenderPresent(renderer);

	}


	TTF_CloseFont(font); 
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}