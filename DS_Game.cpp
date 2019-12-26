#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <sstream>

#define PI 3.14159265


// Dimensions for window
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 640;

//Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 8000;

// Class for textures
class LTexture
{
	public:
		// Initialization
		LTexture();

		// Deallocation
		~LTexture();

		// Image loading
		bool loadFromFile (std::string path);

        // ifdef in case SDL_TTF isn't installed
		#ifdef _SDL_TTF_H
		// Font string -> image
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor, TTF_Font *Font );
		#endif

		// Texture deallocation
		void free();

		// Sets color modulation
		void setColorMod(Uint8 red, Uint8 green, Uint8 blue);

		// Sets blending
		void setBlendMode(SDL_BlendMode blending);

		// Sets alpha modulation
		void setAlphaMod(Uint8 alpha);

		// Renders texture at point
		void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

		// Gets image dimensions (mWidth, mHeight)
		int getWidth();
		int getHeight();

	private:
		// The actual texture
		SDL_Texture* mTexture;

		// Image dimensions
		int mWidth;
		int mHeight;
};

// Initializes SDL
bool init();

// Loads media
bool loadMedia();

// Frees media and shuts down SDL
void close();

// Creates window object
SDL_Window *gWindow = NULL;

// Creates window renderer
SDL_Renderer *gRenderer = NULL;

//Game Controller 1 handler
SDL_Joystick* gGameController = NULL;

TTF_Font *titleFont = NULL;
TTF_Font *pressStartFont = NULL;

// Creates scene textures
LTexture gBackgroundTexture;
LTexture gFighterSprite;
LTexture gTurretSprite;
LTexture gBulletSprite;
LTexture gABulletSprite;
LTexture gHealthSprite;
SDL_Rect gHealthClip;
LTexture gAmmoSprite;
LTexture gRaiderSprite;
LTexture gRaiderDam1Sprite;
LTexture gRaiderDam2Sprite;
LTexture gRaiderDam3Sprite;
LTexture gStrikerSprite;
LTexture gStrikerDam1Sprite;
LTexture gStrikerDam2Sprite;
LTexture gStrikerDam3Sprite;
LTexture gThrasherSprite;
LTexture gThrasherDam1Sprite;
LTexture gThrasherDam2Sprite;
LTexture gThrasherDam3Sprite;
LTexture gWinnerSprite;
LTexture gSpeedSprite;
LTexture gDamageSprite;
LTexture gTextTextureStar;
LTexture gTextTextureCollider;
LTexture gTextTextureLevel1;
LTexture gTextTextureLevel2;
LTexture gTextTextureLevel3;
LTexture gTextTextureGame;
LTexture gTextTextureOver;
LTexture gPressStartTexture;

//The music that will be played
Mix_Music *gMusic = NULL;
Mix_Music *gIdleMusic = NULL;

LTexture::LTexture()
{
	// Initialize texture stuff
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	// Deallocate texture stuff
	free();
}

bool LTexture::loadFromFile (std::string path)
{
	// Deallocate preexisting texture
	free();

	// The usable texture
	SDL_Texture *newTexture = NULL;

	// Load image from path
	SDL_Surface *loadedSurface = IMG_Load(path.c_str());
	if (!loadedSurface) {
        // Spits out specific error if something goes wrong
		printf("Unable to load image %s. SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	} else {
		// Color keys the image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		// Creates texture from surface
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (!newTexture) {
            // Spits out specific error if something goes wrong
			printf("Unable to create texture from %s. SDL Error: %s\n", path.c_str(), SDL_GetError());
		} else {
			// Gets image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		// Gets rid of the old surface
		SDL_FreeSurface(loadedSurface);
	}

	// Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText (std::string textureText, SDL_Color textColor, TTF_Font *Font)
{
	// Gets rid of the preexisting texture
	free();

	// Renders text surface
	SDL_Surface *textSurface = TTF_RenderText_Solid(Font, textureText.c_str(), textColor);
	if (!textSurface) {
        printf("Unable to render text surface. SDL_ttf Error: %s\n", TTF_GetError());
	} else {
		// Creates texture from surface
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (!mTexture) {
            // Spits out specific error if something goes wrong
			printf("Unable to create texture from rendered text. SDL Error: %s\n", SDL_GetError());
		} else {
			// Gets image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		// Gets rid of old surface
		SDL_FreeSurface( textSurface );
	}

	// Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	// If texture exists, free it
	if (mTexture) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColorMod (Uint8 red, Uint8 green, Uint8 blue)
{
	// Modulates texture RGB
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode (SDL_BlendMode blending)
{
	// Sets blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlphaMod (Uint8 alpha)
{
	// Modulates texture alpha (transparency)
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render (int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	// Set rendering space in window
	SDL_Rect renderQuad = {x, y, mWidth, mHeight};

	// Sets clip rendering dimensions
	if (clip) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	// Renders to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth() { return mWidth; }

int LTexture::getHeight() { return mHeight; }


bool init()
{
    // Initialization success flag
	bool success = true;

	// Initializes SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0) {
		printf("SDL could not initialize. SDL Error: %s\n", SDL_GetError());
		success = false;
	} else {
		// Sets texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled\n");
		}

		// Check for joysticks
		if (SDL_NumJoysticks() < 1) {
			printf("Warning: No joysticks connected.\n");
		} else {
			// Load joystick
			gGameController = SDL_JoystickOpen(0);
			if (!gGameController) {
				printf("Warning: Unable to open game controller. SDL Error: %s\n", SDL_GetError());
			}
		}

		// Creates window
		gWindow = SDL_CreateWindow("Star Collider", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (!gWindow) {
			printf("Window could not be created. SDL Error: %s\n", SDL_GetError());
			success = false;
		} else {
		    SDL_Surface *icon = IMG_Load("DS_Game/icon.png");
		    SDL_SetWindowIcon(gWindow, icon);
		    SDL_FreeSurface(icon);
			// Creates renderer for window (vsync so frame rate cooperates)
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (!gRenderer) {
				printf("Renderer could not be created. SDL Error: %s\n", SDL_GetError());
				success = false;
			} else {
				// Initializes renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				// Initializes PNG image loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_image could not initialize. SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

                // Initialize SDL_ttf
                if (TTF_Init() == -1) {
                    printf("SDL_ttf could not initialize. SDL_ttf Error: %s\n", TTF_GetError());
                    success = false;
                }

                // Initialize SDL_mixer
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
                    printf("SDL_mixer could not initialize. SDL_mixer Error: %s\n", Mix_GetError());
                    success = false;
                }
			}
		}
	}

	return success;
}

bool loadMedia()
{
	// Loading success flag
	bool success = true;

	titleFont = TTF_OpenFont("DS_Game/titlefont.ttf", 72);
	pressStartFont = TTF_OpenFont("DS_Game/titlefont.ttf", 24);
    if (titleFont == NULL) {
        printf("Failed to load lazy font. SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    } else {
        //Render text
        SDL_Color textColor = {255, 255, 255};
        if (!gTextTextureStar.loadFromRenderedText("Star", textColor, titleFont)) {
            printf("Failed to render text texture.\n");
            success = false;
        }
        if (!gTextTextureCollider.loadFromRenderedText("Collider", textColor, titleFont)) {
            printf("Failed to render text texture.\n");
            success = false;
        }
        if (!gTextTextureLevel1.loadFromRenderedText("level 1", textColor, titleFont)) {
            printf("Failed to render text texture.\n");
            success = false;
        }
        if (!gTextTextureLevel2.loadFromRenderedText("level 2", textColor, titleFont)) {
            printf("Failed to render text texture.\n");
            success = false;
        }
        if (!gTextTextureLevel3.loadFromRenderedText("level 3", textColor, titleFont)) {
            printf("Failed to render text texture.\n");
            success = false;
        }
        if (!gTextTextureGame.loadFromRenderedText("Game", textColor, titleFont)) {
            printf("Failed to render text texture.\n");
            success = false;
        }
        if (!gTextTextureOver.loadFromRenderedText("Over", textColor,titleFont)) {
            printf("Failed to render text texture.\n");
            success = false;
        }
    }

	// Load textures
	if (!gBackgroundTexture.loadFromFile("DS_Game/backgroundtxtr.png")) {
        printf("Failed to load backgroundtxtr.png.\n");
        success = false;
	}
    if (!gFighterSprite.loadFromFile("DS_Game/fighterspr.png")) {
        printf("Failed to load fighterspr.png texture.\n");
        success = false;
    }
    if (!gTurretSprite.loadFromFile("DS_Game/turretspr.png")) {
        printf("Failed to load turretspr.png texture.\n");
        success = false;
    }
    if (!gBulletSprite.loadFromFile("DS_Game/bulletspr.png")) {
        printf("Failed to load bulletspr.png.\n");
        success = false;
    }
    if (!gABulletSprite.loadFromFile("DS_Game/abulletspr.png")) {
        printf("Failed to loadabulletspr.png.\n");
        success = false;
    }
    if (!gHealthSprite.loadFromFile("DS_Game/healthspr.png")) {
        printf("Failed to load healthspr.png.\n");
        success = false;
    } else {
        gHealthClip.x = 0;
        gHealthClip.y = 0;
        gHealthClip.w = gHealthSprite.getWidth();
        gHealthClip.h = gHealthSprite.getHeight();
    }
    if (!gAmmoSprite.loadFromFile("DS_Game/ammospr.png")) {
        printf("Failed to load ammospr.png.\n");
        success = false;
    }
    if (!gRaiderSprite.loadFromFile("DS_Game/raiderspr.png")) {
        printf("Failed to load raiderspr.png.\n");
        success = false;
    }
    if (!gRaiderDam1Sprite.loadFromFile("DS_Game/raidersprdam1.png")) {
        printf("Failed to load raiderspr.png.\n");
        success = false;
    }
    if (!gRaiderDam2Sprite.loadFromFile("DS_Game/raidersprdam2.png")) {
        printf("Failed to load raiderspr.png.\n");
        success = false;
    }
    if (!gRaiderDam3Sprite.loadFromFile("DS_Game/raidersprdam3.png")) {
        printf("Failed to load raiderspr.png.\n");
        success = false;
    }
    if (!gStrikerSprite.loadFromFile("DS_Game/strikerspr.png")) {
        printf("Failed to load strikerspr.png.\n");
        success = false;
    }
    if (!gStrikerDam1Sprite.loadFromFile("DS_Game/strikersprdam1.png")) {
        printf("Failed to load strikerspr.png.\n");
        success = false;
    }
    if (!gStrikerDam2Sprite.loadFromFile("DS_Game/strikersprdam2.png")) {
        printf("Failed to load strikerspr.png.\n");
        success = false;
    }
    if (!gStrikerDam3Sprite.loadFromFile("DS_Game/strikersprdam3.png")) {
        printf("Failed to load strikerspr.png.\n");
        success = false;
    }
    if (!gThrasherSprite.loadFromFile("DS_Game/thrasherspr.png")) {
        printf("Failed to load thrasherspr.png.\n");
        success = false;
    }
    if (!gThrasherDam1Sprite.loadFromFile("DS_Game/thrashersprdam1.png")) {
        printf("Failed to load thrasherspr.png.\n");
        success = false;
    }
    if (!gThrasherDam2Sprite.loadFromFile("DS_Game/thrashersprdam2.png")) {
        printf("Failed to load thrasherspr.png.\n");
        success = false;
    }
    if (!gThrasherDam3Sprite.loadFromFile("DS_Game/thrashersprdam3.png")) {
        printf("Failed to load thrasherspr.png.\n");
        success = false;
    }
    if (!gWinnerSprite.loadFromFile("DS_Game/winnerspr.png")) {
        printf("Failed to load winnerspr.png.\n");
        success = false;
    }
    if (!gSpeedSprite.loadFromFile("DS_Game/speedspr.png")) {
        printf("Failed to load speedspr.png.\n");
        success = false;
    }
    if (!gDamageSprite.loadFromFile("DS_Game/damgspr.png")) {
        printf("Failed to load damgspr.png.\n");
        success = false;
    }

    gMusic = Mix_LoadMUS("DS_Game/Chase_The_Ace.mp3");
    if (!gMusic) {
        printf("Failed to load DS_Game/Chase_The_Ace.mp3. SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
    }
    gIdleMusic = Mix_LoadMUS("DS_Game/DT.mp3");
    if (!gIdleMusic) {
        printf("Failed to load DS_Game/DT.mp3. SDL_Mixer Error: %s\n", Mix_GetError());
        success = false;
    }

	return success;
}

void close()
{
	// Free loaded images
	gTextTextureStar.free();
	gTextTextureCollider.free();
	gTextTextureGame.free();
	gTextTextureOver.free();
	gTextTextureLevel1.free();
	gTextTextureLevel2.free();
	gTextTextureLevel3.free();
	gBackgroundTexture.free();
	gFighterSprite.free();
	gTurretSprite.free();
	gBulletSprite.free();
	gABulletSprite.free();
	gHealthSprite.free();
	gAmmoSprite.free();
    gRaiderSprite.free();
    gRaiderDam1Sprite.free();
    gRaiderDam2Sprite.free();
    gRaiderDam3Sprite.free();
    gStrikerSprite.free();
    gStrikerDam1Sprite.free();
    gStrikerDam2Sprite.free();
    gStrikerDam3Sprite.free();
    gThrasherSprite.free();
    gThrasherDam1Sprite.free();
    gThrasherDam2Sprite.free();
    gThrasherDam3Sprite.free();
	gWinnerSprite.free();
	gSpeedSprite.free();
	gDamageSprite.free();
	gPressStartTexture.free();

	//Close game controller
    SDL_JoystickClose(gGameController);
    gGameController = NULL;

    //Free the music
    Mix_FreeMusic(gMusic);
    gMusic = NULL;
    Mix_FreeMusic(gIdleMusic);
    gIdleMusic = NULL;

	// Destroy renderer and window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	// Quit SDL stuff
	IMG_Quit();
	SDL_Quit();
	TTF_Quit();
	Mix_Quit();
}


struct Bullets {
    int posX;
    int posY;
    Bullets *next;
};

int addNode (Bullets **bull, int posX, int posY)
{
    Bullets *newBull = (Bullets*)malloc(sizeof(Bullets));
    if (!newBull) { return 0; }
    newBull->posX = posX;
    newBull->posY = posY;
    newBull->next = NULL;

    Bullets *curr = *bull;
    if (!curr) {
        *bull = newBull;
        return 0;
    }

    while (curr->next) { curr = curr->next; }
    curr->next = newBull;

    return 0;
}

int delNode (Bullets **bull, int posX, int posY)
{
    Bullets *curr = *bull;
    Bullets *prev = curr;
    if (!curr) { return 0; }
    if (curr->posX == posX) {
        *bull = curr->next;
        free(curr);
        return 0;
    }
    while (curr->next) {
        prev = curr;
        curr = curr->next;
        if (curr->posX == posX) {
            prev->next = curr->next;
            free(curr);
            return 0;
        }
    }
    return 0;
}

void clrList (Bullets *bull)
{
    Bullets *tmp;
    while (bull) {
        tmp = bull;
        bull = bull->next;
        free(tmp);
    }
}

class Player
{
    public:
        Player(int h, int s, int r, int d);

        int health;
        int posX;
        int posY;
        int turretY;
        bool shooting;
        int speed;
        int bullSpeed;
        int damage;
        int turretSpeed;
        bool turretsCooled ;
        int heatSpeed;
        int coolSpeed;

        int getMaxHealth();

    private:
        int maxHealth;
};

Player::Player(int h, int s, int r, int d)
{
    health = h;
    speed = s;
    bullSpeed = r;
    damage = d;
    shooting = false;
    maxHealth = health;
    posX = SCREEN_WIDTH/2 - gFighterSprite.getWidth()/2;
    posY = SCREEN_HEIGHT;
    turretY = posY+gFighterSprite.getHeight()/2;
    turretSpeed = 1;
    turretsCooled = true;
    heatSpeed = 2;
    coolSpeed = 1;
}

int Player::getMaxHealth() { return maxHealth; }

class Enemy
{
    public:
        Enemy(int h, int s, int r, int d, int t);

        int health;
        int posX;
        int posY;
        bool shooting;
        int timeSinceMove;

        int getSpeed();
        int getRate();
        int getDamage();
        int getType();
        int getMaxHealth();

    private:
        int speed;
        int fireRate;
        int damage;
        int type;
        int maxHealth;
};

Enemy::Enemy(int h, int s, int r, int d, int t)
{
    health = h;
    speed = s;
    fireRate = r;
    damage = d;
    type = t;
    if (type == 1) {
        posX = SCREEN_WIDTH/2-gRaiderSprite.getWidth()/2;
        posY = -gRaiderSprite.getHeight();
    } else if (type == 2) {
        posX = SCREEN_WIDTH/2-gStrikerSprite.getWidth()/2;
        posY = -gStrikerSprite.getHeight();
    } else if (type == 3) {
        posX = SCREEN_WIDTH/2-gThrasherSprite.getWidth()/2;
        posY = -gThrasherSprite.getHeight();
    }
    shooting = false;
    timeSinceMove = 0;
    maxHealth = health;
}

int Enemy::getSpeed() { return speed; }
int Enemy::getRate() { return fireRate; }
int Enemy::getDamage() {return damage; }
int Enemy::getType() { return type; }
int Enemy::getMaxHealth() { return maxHealth; }


int moveBackground(int by1, int by2, int bs)
{
    if (by1+bs < SCREEN_HEIGHT) {
        by1+=bs;
    } else {
        by1 = by2-gBackgroundTexture.getHeight()+bs;
    }

    return by1;
}

int initiate(int by1, int by2, int bs, int posX, int posY, int turretY)
{
    int p2StartA = 255;
    std::stringstream pressStart;
    SDL_Color textColor = { 255, 255, 255, 255 };
    double accel = 8.0;
    while (posY > SCREEN_HEIGHT*3/5 || p2StartA > 8) {
        by1 = moveBackground(by1, by2, bs);
        by2 = moveBackground(by2, by1, bs);

        posY-=accel;
        turretY-=accel;
        if (accel > 5)
            accel-=.2;

        Mix_FadeOutMusic(600);

        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(gRenderer);

        // Render background(s)
        gBackgroundTexture.render(0, by1);
        gBackgroundTexture.render(0, by2);

        gTextTextureStar.setAlphaMod(p2StartA);
        gTextTextureCollider.setAlphaMod(p2StartA);
        gTextTextureStar.render((SCREEN_WIDTH-gTextTextureStar.getWidth())/2, SCREEN_HEIGHT/2-gTextTextureStar.getHeight());
        gTextTextureCollider.render((SCREEN_WIDTH-gTextTextureCollider.getWidth())/2, SCREEN_HEIGHT/2);
        pressStart.str("Press space to start");
        gPressStartTexture.loadFromRenderedText( pressStart.str().c_str(), textColor, pressStartFont );
        gPressStartTexture.setAlphaMod(p2StartA);
        gPressStartTexture.render(SCREEN_WIDTH/2-gPressStartTexture.getWidth()/2, SCREEN_HEIGHT*2/3);
        if (p2StartA > 8) {
            p2StartA-=1.4*accel;
        } else {
            p2StartA = 8;
        }

        gTurretSprite.render(posX+gFighterSprite.getWidth()/6,turretY);
        gTurretSprite.render(posX+gFighterSprite.getWidth()*4/6,turretY,0,0,0,SDL_FLIP_HORIZONTAL);
        gFighterSprite.render(posX, posY);

        SDL_RenderPresent(gRenderer);
    }

    return by1;
}

int main (int argc, char *args[])
{
	// Initialize SDL and create window
	if(!init()) {
		printf( "Failed to initialize\n" );
	} else {
		// Load media
		if(!loadMedia()) {
			printf( "Failed to load media\n" );
		} else {
			// Flag to quit game
			bool quit = false;

			// Event handler (user input)
			SDL_Event evnt;

			//Normalized direction
            int xDir = 0;
            int yDir = 0;

			//Modulation components
			Uint8 r = 75;
			Uint8 g = 75;
			Uint8 b = 255;

			bool start = false;
			bool gameOver = false;
			bool win = false;
			bool flag = false;
			// 1 to 20
			int difficulty = 10;

			Uint8 p2StartA = 255;
			std::stringstream pressStart;
			SDL_Color textColor = { 255, 255, 255, 255 };
			Uint8 titleA = 255;

            int backgroundY[2] = {-6400+640, -6400*2+640};
			int backgroundSpeed = 10;

            Player player1 (100, 5, 10, 10);
            Bullets *bull = NULL;
			int shootTime = 0;
			int score = 0;
			double accel = 1.0;

            int coolTime = 0;
			bool AButton = false;

			int aBulletX;
			int aBulletY = -gBulletSprite.getHeight();

			int enemies[4] = {0,1,1,1};
			//            hp  spd bs  dmg type
			Enemy Raider (1000/(20/difficulty), 5, 1.6, 20, 1);
            Enemy Striker (750/(20/difficulty), 7, 1.8, 15, 2);
            Enemy Thrasher (2000/(20/difficulty), 2, 2, 40, 3);

			int stages[3] = {1,0,0};
			int loading[3] = {0,0,0};

            Uint32 gameTime = 0;
            Uint32 startTime;
            int volume = MIX_MAX_VOLUME/2;

            srand(time(NULL));

            bool spdOnScrn = false;
            int spdX = SCREEN_WIDTH/2-gSpeedSprite.getWidth()/2;
            int spdY = -gSpeedSprite.getHeight();
            bool damgOnScrn = false;
            int damgX = SCREEN_WIDTH/2-gDamageSprite.getWidth()/2;
            int damgY = -gDamageSprite.getHeight();


            Mix_PlayMusic(gIdleMusic, -1);

			// While game is running
			while(!quit) {
				// Handle events in PollEvent queue
				while(SDL_PollEvent(&evnt) != 0) {
					// User wants to quit
					if(evnt.type == SDL_QUIT) {
						quit = true;
					} else if (evnt.type == SDL_JOYAXISMOTION) {
                        // Motion on controller 0
                        if (evnt.jaxis.which == 0) {
                            // X/Y axis motion
                            if (evnt.jaxis.axis == 0) {
                                // Left/right of dead zone
                                if (evnt.jaxis.value < -JOYSTICK_DEAD_ZONE) {
                                    xDir = -1;
                                } else if (evnt.jaxis.value > JOYSTICK_DEAD_ZONE) {
                                    xDir =  1;
                                } else {
                                    xDir = 0;
                                }
                            } else if (evnt.jaxis.axis == 1) {
								// Below/above of dead zone
								if (evnt.jaxis.value < -JOYSTICK_DEAD_ZONE) {
									yDir = -1;
								} else if (evnt.jaxis.value > JOYSTICK_DEAD_ZONE) {
									yDir =  1;
								} else {
									yDir = 0;
								}
							}
						}
					} else if (evnt.type == SDL_JOYBUTTONDOWN) {
					    if (evnt.jbutton.button == 1)
                            quit = true;
					    if (start && !gameOver) {
                            if (evnt.jbutton.button == 0)
                                AButton = true;
					    } else if (!gameOver) {
                            backgroundY[0] = initiate(backgroundY[0], backgroundY[1], backgroundSpeed, player1.posX, player1.posY, player1.turretY);
                            backgroundY[1] = backgroundY[0]-6300;
                            player1.posY = SCREEN_HEIGHT*3/5;
                            player1.turretY = player1.posY+gFighterSprite.getHeight()/2;
                            start = true;
                            gameTime = SDL_GetTicks();
                            startTime = gameTime;
                            if (!gameOver)
                                Mix_PlayMusic(gMusic, 1);
					    }
                        //printf("%d\n",evnt.jbutton.button);
					} else if (evnt.type == SDL_JOYBUTTONUP) {
                        if (evnt.jbutton.button == 0)
                            AButton = false;
					}
				}

				// INPUT monster code
				const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

                if (currentKeyStates[SDL_SCANCODE_ESCAPE])
                    quit = true;
                if (start && !gameOver) {
                    if (currentKeyStates[SDL_SCANCODE_UP] || currentKeyStates[SDL_SCANCODE_W]) {
                        if (player1.posY > SCREEN_HEIGHT*3/5) {
                            player1.posY-=player1.speed;
                            player1.turretY-=player1.speed;
                        }
                    }
                    if (currentKeyStates[SDL_SCANCODE_DOWN] || currentKeyStates[SDL_SCANCODE_S]) {
                        if (player1.posY < SCREEN_HEIGHT-gFighterSprite.getHeight()) {
                            player1.posY+=player1.speed;
                            player1.turretY+=player1.speed;
                        }
                    }
                    if (currentKeyStates[SDL_SCANCODE_LEFT] || currentKeyStates[SDL_SCANCODE_A]) {
                        if (player1.posX > 0) {
                            player1.posX-=player1.speed;
                        }
                    }
                    if (currentKeyStates[SDL_SCANCODE_RIGHT] || currentKeyStates[SDL_SCANCODE_D]) {
                        if (player1.posX < SCREEN_WIDTH-gFighterSprite.getWidth()) {
                            player1.posX+=player1.speed;
                        }
                    }
                    // Activates turrets (animation for turrets)
                    if ((currentKeyStates[SDL_SCANCODE_SPACE] || AButton) && player1.turretsCooled) {
                        if (!player1.shooting && player1.turretY > player1.posY+gFighterSprite.getHeight()/3) {
                            player1.turretY-=player1.turretSpeed;
                        } else {
                            player1.turretY = player1.posY+gFighterSprite.getHeight()/3;
                            player1.shooting = true;
                        }
                    } else {
                        player1.shooting = false;
                        if (player1.turretY < player1.posY+gFighterSprite.getHeight()/2) {
                            player1.turretY+=player1.turretSpeed;
                        } else {
                            player1.turretY = player1.posY+gFighterSprite.getHeight()/2;
                        }
                    }
                    if  (currentKeyStates[SDL_SCANCODE_EQUALS] && volume < 128)
                        volume++;
                    if (currentKeyStates[SDL_SCANCODE_MINUS] && volume > 0)
                        volume--;
                    /*// DEVTOOL
                    if (currentKeyStates[SDL_SCANCODE_H] && !currentKeyStates[SDL_SCANCODE_LSHIFT] && player1.health > 0) {
                        player1.health--;
                    } else if (currentKeyStates[SDL_SCANCODE_H] && currentKeyStates[SDL_SCANCODE_LSHIFT] && player1.health < 100) {
                        player1.health++;
                    }*/
				} else {
                    if (currentKeyStates[SDL_SCANCODE_SPACE] && !start) {
                        backgroundY[0] = initiate(backgroundY[0], backgroundY[1], backgroundSpeed, player1.posX, player1.posY, player1.turretY);
                        backgroundY[1] = backgroundY[0]-6300;
                        player1.posY = SCREEN_HEIGHT*3/5;
                        player1.turretY = player1.posY+gFighterSprite.getHeight()/2;
                        start = true;
                        gameTime = SDL_GetTicks();
                        startTime = gameTime;
                        if (!gameOver)
                            Mix_PlayMusic(gMusic, 1);
                    }
				}

				//Calculate angle
                double joystickAngle = atan2( (double)yDir, (double)xDir ) * ( 180.0 / M_PI );
                //Correct angle
                if (xDir == 0 && yDir == 0) {
                    joystickAngle = 0;
                }
                if (start && !gameOver) {
                    if (xDir == 1 && yDir == 0 && player1.posX+gFighterSprite.getWidth()+player1.speed < SCREEN_WIDTH) {
                        player1.posX+=player1.speed;
                    } else if (xDir == 1 && yDir == 1) {
                        if (player1.posX+gFighterSprite.getWidth()+player1.speed < SCREEN_WIDTH) { player1.posX+=player1.speed; }
                        if (player1.posY+gFighterSprite.getHeight()+player1.speed < SCREEN_HEIGHT) { player1.posY+=player1.speed; player1.turretY+=player1.speed; }
                    } else if (xDir == 0 && yDir == 1 && player1.posY+gFighterSprite.getHeight()+player1.speed < SCREEN_HEIGHT) {
                        player1.posY+=player1.speed;
                        player1.turretY+=player1.speed;
                    } else if (xDir == -1 && yDir == 1) {
                        if (player1.posY+gFighterSprite.getHeight()+player1.speed < SCREEN_HEIGHT) { player1.posY+=player1.speed; player1.turretY+=player1.speed; }
                        if (player1.posX-player1.speed > 0) { player1.posX-=player1.speed; }
                    } else if (xDir == -1 && yDir == 0 && player1.posX-player1.speed > 0) {
                        player1.posX-=player1.speed;
                    } else if (xDir == -1 && yDir == -1) {
                        if (player1.posX-player1.speed > 0) { player1.posX-=player1.speed; }
                        if (player1.posY-player1.speed > SCREEN_HEIGHT*3/5) { player1.posY-=player1.speed; player1.turretY-=player1.speed; }
                    } else if (xDir == 0 && yDir == -1 && player1.posY-player1.speed > SCREEN_HEIGHT*3/5) {
                        player1.posY-=player1.speed;
                        player1.turretY-=player1.speed;
                    } else if (xDir == 1 && yDir == -1) {
                        if (player1.posY-player1.speed > SCREEN_HEIGHT*3/5) { player1.posY-=player1.speed; player1.turretY-=player1.speed; }
                        if (player1.posX+gFighterSprite.getWidth()+player1.speed < SCREEN_WIDTH) { player1.posX+=player1.speed; }
                    }
                }

                // LOGIC monster code
                // Move background (alternates between two images to creates seamless scrolling effect)
                backgroundY[0] = moveBackground(backgroundY[0], backgroundY[1], backgroundSpeed);
                backgroundY[1] = moveBackground(backgroundY[1], backgroundY[0], backgroundSpeed);

                if (player1.shooting && player1.turretsCooled) {
                    if (shootTime % 10 == 0 && shootTime % 20 != 0) {
                        addNode(&bull, player1.posX+gFighterSprite.getWidth()/6+gBulletSprite.getWidth()*3/2, player1.turretY);
                    } else if (shootTime % 20 == 0) {
                        addNode(&bull, player1.posX+gFighterSprite.getWidth()*4/6+gBulletSprite.getWidth(), player1.turretY);
                    }
                    shootTime++;
                } else {
                    for (Bullets *bulle = bull; bulle != NULL; bulle = bulle->next) {
                        if (bulle->posY < 0) {
                            delNode(&bull, bulle->posX, bulle->posY);
                        }
                    }
                    shootTime = 0;
                }

                if (shootTime > 100 && shootTime % 2 == 0) {
                    if (r < 255-player1.heatSpeed) { r+=player1.heatSpeed; }
                    if (b > 1+player1.heatSpeed) { b-=player1.heatSpeed; }
                } else if (shootTime == 0) {
                    if (r > 75+player1.coolSpeed && coolTime % 3 == 0) { r-=player1.coolSpeed; }
                    if (b < 255-player1.coolSpeed && coolTime % 3 == 0) { b+=player1.coolSpeed; }
                    coolTime+=player1.coolSpeed;
                }
                if (r > 240 && b < 10) {
                    player1.turretsCooled = false;
                }
                if (r < 1) { r = 1; } else if (r > 255) { r = 255; }
                if (b < 1) { b = 1; } else if (b > 255) { b = 255; }
                if (r < 85 && b > 240) {
                    player1.turretsCooled = true;
                    coolTime = 0;
                }
                //printf("%d-%d-%d\n",r,g,b); // DEVTOOL

                for (Bullets *bulle = bull; bulle != NULL; bulle = bulle->next) {
                    bulle->posY-=player1.bullSpeed;
                }

                gHealthClip.y = 100-player1.health;
                gHealthClip.h = player1.health;

                // Stages --------------------------------------------------------------------------------------------------------------------------------
                if (stages[0] && start && SDL_GetTicks() < gameTime+2000 && !gameOver) { loading[0] = true; } else { loading[0] = false; }
                if (stages[1] && start && SDL_GetTicks() < gameTime+1500 && !gameOver) { loading[0] = false; loading[1] = true; } else { loading[1] = false; }
                if (stages[2] && start && SDL_GetTicks() < gameTime+1500 && !gameOver) { loading[1] = false; loading[2] = true; } else { loading[2] = false; }

                if (stages[0] == -1) {
                } else if (stages[0] && start && SDL_GetTicks() > gameTime+2000 && !gameOver) {
                    // Bring alien into frame
                    if (Raider.posY < 0)
                        Raider.posY+=Raider.getSpeed();
                    // Alien moves to player
                    if (Raider.posX <= player1.posX && !Raider.shooting)
                        Raider.posX+=Raider.getSpeed();
                    if (Raider.posX >= player1.posX && !Raider.shooting)
                        Raider.posX-=Raider.getSpeed();
                    if (!Raider.shooting && Raider.posX+gRaiderSprite.getWidth()/2 >= player1.posX && Raider.posX+gRaiderSprite.getWidth()/2 <= player1.posX+gFighterSprite.getWidth()/2) {
                        Raider.shooting = true;
                        aBulletX = Raider.posX+gRaiderSprite.getWidth()/2;
                        aBulletY = Raider.posY+gRaiderSprite.getHeight()*2/3;
                    }
                    if (Raider.shooting) {
                        aBulletY+=player1.bullSpeed*Raider.getRate();
                    }
                    // Alien bullet collision
                    if (aBulletY > SCREEN_HEIGHT*3/2+gBulletSprite.getHeight()) {
                        Raider.shooting = false;
                        aBulletY = -gBulletSprite.getHeight();
                    } else if (aBulletY > player1.posY && aBulletY < player1.posY+gFighterSprite.getHeight() && aBulletX > player1.posX && aBulletX < player1.posX+gFighterSprite.getWidth()) {
                        aBulletY = SCREEN_HEIGHT;
                        player1.health-=Raider.getDamage();
                    }
                    // Player bullet collision
                    for (Bullets *bulle = bull; bulle != NULL; bulle = bulle->next) {
                        if (bulle->posY < Raider.posY+gRaiderSprite.getHeight() && bulle->posX > Raider.posX && bulle->posX < Raider.posX+gRaiderSprite.getWidth()) {
                            delNode(&bull, bulle->posX, bulle->posY);
                            Raider.health-=player1.damage;
                            int rad = rand()%10;
                            if (rad == 0)
                                player1.health++;
                        }
                        gBulletSprite.render(bulle->posX, bulle->posY);
                    }

                    if (Raider.health <= 0) {
                        Raider.posY = -gRaiderSprite.getHeight();
                        if (player1.turretsCooled) {
                            stages[0] = 0;
                            stages[1] = 1;
                            gameTime = SDL_GetTicks();
                            spdOnScrn = true;
                        }
                        aBulletY = -gBulletSprite.getHeight();
                    }
                } else if (stages[1] && spdOnScrn) {
                    if (spdY < SCREEN_HEIGHT*4/5)
                        spdY+=5;
                    int ramd = rand()%20;
                    if (ramd > 16)
                        spdX-=5;
                    if (ramd < 4)
                        spdX+=5;
                    if (spdY+gSpeedSprite.getHeight() > player1.posY && spdX+gSpeedSprite.getWidth() > player1.posX && spdX+gSpeedSprite.getWidth()/2 < player1.posX+gFighterSprite.getWidth()) {
                        player1.speed+=4;
                        player1.bullSpeed+=5;
                        player1.coolSpeed+=1;
                        player1.health+=30;
                        spdY = -gSpeedSprite.getHeight();
                        spdOnScrn = false;
                    } else if (spdY+gSpeedSprite.getHeight() > player1.posY && spdX > player1.posX && spdX < player1.posX+gFighterSprite.getWidth()) {
                        player1.speed+=4;
                        player1.bullSpeed+=5;
                        player1.coolSpeed+=1;
                        player1.health+=30;
                        spdY = -gSpeedSprite.getHeight();
                        spdOnScrn = false;
                    }
                } else if (stages[1] && start && SDL_GetTicks() > gameTime+1500 && !gameOver) {
                    if (Striker.posY < 0)
                        Striker.posY+=Striker.getSpeed();
                    if (Striker.posX <= player1.posX && !Striker.shooting)
                        Striker.posX+=Striker.getSpeed();
                    if (Striker.posX >= player1.posX && !Striker.shooting)
                        Striker.posX-=Striker.getSpeed();
                    if (Striker.timeSinceMove == 0)
                        Striker.timeSinceMove = SDL_GetTicks();
                    int randNum = rand() % 10;
                    if (randNum > 6 && Striker.timeSinceMove+300 < SDL_GetTicks() && Striker.posY+gStrikerSprite.getHeight()+Striker.getSpeed() < SCREEN_HEIGHT/2) {
                        Striker.posY+=Striker.getSpeed()/3;
                        Striker.timeSinceMove = SDL_GetTicks();
                    } else if (randNum < 1 && Striker.timeSinceMove+300 < SDL_GetTicks() && Striker.posY+Striker.getSpeed() > 0) {
                        Striker.posY-=Striker.getSpeed()/3;
                        Striker.timeSinceMove = SDL_GetTicks();
                    }
                    if (!Striker.shooting && Striker.posX+gStrikerSprite.getWidth()/2 >= player1.posX && Striker.posX+gStrikerSprite.getWidth()/2 <= player1.posX+gFighterSprite.getWidth()/2) {
                        Striker.shooting = true;
                        aBulletX = Striker.posX+gStrikerSprite.getWidth()/2;
                        aBulletY = Striker.posY+gStrikerSprite.getHeight()*2/3;
                    }
                    if (Striker.shooting) {
                        aBulletY+=player1.bullSpeed*Striker.getRate();
                    }
                    // Alien bullet collision
                    if (aBulletY > SCREEN_HEIGHT+gBulletSprite.getHeight()) {
                        Striker.shooting = false;
                        aBulletY = -gBulletSprite.getHeight();
                    } else if (aBulletY > player1.posY && aBulletX > player1.posX && aBulletX < player1.posX+gFighterSprite.getWidth()) {
                        Striker.shooting = false;
                        aBulletY = -gBulletSprite.getHeight();
                        player1.health-=Striker.getDamage();
                    }
                    // Player bullet collision
                    for (Bullets *bulle = bull; bulle != NULL; bulle = bulle->next) {
                        if (bulle->posY < Striker.posY+gStrikerSprite.getHeight() && bulle->posX > Striker.posX && bulle->posX < Striker.posX+gStrikerSprite.getWidth()) {
                            delNode(&bull, bulle->posX, bulle->posY);
                            Striker.health-=player1.damage;
                            int rad = rand()%10;
                            if (rad == 0)
                                player1.health++;
                        }
                        gBulletSprite.render(bulle->posX, bulle->posY);
                    }
                    if (Striker.health <= 0) {
                        Striker.posY = -gStrikerSprite.getHeight();
                        if (player1.turretsCooled) {
                            stages[1] = 0;
                            stages[2] = 1;
                            gameTime = SDL_GetTicks();
                            damgOnScrn = true;
                        }
                        aBulletY = -gBulletSprite.getHeight();
                    }
                } else if (stages[2] && damgOnScrn) {
                    if (damgY < SCREEN_HEIGHT*4/5)
                        damgY+=5;
                    int ramd = rand()%20;
                    if (ramd > 16)
                        damgX-=5;
                    if (ramd < 4)
                        damgX+=5;
                    if (damgY+gDamageSprite.getHeight() > player1.posY && damgX+gDamageSprite.getWidth() > player1.posX && damgX+gDamageSprite.getWidth()/2 < player1.posX+gFighterSprite.getWidth()) {
                        player1.health+=40;
                        player1.damage+=30;
                        damgY = -gDamageSprite.getHeight();
                        damgOnScrn = false;
                    } else if (damgY+gDamageSprite.getHeight() > player1.posY && damgX > player1.posX && damgX < player1.posX+gFighterSprite.getWidth()) {
                        player1.health+=40;
                        player1.damage+=30;
                        damgY = -gDamageSprite.getHeight();
                        damgOnScrn = false;
                    }
                } else if (stages[2] && start && SDL_GetTicks() > gameTime+1500 && !gameOver) {
                    if (Thrasher.posY < 0)
                        Thrasher.posY+=Thrasher.getSpeed();
                    if (Thrasher.posX <= player1.posX && !Thrasher.shooting)
                        Thrasher.posX+=Thrasher.getSpeed();
                    if (Thrasher.posX >= player1.posX && !Thrasher.shooting)
                        Thrasher.posX-=Thrasher.getSpeed();
                    if (!Thrasher.shooting && Thrasher.posX+gThrasherSprite.getWidth()/2 >= player1.posX && Thrasher.posX+gThrasherSprite.getWidth()/2 <= player1.posX+gFighterSprite.getWidth()/2) {
                        Thrasher.shooting = true;
                        aBulletX = Thrasher.posX+gThrasherSprite.getWidth()/2;
                        aBulletY = Thrasher.posY+gThrasherSprite.getHeight()*2/3;
                    }
                    if (Thrasher.shooting) {
                        aBulletY+=player1.bullSpeed*Thrasher.getRate();
                    }
                    // Alien bullet collision
                    if (aBulletY > SCREEN_HEIGHT*3+gBulletSprite.getHeight()) {
                        Thrasher.shooting = false;
                        aBulletY = -gBulletSprite.getHeight();
                    } else if (aBulletY > player1.posY && aBulletY < player1.posY+gFighterSprite.getHeight() && aBulletX > player1.posX && aBulletX < player1.posX+gFighterSprite.getWidth()) {
                        aBulletY = SCREEN_HEIGHT;
                        player1.health-=Thrasher.getDamage();
                    }
                    // Player bullet collision
                    for (Bullets *bulle = bull; bulle != NULL; bulle = bulle->next) {
                        if (bulle->posY < Thrasher.posY+gThrasherSprite.getHeight() && bulle->posX > Thrasher.posX && bulle->posX < Thrasher.posX+gThrasherSprite.getWidth()) {
                            delNode(&bull, bulle->posX, bulle->posY);
                            Thrasher.health-=player1.damage;
                            int rad = rand()%10;
                            if (rad == 0)
                                player1.health++;
                        }
                        gBulletSprite.render(bulle->posX, bulle->posY);
                    }
                    if (Thrasher.health <= 0) {
                        Thrasher.posY = -gThrasherSprite.getHeight();
                        player1.health+=10;
                        stages[2] = 0;
                        aBulletY = -gBulletSprite.getHeight();
                        gameTime = SDL_GetTicks();
                    }
                } else if (!stages[0] && !stages[1] && !stages[2]) {
                    gameOver = true;
                    win = true;
                    player1.shooting = false;
                    if (!flag) {
                        flag = true;
                        gameTime = SDL_GetTicks();
                    }

                }

                if (player1.health <= 1) {
                    player1.health = 1;
                    player1.shooting = false;
                    gameOver = true;
                } else if (player1.health > 100) {
                    player1.health = 100;
                }

                if (SDL_GetTicks()-startTime >= 179000 && start) { player1.shooting = false; gameOver = true; }


                // RENDER monster code
				// Clear the window
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				// Render background(s)
				gBackgroundTexture.render(0,backgroundY[0]);
				gBackgroundTexture.render(0, backgroundY[1]);

				// Render projectiles
				for (Bullets *bulle = bull; bulle != NULL; bulle = bulle->next)
                    gBulletSprite.render(bulle->posX, bulle->posY);
				if (Raider.shooting || Striker.shooting || Thrasher.shooting)
                    gABulletSprite.render(aBulletX, aBulletY);

                // Render items
                if (spdOnScrn)
                    gSpeedSprite.render(spdX, spdY);
                if (damgOnScrn)
                    gDamageSprite.render(damgX, damgY);

                // Render enemies
                if (enemies[1] && Raider.health > .75*Raider.getMaxHealth()) {
                    gRaiderSprite.render(Raider.posX, Raider.posY);
                } else if (enemies[1] && Raider.health > .5*Raider.getMaxHealth()) {
                    gRaiderDam1Sprite.render(Raider.posX, Raider.posY);
                } else if (enemies[1] && Raider.health > .25*Raider.getMaxHealth()) {
                    gRaiderDam2Sprite.render(Raider.posX, Raider.posY);
                } else {
                    gRaiderDam3Sprite.render(Raider.posX, Raider.posY);
                }
                if (enemies[2] && Striker.health > .75*Striker.getMaxHealth()) {
                    gStrikerSprite.render(Striker.posX, Striker.posY);
                } else if (enemies[2] && Striker.health > .5*Striker.getMaxHealth()) {
                    gStrikerDam1Sprite.render(Striker.posX, Striker.posY);
                } else if (enemies[2] && Striker.health > .25*Striker.getMaxHealth()) {
                    gStrikerDam2Sprite.render(Striker.posX, Striker.posY);
                } else {
                    gStrikerDam3Sprite.render(Striker.posX, Striker.posY);
                }
                if (enemies[3] && Thrasher.health > .75*Thrasher.getMaxHealth()) {
                    gThrasherSprite.render(Thrasher.posX, Thrasher.posY);
                } else if (enemies[3] && Thrasher.health > .5*Thrasher.getMaxHealth()) {
                    gThrasherDam1Sprite.render(Thrasher.posX, Thrasher.posY);
                } else if (enemies[3] && Thrasher.health > .25*Thrasher.getMaxHealth()) {
                    gThrasherDam2Sprite.render(Thrasher.posX, Thrasher.posY);
                } else {
                    gThrasherDam3Sprite.render(Thrasher.posX, Thrasher.posY);
                }

                if (start) {
                    if (!win) {
                        // Render HUD
                        gHealthSprite.render(SCREEN_WIDTH/30, SCREEN_HEIGHT/40+100-player1.health, &gHealthClip);
                        gAmmoSprite.setColorMod( r, g, b );
                        gAmmoSprite.render(SCREEN_WIDTH-SCREEN_WIDTH*1/30-gAmmoSprite.getWidth(), SCREEN_HEIGHT/40);
                    }

                    // Render turrets
                    gTurretSprite.render(player1.posX+gFighterSprite.getWidth()/6,player1.turretY);
                    gTurretSprite.render(player1.posX+gFighterSprite.getWidth()*4/6,player1.turretY,0,0,0,SDL_FLIP_HORIZONTAL);

                    // Render fighter
                    gFighterSprite.render(player1.posX, player1.posY);
                } else {
                    gTextTextureStar.render((SCREEN_WIDTH-gTextTextureStar.getWidth())/2, SCREEN_HEIGHT/2-gTextTextureStar.getHeight());
                    gTextTextureCollider.render((SCREEN_WIDTH-gTextTextureCollider.getWidth())/2, SCREEN_HEIGHT/2);
                    pressStart.str("Press space to start");
                    gPressStartTexture.loadFromRenderedText( pressStart.str().c_str(), textColor, pressStartFont );
                    gPressStartTexture.setAlphaMod(((sin(((double)p2StartA/255)*360 * PI/180)+1)/2*(255*10/9)) <= 255 ? (sin(((double)p2StartA/255)*360 * PI/180)+1)/2*(255*10/9) : 255);
                    gPressStartTexture.render(SCREEN_WIDTH/2-gPressStartTexture.getWidth()/2, SCREEN_HEIGHT*2/3);
                    p2StartA++;
                }

                // Render level text
                for (size_t i = 0; i < sizeof(loading); i++) {
                    if (loading[i] && i == 0)
                        gTextTextureLevel1.render(SCREEN_WIDTH/2-gTextTextureLevel1.getWidth()/2, SCREEN_HEIGHT/2-gTextTextureLevel1.getHeight()/2);
                    if (loading[i] && i == 1)
                        gTextTextureLevel2.render(SCREEN_WIDTH/2-gTextTextureLevel2.getWidth()/2, SCREEN_HEIGHT/2-gTextTextureLevel2.getHeight()/2);
                    if (loading[i] && i == 2)
                        gTextTextureLevel3.render(SCREEN_WIDTH/2-gTextTextureLevel3.getWidth()/2, SCREEN_HEIGHT/2-gTextTextureLevel3.getHeight()/2);
                }

                // Render game over text
                if (gameOver && !win) {
                    gTextTextureGame.render((SCREEN_WIDTH-gTextTextureGame.getWidth())/2, SCREEN_HEIGHT/2-gTextTextureGame.getHeight());
                    gTextTextureOver.render((SCREEN_WIDTH-gTextTextureOver.getWidth())/2, SCREEN_HEIGHT/2);
                    volume--;
                } else if (gameOver && win) {
                    if (player1.turretY < player1.posY+gFighterSprite.getHeight()/2) {
                        player1.turretY+=player1.turretSpeed;
                    } else if (player1.posX+gFighterSprite.getWidth()/2-SCREEN_WIDTH/2 > 10) {
                        player1.posX-=5;
                        puts("a");
                    } else if (player1.posX+gFighterSprite.getWidth()/2-SCREEN_WIDTH/2 < -10) {
                        player1.posX+=5;
                        printf("%db\n",player1.posX+gFighterSprite.getWidth()/2-SCREEN_WIDTH/2);
                    } else {
                        player1.posX = SCREEN_WIDTH/2-gFighterSprite.getWidth()/2;
                    }
                    if (SDL_GetTicks() > gameTime+2000 && player1.posY > -gFighterSprite.getHeight() && player1.posX+gFighterSprite.getWidth()/2 == SCREEN_WIDTH/2) {
                        player1.posY-=2*accel;
                        player1.turretY-=2*accel;
                        backgroundSpeed = 6;
                        accel+=.5;
                    } else {
                        backgroundSpeed = 10;
                        if (player1.posY <= -gFighterSprite.getHeight())
                            gWinnerSprite.render(0, SCREEN_HEIGHT/2-gWinnerSprite.getHeight()/2);
                        //volume--;
                        if (score == 0)
                            score = 179 - (SDL_GetTicks()-startTime)/1000;
                    }
                }
                //printf("%d\n", SDL_GetTicks()-startTime);
                Mix_VolumeMusic(volume);
                //printf("Average volume is %d\n",volume);
                //printf("%f\n",(sin(((double)p2StartA/255)*360 * PI/180)+1)/2*(255*10/9));
                //printf("%d-%d\n",backgroundY[0], backgroundY[1]);
                //printf("%d\n",score);

				// Update window
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	// Free resources and close SDL
	close();

	return 0;
}
