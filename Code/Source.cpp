#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <math.h>
#include <SDL_mixer.h>


//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//The window we'll be rendering to
SDL_Window* window = NULL;
//The surface contained by the window
SDL_Surface* screenSurface = NULL;
SDL_Surface* surface = NULL;

#define FPS 60
int lastTime = 0, currentTime, deltaTime;
float msFrame = 1 / (FPS / 1000.0f);


#pragma region BasicSDL
int currentEffect = 0;
float currentEffectPlayTime = 0;
int currentTransition = -1;
static float MAX_EFFECT_TIME = 8000;
float totalTimePlayEffect; 

bool initSDL();
void init();
void InitTransitions();
void update();
void UpdateTransitions();
void render();
void RenderTransitions();
void clear();
void ClearTransitions();
void CheckEndOfEffect();
void close();
void waitTime();
#pragma endregion

#pragma region stars
// Stars
#define MAXSTARS 512

struct Vector2
{
	float x, y;
};

struct Star
{
	Vector2 tail[8];
	Vector2 position;             
	unsigned char plane;    
	int direction;
};

Star* stars;

void InitStars();
void UpdateStars();
void RenderStars();
void WritePixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
void ClearStars();
#pragma endregion

#pragma region RotoZoom

int Ax, Ay, Bx, By, Cx, Cy;

void InitRotoZoom();
void UpdateRotoZoom();
void RenderRotoZoom();
void ClearRotoZoom();
#pragma endregion

#pragma region Distortion

// displacement buffers
char* dispX, * dispY;
// define the distortion buffer movement
int windowx1, windowy1, windowx2, windowy2;

void InitDistortion();
void UpdateDistortion();
void RenderDistortion();
void ClearDistortion();
#pragma endregion

#pragma region Tunnel

unsigned char* tunnelBuffer;

void InitTunnel();
void UpdateTunnel();
void RenderTunnel();
void ClearTunnel();
#pragma endregion

#pragma region Plasma

unsigned char* plasma1;
unsigned char* plasma2;
SDL_Color palette[256];
long src1, src2;

void InitPlasma();
void UpdatePlasma();
void RenderPlasma();
void ClearPlasma();
#pragma endregion

#pragma region transitions

SDL_Surface* transitionSurface = NULL;
SDL_Rect rects[2];
bool isOpening = false;
Uint8 currentAlpha = 0;
bool isObscuring = true;
bool playingTransition = false;

void InitDoorsTransition();
void UpdateDoorsTransition();
void RenderDoorsTransition();
void ClearDoorsTransition();

void InitFadeTransition();
void UpdateFadeTransition();
void RenderFadeTransition();
void ClearFadeTransition();
#pragma endregion

#pragma region Mixer
Mix_Music* mySong;
#define BPM_MUSIC 66
#define MSEG_BPM (60000 / BPM_MUSIC)
#define FLASH_MAX_TIME 128
int flashtime;
int MusicCurrentTime;
int MusicCurrentTimeBeat;
int MusicCurrentBeat;
int MusicPreviousBeat;

void InitMusic();
void UpdateMusic();
#pragma endregion



int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!initSDL())
	{
		std::cout << "Failed to initialize!\n";
		return 1;
	}
	else
	{
		//Main loop flag
		bool quit = false;
		//Event handler
		SDL_Event e;
		//While application is running
		while (!quit)
		{
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_KEYDOWN) {
					if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
						quit = true;
					}
				}
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
			}

			// updates all
			update();

			//Render
			render();

			//Update the surface
			SDL_UpdateWindowSurface(window);
			CheckEndOfEffect();
			waitTime();
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}


bool initSDL() {

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "SDL could not initialize! SDL_Error: %s\n" << SDL_GetError();
		return false;
	}
	//Create window
	window = SDL_CreateWindow("PEC 1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (window == NULL)
	{
		std::cout << "Window could not be created! SDL_Error: %s\n" << SDL_GetError();
		return false;
	}
	//Get window surface
	screenSurface = SDL_GetWindowSurface(window);
	transitionSurface = SDL_CreateRGBSurface(
		0,
		screenSurface->w,
		screenSurface->h,
		screenSurface->format->BitsPerPixel,
		screenSurface->format->Rmask,
		screenSurface->format->Gmask,
		screenSurface->format->Bmask,
		0xFF000000
	);
	SDL_SetSurfaceBlendMode(transitionSurface, SDL_BLENDMODE_BLEND);	
	init();
	InitMusic();
	IMG_Init(IMG_INIT_PNG);
	return true;
}

void init() {
	switch (currentEffect)
	{
	case 0:
		InitStars();
		break;
	case 1:
		InitRotoZoom();
		break;
	case 2:
		InitDistortion();
		break;
	case 3:
		InitTunnel();
		break;
	case 4:
		InitPlasma();
		break;
	default:
		break;	
	}
}

void update() {
	switch (currentEffect)
	{
	case 0:
		UpdateStars();
		break;
	case 1:
		UpdateRotoZoom();
		break;
	case 2:
		UpdateDistortion();
		break;
	case 3:
		UpdateTunnel();
		break;
	case 4:
		UpdatePlasma();
		break;
	default:
		break;
	}
	UpdateMusic();
	UpdateTransitions();
}

void render() {	
	switch (currentEffect)
	{
	case 0:
		RenderStars();
		break;
	case 1:
		RenderRotoZoom();
		break;
	case 2:
		RenderDistortion();
		break;
	case 3:
		RenderTunnel();
		break;
	case 4:
		RenderPlasma();
		break;
	default:
		break;
	}	
	RenderTransitions();
}

void clear() {
	switch (currentEffect)
	{
	case 0:
		ClearStars();
		break;
	case 1:
		ClearRotoZoom();
		break;
	case 2:
		ClearDistortion();
		break;
	case 3:
		ClearTunnel();
		break;
	case 4:
		ClearPlasma();
		break;
	default:
		break;
	}
}

void CheckEndOfEffect() {
	if (currentEffectPlayTime > MAX_EFFECT_TIME) {
		currentEffectPlayTime = 0;
		clear();
		currentEffect++;
		if (currentEffect > 4) currentEffect = 0;
		init();
	}
	else if (currentEffectPlayTime >= MAX_EFFECT_TIME - 2800 && !playingTransition)
	{
		playingTransition = true;
		currentTransition++;
		if (currentTransition > 1) currentTransition = 0;
		InitTransitions();
	}
}

void close() {

	//Clean Audio Mixer
	Mix_HaltMusic();
	Mix_FreeMusic(mySong);
	Mix_Quit();
	Mix_CloseAudio();

	//Clear the current effect
	clear();

	SDL_FreeSurface(surface);
	//Destroy window
	SDL_DestroyWindow(window);
	//Quit SDL subsystems
	SDL_Quit();
}

void waitTime() {
	currentTime = SDL_GetTicks();	
	deltaTime = currentTime - lastTime;
	currentEffectPlayTime += deltaTime;
	if (deltaTime < (int)msFrame) {
		SDL_Delay((int)msFrame - deltaTime);
	}
	lastTime = currentTime;
}

#pragma region Stars

void InitStars() {

	stars = new Star[MAXSTARS];

	for (int i = 0; i < MAXSTARS; i++)
	{		
		stars[i].position.x = (float)(rand() % SCREEN_WIDTH);
		stars[i].position.y = (float)(rand() % SCREEN_HEIGHT);
		stars[i].plane = rand() % 3;     // star colour between 0 and 2
		stars[i].direction = rand() % 3 + (-1);			
		for (int j = 0; j < 8; j++) {
			stars[i].tail[j].x = stars[i].position.x;
			stars[i].tail[j].y = stars[i].position.y - j;
		}
	}
}

void UpdateStars() {
	// update all stars
	for (int i = 0; i < MAXSTARS; i++)
	{
		// move this star depending the direction, determine how fast depending on which
		// plane it belongs to
		stars[i].position.y += (deltaTime + (float)stars[i].plane) * 0.15f;
		stars[i].position.x += (stars[i].direction);
		// check if it's gone out of the right of the screen
		if (stars[i].position.y > SCREEN_HEIGHT || stars[i].position.x < 0 || stars[i].position.x > SCREEN_WIDTH)
		{
			// if so, make it return to the left
			stars[i].position.x = (float)(rand() % SCREEN_WIDTH);
			// and randomly change the y position
			stars[i].position.y = -(float)(rand() % SCREEN_HEIGHT);
		}

		stars[i].tail[0].x = stars[i - 1].position.x;
		stars[i].tail[0].y = stars[i - 1].position.y;

		for (int j = 1; j < 8; j++) {
			stars[i].tail[j].x = stars[i-1].position.x;
			stars[i].tail[j].y = stars[i-1].position.y;
		}
	}
}

void RenderStars() {

	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));
	// update all stars
	for (int i = 0; i < MAXSTARS; i++)
	{
		// draw this star, with a colour depending on the plane
		unsigned int color = 0;
		switch (stars[i].plane) {
		case 0:
			color = 0xFF606060; // dark grey
			break;
		case 1:
			color = 0xFFC2C2C2; // light grey
			break;
		case 2:
			color = 0xFFFFFFFF; // white
			break;
		}
		WritePixel(screenSurface, (int)stars[i].position.x, (int)stars[i].position.y, color);
		for (int j = 0; j < 8; j++) {
			WritePixel(screenSurface, (int)stars[i].tail[j].x, (int)stars[i].tail[j].y, color);
		}
		
	}
}

void WritePixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	// Check if whe are out of bounds
	if ((x < 0) || (x >= SCREEN_WIDTH) || (y < 0) || (y >= SCREEN_HEIGHT))
		return;

	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;	
	*(Uint32*)p = pixel;
}

void ClearStars() {
	free(stars);
}
#pragma endregion

#pragma region RotoZoom
void InitRotoZoom() {
	SDL_Surface* temp = IMG_Load("mushroom.png");
	if (temp == NULL) {
		std::cout << "Image can be loaded! " << IMG_GetError();
		close();
		exit(1);
	}
	surface = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);
}

void UpdateRotoZoom() {
	float radius = surface->h + surface->w * cos((float)currentTime / 1000.0f); // This defines de distance
	float angle = -(float)((float)(currentTime) / 1000); // This defines the direction of the rotoZoom
	Ax = (int)((surface->h * 300) * (radius * cos(angle))),
	Ay = (int)((surface->w * 300) * (radius * sin(angle))),
	Bx = (int)((surface->h * 300) * (radius * cos(angle + surface->h))),
	By = (int)((surface->w * 300) * (radius * sin(angle + surface->h))),
	Cx = (int)((surface->h * 300) * (radius * cos(angle - (float)(surface->h)))),
	Cy = (int)((surface->w * 300) * (radius * sin(angle - (float)(surface->h))));
}

void RenderRotoZoom() {
	// setup the offsets in the buffers
	Uint8* dst;
	Uint8* initbuffer = (Uint8*)screenSurface->pixels;
	int bpp = screenSurface->format->BytesPerPixel;
	Uint8* imagebuffer = (Uint8*)surface->pixels;
	int bppImage = surface->format->BytesPerPixel;
	// compute deltas
	int dxdx = (Bx - Ax) / SCREEN_WIDTH,
		dydx = (By - Ay) / SCREEN_WIDTH,
		dxdy = (Cx - Ax) / SCREEN_HEIGHT,
		dydy = (Cy - Ay) / SCREEN_HEIGHT;

	SDL_LockSurface(screenSurface);
	// loop for all lines
	for (int j = 0; j < SCREEN_HEIGHT; j++)
	{
		dst = initbuffer + j * screenSurface->pitch;
		Cx = Ax;
		Cy = Ay;
		// for each pixel
		for (int i = 0; i < SCREEN_WIDTH; i++)
		{
			// get texel and store pixel
			Uint8* p = (Uint8*)imagebuffer + ((Cy >> 16) & 0xff) * surface->pitch + ((Cx >> 16) & 0xFF) * bppImage;
			// copy it to the screen
			*(Uint32*)dst = *(Uint32*)p;
			// interpolate to get next texel in texture space
			Cx += dxdx;
			Cy += dydx;
			// next pixel
			dst += bpp;
		}
		// interpolate to get start of next line in texture space
		Ax += dxdy;
		Ay += dydy;
	}
	SDL_UnlockSurface(screenSurface);
}

void ClearRotoZoom() {
	// There is nothing to do here
	IMG_Quit();
}
#pragma endregion

#pragma region Distortion

void InitDistortion() {
	// Load Image
	SDL_Surface* temp = IMG_Load("star.png");
	if (temp == NULL) {
		std::cout << "Image can be loaded! " << IMG_GetError();
		close();
		exit(1);
	}	
	surface = SDL_ConvertSurfaceFormat(temp, SDL_PIXELFORMAT_ARGB8888, 0);

	// Alloc buffers
	dispX = (char*)malloc(SCREEN_HEIGHT * SCREEN_WIDTH * 4);
	dispY = (char*)malloc(SCREEN_HEIGHT * SCREEN_WIDTH * 4);
	int dst = 0;
	// Prepare buffers
	for (int i = 0; i < SCREEN_HEIGHT * 2; i++) {
		for (int j = 0; j < SCREEN_WIDTH * 2; j++) {
			float x = (float)i;
			float y = (float)j;
			dispX[dst] = (signed char)(8 * (2 * sin(x / FPS / 2) + sin(x * y / FPS) * cos(x * y)));
			dispY[dst] = (signed char)(8 * (cos(x / FPS / 4) + cos(x * y / FPS / 2) * sin(x * y)));
			dst++;
		}
	}

}

void UpdateDistortion() {
	// move distortion buffer
	windowx1 = (SCREEN_WIDTH / 2) + (int)(((SCREEN_WIDTH / 2)) * cos((double)currentTime / screenSurface->h));
	windowx2 = (SCREEN_WIDTH / 2) + (int)(((SCREEN_WIDTH / 2)) * sin((double)-currentTime / screenSurface->h));
	windowy1 = (SCREEN_HEIGHT / 2) + (int)(((SCREEN_HEIGHT / 2)) * sin((double)currentTime / screenSurface->w));
	windowy2 = (SCREEN_HEIGHT / 2) + (int)(((SCREEN_HEIGHT / 2)) * cos((double)-currentTime / screenSurface->w));
}

void RenderDistortion() {
	// setup the offsets in the buffers
	Uint8* dst;
	int	src1 = windowy1 * (SCREEN_WIDTH * 2) + windowx1,
		src2 = windowy2 * (SCREEN_WIDTH * 2) + windowx2;
	int dX, dY;
	Uint8* initbuffer = (Uint8*)screenSurface->pixels;
	int bpp = screenSurface->format->BytesPerPixel;
	Uint8* imagebuffer = (Uint8*)surface->pixels;
	int bppImage = surface->format->BytesPerPixel;

	SDL_LockSurface(screenSurface);
	// loop for all lines
	for (int j = 0; j < (SCREEN_HEIGHT); j++)
	{
		dst = initbuffer + j * screenSurface->pitch;
		// for all pixels
		for (int i = 0; i < SCREEN_WIDTH; i++)
		{
			// get distorted coordinates, use the integer part of the distortion
			// buffers and truncate to closest texel
			dY = j + (dispY[src1] >> 3);
			dX = i + (dispX[src2] >> 3);
			// check the texel is valid
			if ((dY >= 0) && (dY < (SCREEN_HEIGHT - 1)) && (dX >= 0) && (dX < (SCREEN_WIDTH - 1)))
			{
				// copy it to the screen
				Uint8* p = (Uint8*)imagebuffer + dY * surface->pitch + dX * bppImage;
				*(Uint32*)dst = *(Uint32*)p;
			}
			// otherwise, just set it to black
			else *(Uint32*)dst = 0;
			// next pixel
			dst += bpp;
			src1++; src2++;
		}
		// next line
		src1 += SCREEN_WIDTH;
		src2 += SCREEN_WIDTH;
	}
	SDL_UnlockSurface(screenSurface);
}

void ClearDistortion() {
	IMG_Quit();
	free(dispX);
	free(dispY);
}
#pragma endregion

#pragma region Tunnel
void InitTunnel() {
	
	tunnelBuffer = (unsigned char*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 2);
	long counter = 0;
	// precalculate
	for (int j = -(SCREEN_HEIGHT / 2); j < (SCREEN_HEIGHT / 2); j++) {
		for (int i = -(SCREEN_WIDTH / 2); i < (SCREEN_WIDTH / 2); i++)
		{
			float dx = (float)i / SCREEN_HEIGHT;
			float dy = (float)j / SCREEN_HEIGHT;
			float dz = 1;

			float d = 40 / sqrt(dx * dx + dy * dy);
			dx *= d;
			dy *= d;
			dz *= d;

			dx -= -1 * sin(dz * M_PI / screenSurface->w);
			dy -= -1 * sin(dz * M_PI / screenSurface->w);
			float ang = atan2(dy, dx) * screenSurface->w / M_PI;
			unsigned char anglePosition = (unsigned char)ang;
			unsigned char distancePosition = (unsigned char)dz;

			tunnelBuffer[counter] = anglePosition;
			tunnelBuffer[counter + 1] = distancePosition;
			counter += 2;
		}
	}

	// load the texture
	surface = IMG_Load("wall.png");
}

void UpdateTunnel() {
	// There is nothing to do here
}

void RenderTunnel() {
	Uint8* dst;
	int counter = 0;
	SDL_LockSurface(screenSurface);
	// Start pointer offset
	dst = (Uint8*)screenSurface->pixels;
	for (int j = 0; j < SCREEN_HEIGHT; j++) {		
		for (int i = 0; i < SCREEN_WIDTH; i++) {
			// Add displacement to the points
			unsigned char angle = tunnelBuffer[counter] + currentTime / (FPS / 2);
			unsigned char distance = tunnelBuffer[counter + 1] + currentTime / (FPS / 2);

			SDL_Color Color;
			SDL_GetRGB(*(Uint32*)((Uint8*)surface->pixels + distance * surface->pitch + angle * surface->format->BytesPerPixel), surface->format, &Color.r, &Color.g, &Color.b);
			Uint32 resultColor = SDL_MapRGB(screenSurface->format, Color.r, Color.g, Color.b);
			*(Uint32*)dst = resultColor;
			dst += screenSurface->format->BytesPerPixel;

			counter += 2;
		}

		dst = (Uint8*)screenSurface->pixels + j * screenSurface->pitch;
	}
	SDL_UnlockSurface(screenSurface);
}

void ClearTunnel() {
	free(tunnelBuffer);
	IMG_Quit();
}
#pragma endregion

#pragma region Plasma

void InitPlasma() {

	plasma1 = (unsigned char*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4);
	plasma2 = (unsigned char*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4);
	int dst = 0;

	for (int j = 0; j < SCREEN_HEIGHT * 2; j++)
	{
		for (int i = 0; i < SCREEN_WIDTH * 2; i++)
		{
			plasma1[dst] = (unsigned char)(64 + 63 * sin(hypot(SCREEN_HEIGHT - (float)j, SCREEN_WIDTH - (float)i) / 16));
			plasma2[dst] = (unsigned char)(64 + 63 * sin((float)i / (37 + 15 * cos((float)j / 74))) * cos((float)j / (31 + 11 * sin((float)i / 57))));
			dst++;
		}
	}
}

void UpdatePlasma() {
	for (int i = 0; i < 256; i++) {		
		palette[i] = {
			(unsigned char)(256 * sin(i * M_PI / 128 + currentTime/500)),
			(unsigned char)(256 * cos(i * M_PI / 128 + currentTime/800)),
			(unsigned char)(256 * sin(i * M_PI / 128 + currentTime/1000))
		};
	}

	int ran = rand();
	src1 = ran % 256 * currentTime / 20000;
	src2 = ran % 256 * currentTime / 20000;

}

void RenderPlasma() {
	Uint8* dst;
	long i, j;
	Uint8* initbuffer = (Uint8*)screenSurface->pixels;
	int bpp = screenSurface->format->BytesPerPixel;

	SDL_LockSurface(screenSurface);

	dst = initbuffer;
	for (j = 0; j < SCREEN_HEIGHT; j++)
	{
		dst = initbuffer + j * screenSurface->pitch;
		for (i = 0; i < SCREEN_WIDTH; i++)
		{
			// plot the pixel as a sum of all our plasma functions
			unsigned int Color = 0;
			int indexColor = (plasma1[src1] + plasma2[src2]) % 256;
			Color = 0xFF000000 + (palette[indexColor].r << 16) + (palette[indexColor].g << 8) + palette[indexColor].b;
			*(Uint32*)dst = Color;

			dst += bpp;
			src1++; src2++;
		}
		// get the next line in the precalculated buffers
		src1 += SCREEN_WIDTH; src2 += SCREEN_WIDTH;
	}
	SDL_UnlockSurface(screenSurface);
}

void ClearPlasma() {
	free(plasma1);
	free(plasma2);
}
#pragma endregion

#pragma region Mixer
void InitMusic() {
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Init(MIX_INIT_OGG);
	mySong = Mix_LoadMUS("Interplanetary_Odyssey.ogg");
	if (!mySong) {
		std::cout << "Error loading Music: " << Mix_GetError() << std::endl;
		close();
		exit(1);
	}
	Mix_PlayMusic(mySong, 0);
	flashtime = 0;
	MusicCurrentTime = 0;
	MusicCurrentTimeBeat = 0;
	MusicCurrentBeat = 0;
	MusicPreviousBeat = -1;
}

void UpdateMusic() {
	MusicCurrentTime += deltaTime;
	MusicCurrentTimeBeat += deltaTime;
	MusicPreviousBeat = MusicCurrentBeat;
	if (MusicCurrentTimeBeat >= MSEG_BPM) {
		MusicCurrentTimeBeat = 0;
		MusicCurrentBeat++;
		flashtime = FLASH_MAX_TIME;
	}
	if (flashtime > 0) {
		flashtime -= deltaTime;
	}
	else {
		flashtime = 0;
	}
	if (!Mix_PlayingMusic()) {
		clear();
		close();
		exit(0);
	}
}
#pragma endregion


#pragma region DoorsTransition

void InitDoorsTransition() {
	transitionSurface = SDL_CreateRGBSurface(
		0,
		screenSurface->w,
		screenSurface->h,
		screenSurface->format->BitsPerPixel,
		screenSurface->format->Rmask,
		screenSurface->format->Gmask,
		screenSurface->format->Bmask,
		0xFF000000);
	SDL_SetSurfaceBlendMode(transitionSurface, SDL_BLENDMODE_BLEND);

	for (int i = 0; i < 2; i++) {
		rects[i].w = 0;
		rects[i].h = SCREEN_HEIGHT;
		rects[i].y = 0;
	}
	
	rects[0].x = 0;	
	rects[1].x = SCREEN_WIDTH;
	
}

void UpdateDoorsTransition() {
	if (isOpening) {
		if (rects[0].w - 1 > 0) {
			rects[0].w--;
		}
		else
		{
			isOpening = false;
			playingTransition = false;
		}

		if (rects[1].x + 1 < SCREEN_WIDTH) {
			rects[1].w--;
			rects[1].x++;
		}
	}
	else
	{
		if (rects[0].w + 1  <= SCREEN_WIDTH / 2) rects[0].w++;

		if (rects[1].x - 1 >= SCREEN_WIDTH / 2) {
			rects[1].w++;
			rects[1].x--;
		}
		else
		{
			isOpening = true;
		}
	}	
	
}

void RenderDoorsTransition() {
	SDL_FillRect(transitionSurface, NULL, 0);
	Uint32 color = 0x000000 | 255 << 24;
	for (int i = 0; i < 2; i++) {				
		// ALPHA: This draw the rectangle in layer1
		SDL_FillRect(transitionSurface, &rects[i], color);
		// ALPHA: After that I copy this layer over windowsurface. NULL means copy ALL surface from source surface to taget surface
		SDL_BlitSurface(transitionSurface, NULL, screenSurface, NULL);
	}
}

void ClearDoorsTransition() {

}
#pragma endregion

#pragma region FadeTransition

void InitFadeTransition() {
	transitionSurface = SDL_CreateRGBSurface(
		0,
		screenSurface->w,
		screenSurface->h,
		screenSurface->format->BitsPerPixel,
		screenSurface->format->Rmask,
		screenSurface->format->Gmask,
		screenSurface->format->Bmask,
		0xFF000000);
	SDL_SetSurfaceBlendMode(transitionSurface, SDL_BLENDMODE_BLEND);
}

void UpdateFadeTransition() {
	if (isObscuring) {
		if (currentAlpha + 1 < 256) {
			currentAlpha++;
		}
		else
		{
			isObscuring = false;
		}
	}
	else
	{
		if (currentAlpha - 1 > 0) {
			currentAlpha--;			
		}
		else
		{
			playingTransition = false;
			isObscuring = true;
		}
	}
}

void RenderFadeTransition() {
	SDL_FillRect(transitionSurface, NULL, SDL_MapRGB(transitionSurface->format, 0, 0, 0));
	SDL_SetSurfaceAlphaMod(transitionSurface, currentAlpha);
	SDL_BlitSurface(transitionSurface, NULL, screenSurface, NULL);
}

void ClearFadeTransition() {

}
#pragma endregion


#pragma region Transitions

void InitTransitions() {
	switch (currentTransition)
	{
	case 0:
		InitDoorsTransition();
		break;
	case 1:
		InitFadeTransition();
		break;
	default:
		break;
	}
}

void UpdateTransitions() {	

	if (playingTransition) {
		switch (currentTransition)
		{
		case 0:
			UpdateDoorsTransition();
			break;
		case 1:
			UpdateFadeTransition();
			break;
		default:
			break;
		}
	}	
}

void RenderTransitions() {
	if (playingTransition) {
		switch (currentTransition)
		{
		case 0:
			RenderDoorsTransition();
			break;
		case 1:
			RenderFadeTransition();
			break;
		default:
			break;
		}
	}	
}

void ClearTransitions() {
	switch (currentTransition)
	{
	case 0:
		ClearDoorsTransition();
		break;
	case 1:
		ClearFadeTransition();
		break;
	default:
		break;
	}
}
#pragma endregion
