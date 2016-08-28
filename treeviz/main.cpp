#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include "random-bst/RandomBST.h"

#include "hume/GFX/Graphics.h"

hume::WindowSettings window_settings;
int32_t camera_x = -400, camera_y = -40;
bool dir_up, dir_down, dir_left, dir_right;
float min_sp = 100, max_sp = -10;
int16_t min_samples = 100, max_samples = -10;

struct payload
{
	uint16_t vals;
	float sp_cond;

	int32_t x;
	int32_t y;
	uint32_t spacing;
};

void average_values(RandomBSTNode* const root);
void assign_positions(RandomBSTNode* const root, uint32_t x = 0, uint32_t y = 0, uint32_t spacing = 500);
void draw_nodes(hume::Graphics* const gfx, const RandomBSTNode* const root);

int main()
{
	// Get different results each time!
	srand(time(nullptr));

	ifstream file;
	file.open("temp_odo.txt");
	assert(file.is_open());

	float temp, sp_cond;
	vector<struct payload*> draw_queue;
	RandomBSTNode* root = nullptr;
	uint32_t count = 0;
	while(file >> temp >> sp_cond)
	{
		if(sp_cond > max_sp) max_sp = sp_cond;
		else if(sp_cond < min_sp) min_sp = sp_cond;

		if(!randombst_find(root, temp))
		{
			struct payload* p = new struct payload;
			p->sp_cond = sp_cond;
			p->vals = 1;
			root = randombst_insert(root, temp, (void*) p);
			draw_queue.push_back(p);
		}
		else
		{
			RandomBSTNode* result = randombst_find(root, temp);
			assert(result);
			struct payload* p = (struct payload*) result->data;
			p->vals++;
			p->sp_cond += sp_cond;
		}
		count++;
	}

	cout << "Inserted " << count << " values." << endl;

	average_values(root);
	cout << "Averaged values." << endl;

	assign_positions(root);
	cout << "Assigned positions." << endl;

	// Get graphics stuff ready...
	window_settings.width = 1200;
	window_settings.height = 900;
	window_settings.fullscreen = false;
	window_settings.title = "treeviz";
	hume::Window* window = new hume::Window();
	window->apply(window_settings);
	window->create();

	hume::Graphics* gfx = new hume::Graphics();
	gfx->set_window(window);
	gfx->initialize();

	// Draw!
	int32_t v_vel = 0;
	int32_t h_vel = 0;
	hume::Properties props;
	bool running = true;
	while(running)
	{
		SDL_Event e;
		while(SDL_PollEvent(&e))
		{
			if(e.type == SDL_QUIT)
			{
				running = false;
			}
			else if(e.type == SDL_KEYDOWN)
			{
				switch(e.key.keysym.sym)
				{
				case SDLK_UP:
					dir_up = true;
					break;
				case SDLK_DOWN:
					dir_down = true;
					break;
				case SDLK_LEFT:
					dir_left = true;
					break;
				case SDLK_RIGHT:
					dir_right = true;
					break;
				default:
					break;
				}
			}
			else if(e.type == SDL_KEYUP)
			{
				switch(e.key.keysym.sym)
				{
				case SDLK_UP:
					dir_up = false;
					v_vel = 0;
					break;
				case SDLK_DOWN:
					dir_down = false;
					v_vel = 0;
					break;
				case SDLK_LEFT:
					dir_left = false;
					h_vel = 0;
					break;
				case SDLK_RIGHT:
					dir_right = false;
					h_vel = 0;
					break;
				default:
					break;
				}
			}
		}

		// update
		if(dir_up && v_vel > -20)
			v_vel--;
		else if(dir_down && v_vel < 20)
			v_vel++;
		else if(dir_left && h_vel > -20)
			h_vel--;
		else if(dir_right && h_vel < 20)
			h_vel++;
		camera_x += h_vel;
		camera_y += v_vel;

		// display
		gfx->set_color(0, 0, 0, 255);
		gfx->clear();

		draw_nodes(gfx, root);

		gfx->refresh();

		SDL_Delay(10);
	}

	return 0;
}

void average_values(RandomBSTNode* const root)
{
	if(root == nullptr) return;
	struct payload* const p = (struct payload*) root->data;
	if(p->vals > max_samples) max_samples = p->vals;
	else if(p->vals < max_samples) min_samples = p->vals;
	p->sp_cond = p->sp_cond / p->vals;
	average_values(root->left);
	average_values(root->right);

	return;
}

void assign_positions(RandomBSTNode* const root, uint32_t x, uint32_t y, uint32_t spacing)
{
	if(root == nullptr) return;
	struct payload* const p = (struct payload*) root->data;
	p->x = x;
	p->y = y;
	p->spacing = spacing;

	assign_positions(root->left, x - spacing, y + 20, spacing / 2);
	assign_positions(root->right, x + spacing, y + 20, spacing / 2);

	return;
}

void draw_nodes(hume::Graphics* const gfx, const RandomBSTNode* const root)
{
	if(root == nullptr) return;

	const uint8_t draw_size = 3;

	SDL_Rect viewport;
	SDL_RenderGetViewport(gfx->get_window()->get_renderer(), &viewport);
	const struct payload* p = (struct payload*) root->data;

	hume::Properties props;
	props.x = p->x - camera_x;
	props.y = p->y - camera_y;

	if(!(props.x < 0 || props.x >= window_settings.width ||
		 props.y < 0 || props.y >= window_settings.height))
	{
		// draw vertices
		gfx->set_color(219, 219, 219, 255);
		if(root->left)
			gfx->draw_line(props.x + (draw_size / 2), props.y + (draw_size / 2), props.x - (p->spacing) + (draw_size / 2), props.y + 20 + (draw_size / 2));
		if(root->right)
			gfx->draw_line(props.x + (draw_size / 2), props.y + (draw_size / 2), props.x + (p->spacing) + (draw_size / 2), props.y + 20 + (draw_size / 2));
		const float perc_of_sp_cond_max = ((p->sp_cond - min_sp) / (max_sp - min_sp));
		const float perc_of_samples_max = ((p->vals - min_samples) / (max_samples - min_samples));
		gfx->set_color(255, 255 - (perc_of_sp_cond_max * 255), 255 - (perc_of_sp_cond_max * 255), perc_of_samples_max * 255);
		gfx->draw_rect(props.x, props.y, draw_size, draw_size);
	}
	
	draw_nodes(gfx, root->left);
	draw_nodes(gfx, root->right);

	return;
}
