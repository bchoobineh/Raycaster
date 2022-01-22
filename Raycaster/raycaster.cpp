#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <algorithm>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_TITLE "Raycaster"
#define FPS 60
#define FRAME_TIME_MAX 1000/FPS
#define mapWidth 24
#define mapHeight 24
#define texWidth 64
#define texHeight 64

// Image Resources
std::vector<SDL_Surface*> images;

// Player params
double posX, posY;
double dirX, dirY;
double planeX, planeY;

// Time params
double time;
double oldTime;
bool running;

// Keyboard
const Uint8* keyState = SDL_GetKeyboardState(NULL);

// Window and renderer
SDL_Window* window;
SDL_Renderer* renderer;

// World Map
int worldMap[mapWidth][mapHeight] =
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


// Function to get a specific pixel on a surface
Uint32 getPixel(SDL_Surface* surface, int x, int y)
{
	SDL_LockSurface(surface);

	int bytesPerPixel = surface->format->BytesPerPixel;
	Uint8* pixelAddress = (Uint8*)surface->pixels + y * surface->pitch + x * bytesPerPixel;
	Uint32 pixel;

	// Handle 4 possible options for bytes per pixel
	switch (bytesPerPixel)
	{
		case 1:
			pixel = *pixelAddress;
			break;

		case 2:
			pixel =  *(Uint16*)pixelAddress;
			break;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				pixel = pixelAddress[0] << 16 | pixelAddress[1] << 8 | pixelAddress[2];
			else
				pixel = pixelAddress[0] | pixelAddress[1] << 8 | pixelAddress[2] << 16;
			break;

		case 4:
			pixel = *(Uint32*)pixelAddress;
			break;

		default:
			return 0;
			break;
	}

	SDL_UnlockSurface(surface);
	return pixel;

}

// Initialize raycaster parameters
void initRaycaster()
{
	// Initialize player starting parameters
	posX = 22;
	posY = 12;
	dirX = -1;
	dirY = 0;
	planeX = 0;
	planeY = 0.66;

	// Initialize time
	time = 0;
	oldTime = 0;

	// Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	SDL_SetWindowTitle(window, SCREEN_TITLE);

	// Load image textures
	// Tiles
	images.push_back(IMG_Load("eagle.png"));
	images.push_back(IMG_Load("redbrick.png"));
	images.push_back(IMG_Load("purplestone.png"));
	images.push_back(IMG_Load("greystone.png"));
	images.push_back(IMG_Load("bluestone.png"));
	images.push_back(IMG_Load("mossy.png"));
	images.push_back(IMG_Load("wood.png"));
	images.push_back(IMG_Load("colorstone.png"));

	// Sprites
	images.push_back(IMG_Load("barrel.png"));
	images.push_back(IMG_Load("pillar.png"));
	images.push_back(IMG_Load("greenlight.png"));
}

// Do cleanup operations for raycaster
void cleanupRaycaster()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void updateGame(double frameTime)
{
	// Speeds
	double moveSpeed = frameTime * 5.0;
	double rotSpeed = frameTime * 3.0;

	// Get current state
	SDL_PumpEvents();

	// Move UP
	if (keyState[SDL_SCANCODE_UP])
	{
		if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false)  // Move X
			posX += dirX * moveSpeed;

		if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) // Move Y
			posY += dirY * moveSpeed;
	}

	// Move Down
	if (keyState[SDL_SCANCODE_DOWN])
	{
		if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) // Move X
			posX -= dirX * moveSpeed;

		if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) // Move Y
			posY -= dirY * moveSpeed;
	}

	// Rotate Left
	if (keyState[SDL_SCANCODE_LEFT])
	{
		double oldDirX = dirX;
        dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
        dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
        double oldPlaneX = planeX;
        planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
        planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
	}

	// Rotate Right
	if (keyState[SDL_SCANCODE_RIGHT])
	{
		double oldDirX = dirX;
        dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
        dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
        double oldPlaneX = planeX;
        planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
        planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
	}

	// Exit Game
	if (keyState[SDL_SCANCODE_ESCAPE])
		running = false;
}

void raycastWalls()
{
	// Direct reference to: https://lodev.org/cgtutor/raycasting.html
	// Draw walls for each x in the map
	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		// X coord of camera plane ranging between -1 and 1
		// Used to determine direction of ray
		double cameraX = 2 * x / double(SCREEN_WIDTH) - 1; 

		// Get ray direction vectors
		double rayDirX = dirX + planeX * cameraX;
		double rayDirY = dirY + planeY * cameraX;

		// Which tile in the map are we located
		int mapX = int(posX);
		int mapY = int(posY);

		// Initial distance from ray to intersect with first x-side or y-side of the grid
		double sideDistX;
		double sideDistY;

		// Distance from one x-side or y-side to the next
		double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
        double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);

		// Distance of perpendicular ray to ray hit
		double perpWallDist;

		// What direction to step in when casting the ray
		int stepX, stepY;

		// Wall was hit?
		int hit = 0;

		// Side wall was hit?
		int side;

		// Get side distances and step values
		if (rayDirX < 0)
		{
			stepX = -1;
			sideDistX = (posX - mapX) * deltaDistX;
		}
		else
		{
			stepX = 1;
			sideDistX = (mapX + 1.0 - posX) * deltaDistX;
		}
		if (rayDirY < 0)
		{
			stepY = -1;
			sideDistY = (posY - mapY) * deltaDistY;
		}
		else
		{
			stepY = 1;
			sideDistY = (mapY + 1.0 - posY) * deltaDistY;
		}

		// Trace the ray
		while (hit == 0)
		{
			// Jump to next x square
			if (sideDistX < sideDistY)
			{
				sideDistX += deltaDistX;
				mapX += stepX;
				side = 0;
			}
			// Jump to next y square
			else
			{
				sideDistY += deltaDistY;
				mapY += stepY;
				side = 1;
			}

			// Check if we have hit anything
			if (worldMap[mapX][mapY] > 0)
				hit = 1;
		}

		// Calculate perpendicular ray distance
		// Counter fish-eye lens effect
		if (side == 0)
			perpWallDist = (sideDistX - deltaDistX);
		else
			perpWallDist = (sideDistY - deltaDistY);

		// Find height of line to draw
		int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);

		// Calculate y location for line start
		int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
		if (drawStart < 0) 
			drawStart = 0;

		// Calculate y location for line end
		int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
		if (drawEnd >= SCREEN_HEIGHT) 
			drawEnd = SCREEN_HEIGHT - 1;

		// What texture are we drawing
		int texNum = worldMap[mapX][mapY] - 1; // 0 index

		// Find where the wall was hit
		double wallX;
		if(side == 0)
			wallX = posY + perpWallDist * rayDirY;
		else
			wallX = posX + perpWallDist * rayDirX;
		wallX -= floor((wallX));

		// Find x-coordinate of texture to use
		int texX = int(wallX * double(texWidth));
		if (side == 0 && rayDirX > 0) 
			texX = texWidth - texX - 1;
		if (side == 1 && rayDirY < 0) 
			texX = texWidth - texX - 1;

		// Texture pixel step increase
		double step = 1.0 * texHeight / lineHeight;

		// Initial texture coordinate
		double texPos = (drawStart - SCREEN_HEIGHT / 2 + lineHeight / 2) * step;

		for (int y = drawStart; y < drawEnd; y++)
		{
			int texY = (int)texPos & (texHeight - 1);
			texPos += step; // move to next texture location

			// Get color of pixel of the texture
			Uint32 color = getPixel(images[texNum], texX, texY);
			
			// Shadow
			if (side == 1) 
				color = (color >> 1) & 8355711;

			Uint8 r, g, b, a;
			SDL_GetRGBA(color, images[texNum]->format, &r, &g, &b, &a);

			SDL_SetRenderDrawColor(renderer, r, g, b, a);
			SDL_RenderDrawPoint(renderer, x, y);
		}
	}
}

void raycastFloor()
{
	// Direct reference to: https://lodev.org/cgtutor/raycasting.html
	for (int y = SCREEN_HEIGHT / 2 + 1; y < SCREEN_HEIGHT; ++y)
	{
		float rayDirX0 = dirX - planeX;
		float rayDirY0 = dirY - planeY;
		float rayDirX1 = dirX + planeX;
		float rayDirY1 = dirY + planeY;

		// horizon
		int p = y - SCREEN_HEIGHT / 2;

		// Vertical position of the camera 
		float posZ = 0.5 * SCREEN_HEIGHT;

		float rowDistance = posZ / p;


		float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / SCREEN_WIDTH;
		float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / SCREEN_WIDTH;

		float floorX = posX + rowDistance * rayDirX0;
		float floorY = posY + rowDistance * rayDirY0;

		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			int cellX = (int)(floorX);
			int cellY = (int)(floorY);

			int tx = (int)(texWidth * (floorX - cellX)) & (texWidth - 1);
			int ty = (int)(texHeight * (floorY - cellY)) & (texHeight - 1);

			floorX += floorStepX;
			floorY += floorStepY;

			int floorTexture = 6;
			int ceilingTexture = 7;

			Uint32 color = getPixel(images[floorTexture], tx, ty);
			color = (color >> 1) & 8355711; // make a bit darker

			Uint8 r, g, b, a;
			SDL_GetRGBA(color, images[floorTexture]->format, &r, &g, &b, &a);
			SDL_SetRenderDrawColor(renderer, r, g, b, a);
			SDL_RenderDrawPoint(renderer, x, y);


			color = getPixel(images[ceilingTexture], tx, ty);
			color = (color >> 1) & 8355711; // make a bit darker

			SDL_GetRGBA(color, images[floorTexture]->format, &r, &g, &b, &a);
			SDL_SetRenderDrawColor(renderer, r, g, b, a);
			SDL_RenderDrawPoint(renderer, x, SCREEN_HEIGHT - 1 - y);

		}

	}
}

void gameLoop()
{
	// Start game
	running = true;
	while (running)
	{
		// Clear screen buffer
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		// Raycast!
		raycastFloor();
		raycastWalls();

		// Draw Game state
		SDL_RenderPresent(renderer);

		// Calculate Frame Time
		oldTime = time;
        time = SDL_GetTicks();
        double frameTime = (time - oldTime) / 1000.0; //frametime is the time this frame has taken, in seconds

		// Update Game
		updateGame(frameTime);		
	}
}

int main(int argc, char* argv[])
{
	// Initialize Raycaster
	initRaycaster();

	// Start game loop
	gameLoop();

	// Cleanup Raycaster
	cleanupRaycaster();

	return 0;
}