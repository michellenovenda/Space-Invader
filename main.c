// [main.c]
// this template is provided for the 2D shooter game.

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>

// If defined, logs will be shown on console and written to file.
// If commented out, logs will not be shown nor be saved.
#define LOG_ENABLED

/* Constants. */

// Frame rate (frame per second)
const int FPS = 60;
// Display (screen) width.
const int SCREEN_W = 1000;
// Display (screen) height.
const int SCREEN_H = 800;
// At most 4 audios can be played at a time.
const int RESERVE_SAMPLES = 4;
// Same as:
// const int SCENE_MENU = 1;
// const int SCENE_START = 2;
const float resurrect = 1; //time for plane to be dead.

enum
{
    SCENE_MENU = 1,
    SCENE_START = 2

    , SCENE_SETTINGS = 3
    , SCENE_LOSE = 4
    , SCENE_WIN = 5
    , SCENE_CHARACTER = 6
    , SCENE_INSTRUCTION1 = 7
    , SCENE_INSTRUCTION2 = 8
};

/* Input states */

// The active scene id.
int active_scene;
// Keyboard state, whether the key is down or not.
bool key_state[ALLEGRO_KEY_MAX];
// Mouse state, whether the key is down or not.
// 1 is for left, 2 is for right, 3 is for middle.
bool* mouse_state;
// Mouse position.
int mouse_x, mouse_y;
// TODO: More variables to store input states such as joysticks, ...
int kill_count = 0;
int counter = 0;
int counter_bigboss = 10;

int plane1 = 1;
int plane2 = 0;
int plane3 = 0;
int plane4 = 0;
int plane5 = 0;

int music_flag1 = 1;
int music_flag2 = 0;
int music_flag3 = 0;
int music_flag4 = 0;

/* Variables for allegro basic routines. */

ALLEGRO_DISPLAY* game_display;
ALLEGRO_EVENT_QUEUE* game_event_queue;
ALLEGRO_TIMER* game_update_timer;

/* Shared resources*/

ALLEGRO_FONT* font_pirulen_32;
ALLEGRO_FONT* font_pirulen_24;
ALLEGRO_FONT* font_earthorbiter_20;
ALLEGRO_FONT* font_sega_50;
ALLEGRO_FONT* font_death_star_70;
ALLEGRO_FONT* font_over_there_70; //35
ALLEGRO_FONT* font_stone_50; //150
ALLEGRO_FONT* font_darth_chowder_50;

/* Menu Scene resources*/
ALLEGRO_BITMAP* main_img_background;
ALLEGRO_BITMAP* img_interstellar_odyssey;
ALLEGRO_BITMAP* img_interplanetary_odyssey;
ALLEGRO_BITMAP* img_electronic_fantasy;
ALLEGRO_BITMAP* img_background_settings;
ALLEGRO_BITMAP* img_base;
ALLEGRO_BITMAP* img_arrow_character;
ALLEGRO_BITMAP* img_fuel;
ALLEGRO_BITMAP* img_bigboss_bullet;
ALLEGRO_BITMAP* img_bag;
ALLEGRO_BITMAP* img_alien_laser;

ALLEGRO_BITMAP* img_bluesky;
ALLEGRO_BITMAP* img_orangesky;
ALLEGRO_BITMAP* img_lose;

ALLEGRO_BITMAP* img_enemies_lvl3;
ALLEGRO_BITMAP* img_enemies_lvl5;
ALLEGRO_BITMAP* img_bigboss;

ALLEGRO_BITMAP* img_instruction;
ALLEGRO_BITMAP* img_instruction1;
ALLEGRO_BITMAP* img_instruction2;

ALLEGRO_BITMAP* img_settings;
ALLEGRO_BITMAP* img_settings2;
ALLEGRO_BITMAP* img_settings;
ALLEGRO_BITMAP* img_settings2;

ALLEGRO_SAMPLE* main_bgm;
ALLEGRO_SAMPLE_ID main_bgm_id;
ALLEGRO_SAMPLE* audio_electronic_fantasy;
ALLEGRO_SAMPLE* audio_interstellar_odyssey;
ALLEGRO_SAMPLE* audio_interplanetary_odyssey;
ALLEGRO_SAMPLE_ID audio_electronic_fantasy_id;
ALLEGRO_SAMPLE_ID audio_interplanetary_odyssey_id;
ALLEGRO_SAMPLE_ID audio_interstellar_odyssey_id;

ALLEGRO_SAMPLE* audio_shoot;
ALLEGRO_SAMPLE* audio_crash;
ALLEGRO_SAMPLE* audio_bigboss_shoot;
ALLEGRO_SAMPLE* audio_money;
ALLEGRO_SAMPLE* audio_fuel;
ALLEGRO_SAMPLE_ID audio_shoot_id;
ALLEGRO_SAMPLE_ID audio_crash_id;
ALLEGRO_SAMPLE_ID audio_bigboss_shoot_id;
ALLEGRO_SAMPLE_ID audio_money_id;
ALLEGRO_SAMPLE_ID audio_fuel_id;

/* Start Scene resources*/
ALLEGRO_BITMAP* start_img_background;
ALLEGRO_BITMAP* start_img_plane;
ALLEGRO_BITMAP* img_plane2;
ALLEGRO_BITMAP* img_plane3;
ALLEGRO_BITMAP* img_plane4;
ALLEGRO_BITMAP* img_plane5;
ALLEGRO_BITMAP* img_shelf;
ALLEGRO_BITMAP* img_laser;

ALLEGRO_BITMAP* start_img_enemy;
ALLEGRO_SAMPLE* start_bgm;
ALLEGRO_SAMPLE_ID start_bgm_id;
ALLEGRO_BITMAP* img_bullet;

typedef struct
{
    // The center coordinate of the image.
    float x, y;
    // The width and height of the object.
    float w, h;
    // The velocity in x, y axes.
    float vx, vy;
    // Should we draw this object on the screen.
    bool hidden;
    // The pointer to the object’s image.

    int score;
    int hp;

    int e_hp;

    ALLEGRO_BITMAP* img;
} MovableObject;
void draw_movable_object(MovableObject obj);
#define MAX_ENEMY 3
#define MAX_BULLET 5
#define MAX_BAG 2
#define MAX_BIGBOSS_BULLET 3
#define MAX_ALIEN_LASER 3

MovableObject plane;
MovableObject enemies[MAX_ENEMY];
MovableObject enemies1[MAX_ENEMY];
MovableObject enemies2[MAX_ENEMY];
MovableObject bigboss;
MovableObject laser;
MovableObject bullets[MAX_BULLET];
MovableObject bigboss_bullets[MAX_BIGBOSS_BULLET];
MovableObject alien_laser[MAX_ALIEN_LASER];
MovableObject fuel;
MovableObject bag[MAX_BAG];

const float MAX_COOLDOWN = 0.2;
const float MAX_ENEMY_COOLDOWN = 0.5; //enemy cooldown
const float MAX_ALIEN_COOLDOWN = 1;
double last_shoot_timestamp;
double last_enemy_timestamp;
double last_alien_timestamp;
double plane_crash;


/* Declare function prototypes. */

// Initialize allegro5 library
void allegro5_init(void);
// Initialize variables and resources.
// Allows the game to perform any initialization it needs before
// starting to run.
void game_init(void);
// Process events inside the event queue using an infinity loop.
void game_start_event_loop(void);
// Run game logic such as updating the world, checking for collision,
// switching scenes and so on.
// This is called when the game should update its logic.
void game_update(void);
// Draw to display.
// This is called when the game should draw itself.
void game_draw(void);
// Release resources.
// Free the pointers we allocated.
void game_destroy(void);
// Function to change from one scene to another.
void game_change_scene(int next_scene);
// Load resized bitmap and check if failed.
ALLEGRO_BITMAP* load_bitmap_resized(const char* filename, int w, int h);

bool pnt_in_rect(int px, int py, int x, int y, int w, int h);

/* Event callbacks. */
void on_key_down(int keycode);
void on_mouse_down(int btn, int x, int y);

/* Declare function prototypes for debugging. */

// Display error message and exit the program, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// If the program crashes unexpectedly, you can inspect "log.txt" for
// further information.
void game_abort(const char* format, ...);
// Log events for later debugging, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// You can inspect "log.txt" for logs in the last run.
void game_log(const char* format, ...);
// Log using va_list.
void game_vlog(const char* format, va_list arg);

int main(int argc, char** argv)
{
    // Set random seed for better random outcome.
    srand(time(NULL));
    allegro5_init();
    game_log("Allegro5 initialized");
    game_log("Game begin");
    // Initialize game variables.
    game_init();
    game_log("Game initialized");
    // Draw the first frame.
    game_draw();
    game_log("Game start event loop");
    // This call blocks until the game is finished.
    game_start_event_loop();
    game_log("Game end");
    game_destroy();
    return 0;
}

void allegro5_init(void)
{
    if (!al_init())
        game_abort("failed to initialize allegro");

    // Initialize add-ons.
    if (!al_init_primitives_addon())
        game_abort("failed to initialize primitives add-on");
    if (!al_init_font_addon())
        game_abort("failed to initialize font add-on");
    if (!al_init_ttf_addon())
        game_abort("failed to initialize ttf add-on");
    if (!al_init_image_addon())
        game_abort("failed to initialize image add-on");
    if (!al_install_audio())
        game_abort("failed to initialize audio add-on");
    if (!al_init_acodec_addon())
        game_abort("failed to initialize audio codec add-on");
    if (!al_reserve_samples(RESERVE_SAMPLES))
        game_abort("failed to reserve samples");
    if (!al_install_keyboard())
        game_abort("failed to install keyboard");
    if (!al_install_mouse())
        game_abort("failed to install mouse");

    // Setup game display.
    game_display = al_create_display(SCREEN_W, SCREEN_H);
    if (!game_display)
        game_abort("failed to create display");
    al_set_window_title(game_display, "Space Shooter");

    // Setup update timer.
    game_update_timer = al_create_timer(1.0f / FPS);
    if (!game_update_timer)
        game_abort("failed to create timer");

    // Setup event queue.
    game_event_queue = al_create_event_queue();
    if (!game_event_queue)
        game_abort("failed to create event queue");

    // Malloc mouse buttons state according to button counts.
    const unsigned m_buttons = al_get_mouse_num_buttons();
    game_log("There are total %u supported mouse buttons", m_buttons);
    // mouse_state[0] will not be used.
    mouse_state = malloc((m_buttons + 1) * sizeof(bool));
    memset(mouse_state, false, (m_buttons + 1) * sizeof(bool));

    // Register display, timer, keyboard, mouse events to the event queue.
    al_register_event_source(game_event_queue, al_get_display_event_source(game_display));
    al_register_event_source(game_event_queue, al_get_timer_event_source(game_update_timer));
    al_register_event_source(game_event_queue, al_get_keyboard_event_source());
    al_register_event_source(game_event_queue, al_get_mouse_event_source());
    // TODO: Register other event sources such as timer, video, ...

    // Start the timer to update and draw the game.
    al_start_timer(game_update_timer);
}

void game_init(void)
{
    /* Shared resources*/
    font_pirulen_32 = al_load_font("pirulen.ttf", 32, 0);
    if (!font_pirulen_32)
        game_abort("failed to load font: pirulen.ttf with size 32");

    font_pirulen_24 = al_load_font("pirulen.ttf", 24, 0);
    if (!font_pirulen_24)
        game_abort("failed to load font: pirulen.ttf with size 24");

    font_earthorbiter_20 = al_load_font("earthorbiter.ttf", 20, 0);
    if (!font_earthorbiter_20)
        game_abort("failed to load font: earthorbiter.ttf with size 20");

    font_sega_50 = al_load_font("sega.ttf", 50, 0);
    if (!font_sega_50)
        game_abort("failed to load font: sega.ttf with size 50");

    font_death_star_70 = al_load_font("deathstar.ttf", 70, 0);
    if (!font_death_star_70)
        game_abort("failed to load font: deathstar.ttf with size 70");

    font_over_there_70 = al_load_font("overthere.ttf", 35, 0);
    if (!font_over_there_70)
        game_abort("failed to load font: overthere.ttf with size 70");

    font_stone_50 = al_load_font("stone.ttf", 150, 0);
    if (!font_stone_50)
        game_abort("failed to load font: stone.ttf with size 50");

    font_darth_chowder_50 = al_load_font("darthchowder.ttf", 50, 0);
    if (!font_darth_chowder_50)
        game_abort("failed to load font: darthchowder.ttf with size 50");

    /* Menu Scene resources*/
    main_img_background = load_bitmap_resized("main-bg.jpg", SCREEN_W, SCREEN_H);

    main_bgm = al_load_sample("S31-Night Prowler.ogg"); //opening music
    if (!main_bgm)
        game_abort("failed to load audio: S31-Night Prowler.ogg");

    audio_electronic_fantasy = al_load_sample("Electronic Fantasy.ogg");
    if (!audio_electronic_fantasy)
        game_abort("failed to load audio: Electronic Fantasy.ogg");

    audio_interstellar_odyssey = al_load_sample("Interstellar Odyssey.ogg");
    if (!audio_interstellar_odyssey)
        game_abort("failed to load audio: Interstellar Odyssey.ogg");

    audio_interplanetary_odyssey = al_load_sample("Interplanetary Odyssey.ogg");
    if (!audio_interplanetary_odyssey)
        game_abort("failed to load audio: Interplanetary Odyssey.ogg");

    audio_shoot = al_load_sample("shoot.ogg");
    if (!audio_shoot)
        game_abort("failed to load audio: shoot.ogg");

    audio_bigboss_shoot = al_load_sample("bigboss_shoot.ogg");
    if (!audio_bigboss_shoot)
        game_abort("failed to load audio: bigboss_shoot.ogg");

    audio_crash = al_load_sample("crash.ogg");
    if (!audio_crash)
        game_abort("failed to load audio: crash.ogg");

    audio_money = al_load_sample("money.ogg");
    if (!audio_money)
        game_abort("failed to load audio: money.ogg");

    audio_fuel = al_load_sample("fuel.ogg");
    if (!audio_fuel)
        game_abort("failed to load audio: fuel.ogg");

    img_settings = al_load_bitmap("settings.png");
    if (!img_settings)
        game_abort("failed to load image: settings.png");
    img_settings2 = al_load_bitmap("settings2.png");
    if (!img_settings2)
        game_abort("failed to load image: settings2.png");

    img_base = al_load_bitmap("base.png");
    if (!img_base)
        game_abort("failed to load image: base.png");

    img_electronic_fantasy = al_load_bitmap("electronic_fantasy.png");
    if (!img_electronic_fantasy)
        game_abort("failed to load image: electronic_fantasy.png");

    img_interplanetary_odyssey = al_load_bitmap("interplanetary_odyssey.png");
    if (!img_interplanetary_odyssey)
        game_abort("failed to load image: interplanetary_odyssey.png");

    img_interstellar_odyssey = al_load_bitmap("interstellar_odyssey.png");
    if (!img_interstellar_odyssey)
        game_abort("failed to load image: interstellar_odyssey.png");

    img_background_settings = al_load_bitmap("outerspace.png");
    if (!img_background_settings)
        game_abort("failed to load image: outerspace.png");

    img_lose = al_load_bitmap("spacelose.png");
    if (!img_lose)
        game_abort("failed to load image: spacelose.png");

    img_arrow_character = al_load_bitmap("arrow.png");
    if (!img_arrow_character)
        game_abort("failed to load image: arrow.png");

    img_plane2 = al_load_bitmap("plane2.png");
    if (!img_plane2)
        game_abort("failed to load image: plane2.png");

    img_plane3 = al_load_bitmap("plane3.png");
    if (!img_plane3)
        game_abort("failed to load image: plane3.png");

    img_plane4 = al_load_bitmap("plane4.png");
    if (!img_plane4)
        game_abort("failed to load image: plane4.png");

    img_plane5 = al_load_bitmap("plane5.png");
    if (!img_plane5)
        game_abort("failed to load image: plane5.png");

    img_shelf = al_load_bitmap("shelf.png");
    if (!img_shelf)
        game_abort("failed to load image: shelf.png");

    img_fuel = al_load_bitmap("fuel.png");
    if (!img_fuel)
        game_abort("failed to load image: fuel.png");

    img_bag = al_load_bitmap("bag.png");
    if (!img_bag)
        game_abort("failed to load image: bag.png");


    img_enemies_lvl3 = al_load_bitmap("3times.png");
    if (!img_enemies_lvl3)
        game_abort("failed to load image: 3times.png");

    img_enemies_lvl5 = al_load_bitmap("5times.png");
    if (!img_enemies_lvl5)
        game_abort("failed to load image: 5times.png");

    img_bigboss = al_load_bitmap("bigboss.png");
    if (!img_bigboss)
        game_abort("failed to load image: bigboss.png");

    img_bigboss_bullet = al_load_bitmap("bigbossbullet.png");
    if (!img_bigboss_bullet)
        game_abort("failed to load image: bigbossbullet.png");

    img_bluesky = al_load_bitmap("bluesky.png");
    if (!img_bluesky)
        game_abort("failed to load image: bluesky.png");

    img_orangesky = al_load_bitmap("orangesky.png");
    if (!img_orangesky)
        game_abort("failed to load image: orangesky.png");


    img_instruction = al_load_bitmap("instruction.png");
    if (!img_instruction)
        game_abort("failed to load image: instruction.png");

    img_instruction1 = al_load_bitmap("instruction1.png");
    if (!img_instruction1)
        game_abort("failed to load image: instruction1.png");


    img_instruction2 = al_load_bitmap("instruction2.png");
    if (!img_instruction2)
        game_abort("failed to load image: instruction2.png");


    img_laser = al_load_bitmap("laser.png");
    if (!img_laser)
        game_abort("failed to load image: laser.png");

    img_alien_laser = al_load_bitmap("redlaser.png");
    if (!img_alien_laser)
        game_abort("failed to load image: redlaser.png");

    /* Start Scene resources*/
    start_img_background = load_bitmap_resized("start-bg.jpg", SCREEN_W, SCREEN_H);

    start_img_plane = al_load_bitmap("plane.png");
    if (!start_img_plane)
        game_abort("failed to load image: plane.png");

    start_img_enemy = al_load_bitmap("smallfighter0006.png");
    if (!start_img_enemy)
        game_abort("failed to load image: smallfighter0006.png");

    start_bgm = al_load_sample("mythica.ogg"); //game sound
    if (!start_bgm)
        game_abort("failed to load audio: mythica.ogg");

    img_bullet = al_load_bitmap("image12.png");
    if (!img_bullet)
        game_abort("failed to load image: image12.png");

    // Change to first scene.
    game_change_scene(SCENE_MENU);
}

void game_start_event_loop(void)
{
    bool done = false;
    ALLEGRO_EVENT event;
    int redraws = 0;
    while (!done)
    {
        al_wait_for_event(game_event_queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            // Event for clicking the window close button.
            game_log("Window close button clicked");
            done = true;
        }
        else if (event.type == ALLEGRO_EVENT_TIMER)
        {
            // Event for redrawing the display.
            if (event.timer.source == game_update_timer)
                // The redraw timer has ticked.
                redraws++;
        }


        else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            // Event for keyboard key down.
            game_log("Key with keycode %d down", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = true;
            on_key_down(event.keyboard.keycode);
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP)
        {
            // Event for keyboard key up.
            game_log("Key with keycode %d up", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = false;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
        {
            // Event for mouse key down.
            game_log("Mouse button %d down at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
            mouse_state[event.mouse.button] = true;
            on_mouse_down(event.mouse.button, event.mouse.x, event.mouse.y);
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            // Event for mouse key up.
            game_log("Mouse button %d up at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
            mouse_state[event.mouse.button] = false;
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            if (event.mouse.dx != 0 || event.mouse.dy != 0)
            {
                // Event for mouse move.
                game_log("Mouse move to (%d, %d)", event.mouse.x, event.mouse.y);
                mouse_x = event.mouse.x;
                mouse_y = event.mouse.y;
            }
            else if (event.mouse.dz != 0)
            {
                // Event for mouse scroll.
                game_log("Mouse scroll at (%d, %d) with delta %d", event.mouse.x, event.mouse.y, event.mouse.dz);
            }
        }
        // TODO: Process more events and call callbacks by adding more
        // entries inside Scene.

        // Redraw
        if (redraws > 0 && al_is_event_queue_empty(game_event_queue))
        {
            // if (redraws > 1)
            // game_log("%d frame(s) dropped", redraws - 1);
            // Update and draw the next frame.
            game_update();
            game_draw();
            redraws = 0;
        }
    }
}

void game_update(void)
{
    if (active_scene == SCENE_START)
    {
        plane.vx = plane.vy = 0;
        if (key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W])
            plane.vy -= 1;
        if (key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S])
            plane.vy += 1;
        if (key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A])
            plane.vx -= 1;
        if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])
            plane.vx += 1;

        // 0.71 is (1/sqrt(2)).
        plane.y += plane.vy * 10 * (plane.vx ? 0.71f : 1); //plane speed up down
        plane.x += plane.vx * 10 * (plane.vy ? 0.71f : 1); //plane speed right left

        if (plane.x - plane.w / 2 < 0)
            plane.x = plane.w / 2;

        else if (plane.x + plane.w / 2 > SCREEN_W)
            plane.x = SCREEN_W - plane.w / 2;

        if (plane.y - plane.h / 2 < 0)
            plane.y = plane.h / 2;

        else if (plane.y + plane.h / 2 > SCREEN_H)
            plane.y = SCREEN_H - plane.h / 2;


        double time_now = al_get_time(); //to get new time after being dead.

        //to revive the plane after being dead, but hp is still decreased
        if (time_now - plane_crash >= resurrect)
            plane.hidden = false;

        int i;

        for (int i = 0; i < MAX_BULLET; i++)
        {
            if (bullets[i].hidden == true)
                continue;

            bullets[i].x += bullets[i].vx;
            bullets[i].y += bullets[i].vy;

            if (bullets[i].y < 0)
                bullets[i].hidden = true;
        }

        //bigboss bullets
        for (int i = 0; i < MAX_BIGBOSS_BULLET; i++)
        {
            if (!bigboss_bullets[i].hidden)
            {
                bigboss_bullets[i].x += bigboss_bullets[i].vx;
                bigboss_bullets[i].y += bigboss_bullets[i].vy;
            }
        }

        for (int i = 0; i < MAX_BIGBOSS_BULLET; i++)
        {
            if (bigboss_bullets[i].y >= SCREEN_H)
                bigboss_bullets[i].hidden = true;
        }

        //laser bigboss
        if (!laser.hidden)
        {
            laser.x += laser.vx;
            laser.y += laser.vy;
        }
        if (laser.y < 0)
            laser.hidden = true;

        double now = al_get_time();

        if (!plane.hidden && counter_bigboss != 0)
        {
            if (key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN)
            {
                al_play_sample(audio_shoot, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                for (i = 0; i < MAX_BULLET; i++)
                {
                    if (bullets[i].hidden == true)
                        break;
                }
                if (i < MAX_BULLET)
                {
                    last_shoot_timestamp = now;
                    bullets[i].hidden = false;
                    bullets[i].x = plane.x;
                    bullets[i].y = plane.y;
                }
            }
        }

        if (!plane.hidden && counter_bigboss == 0 && kill_count >= 18)
        {
            if (key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN)
            {
                if (laser.hidden == true)
                {
                    last_shoot_timestamp = now;
                    laser.hidden = false;
                    laser.x = plane.x;
                    laser.y = plane.y;
                    counter_bigboss = 10;
                }
            }
        }

        //bigboss shooting bullets
        double enemy_now = al_get_time();
        for (int i = 0; i < MAX_BIGBOSS_BULLET; i++)
        {
            if (!bigboss.hidden && enemy_now - last_enemy_timestamp >= MAX_ENEMY_COOLDOWN)
            {
                if (bigboss_bullets[i].hidden)
                {
                    last_enemy_timestamp = enemy_now;
                    bigboss_bullets[i].hidden = false;
                    bigboss_bullets[i].x = bigboss.x;
                    bigboss_bullets[i].y = bigboss.y + bigboss.h / 2;
                }
            }
        }

        for (int i = 0; i < MAX_BIGBOSS_BULLET; i++)
        {
            if (!bigboss_bullets[i].hidden)
            {
                bigboss_bullets[i].x += bigboss_bullets[i].vx;
                bigboss_bullets[i].y += bigboss_bullets[i].vy;
            }
        }

        if (bigboss.y >= SCREEN_H)
            bigboss.hidden = true;

        if (kill_count < 10)
        {
            for (i = 0; i < MAX_ENEMY; i++) //to make the enemies move downwards
            {
                if (enemies[i].hidden == true)
                    continue;

                enemies[i].x += enemies[i].vx;
                enemies[i].y += enemies[i].vy;

                if (enemies[i].y > SCREEN_H)
                    enemies[i].hidden = true;
            }


            for (i = 0; i < MAX_ENEMY; i++)
            {
                if (enemies[i].hidden == true)
                {
                    break;
                }
            }

            if (i < MAX_ENEMY)  //to show the enemies again
            {
                enemies[i].hidden = false;
                enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
                enemies[i].y = 0;
            }

            if (!fuel.hidden)
            {
                fuel.x += fuel.vx;
                fuel.y += fuel.vy;
            }

            if (fuel.y > SCREEN_H)
                fuel.hidden = true;

            if (fuel.hidden == true)
            {
                fuel.hidden = false;
                fuel.x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
                fuel.y = 0;
            }

            for (i = 0; i < MAX_BAG; i++)
            {
                if (bag[i].hidden == true)
                    continue;

                bag[i].x += bag[i].vx;
                bag[i].y += bag[i].vy;

                if (bag[i].y > SCREEN_H)
                    bag[i].hidden = true;
            }

            for (i = 0; i < MAX_BAG; i++)
            {
                if (bag[i].hidden == true)
                    break;
            }

            if (i < MAX_BAG)  //to show the bag again
            {
                bag[i].hidden = false;
                bag[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
                bag[i].y = 0;
            }

            //COLLISION BULLET PLANE TO ENEMY
            for (i = 0; i < MAX_BULLET; i++) //if bullets is not hidden, then use for loop
            {
                if (!bullets[i].hidden)
                {
                    for (int j = 0; j < MAX_ENEMY; j++) //if enemies is not hidden, then use for loop
                    {
                        if (!enemies[j].hidden)
                        {
                            //when u know that bullets and enemies are not hidden,
                            //then set the coordinate of the bullets and the enemies
                            //so that when their coordinates align
                            //the enemies will disappear
                            if (bullets[i].x < enemies[j].x + enemies[j].w / 2 &&
                                bullets[i].x > enemies[j].x - enemies[j].w / 2 &&
                                bullets[i].y < enemies[j].y + enemies[j].h / 2 &&
                                bullets[i].y > enemies[j].y - enemies[j].h / 2)
                            {
                                al_play_sample(audio_crash, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                                enemies[j].hidden = true;
                                bullets[i].hidden = true;
                                plane.score += 10;
                                kill_count += 1;
                            }
                        }
                    }
                }
            }


            double time_crash = al_get_time();

            //ENEMIES CRASH TO PLANE
            for (i = 0; i < MAX_ENEMY; i++) //if enemy is not hidden, then use for loop
            {
                if (!enemies[i].hidden)
                {
                    if (!plane.hidden)
                        //when u know that plane and enemies are not hidden,
                        //when their coordinates align
                        //both the plane and enemies will disappear
                        if (plane.x - plane.w / 2 < enemies[i].x + enemies[i].w / 2 &&
                            plane.x + plane.w / 2 > enemies[i].x - enemies[i].w / 2 &&
                            plane.y - plane.h / 2 < enemies[i].y + enemies[i].h / 2 &&
                            plane.y + plane.h / 2 > enemies[i].y - enemies[i].h / 2)
                        {
                            al_play_sample(audio_crash, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                            enemies[i].hidden = true;
                            plane.hidden = true;
                            plane.hp -= 1;
                            plane_crash = time_crash; //to get the time when plane crashes the enemy;
                        }
                }
            }

        }

        if (kill_count >= 10 && kill_count < 15)
        {
            for (i = 0; i < MAX_ENEMY; i++) //to make the enemies move downwards
            {
                if (enemies1[i].hidden == true)
                    continue;

                enemies1[i].x += enemies1[i].vx;
                enemies1[i].y += enemies1[i].vy;

                if (enemies1[i].y > SCREEN_H)
                    enemies1[i].hidden = true;
            }

            for (i = 0; i < MAX_ENEMY; i++)
            {
                if (enemies1[i].hidden == true)
                    break;
            }

            if (i < MAX_ENEMY)   //to show the enemies again
            {
                enemies1[i].hidden = false;
                enemies1[i].x = enemies1[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies1[i].w);
                enemies1[i].y = 0;
            }

            if (!fuel.hidden)
            {
                fuel.x += fuel.vx;
                fuel.y += fuel.vy;
            }

            if (fuel.y > SCREEN_H)
                fuel.hidden = true;

            if (fuel.hidden == true)
            {
                fuel.hidden = false;
                fuel.x = enemies1[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies1[i].w);
                fuel.y = 0;
            }

            for (i = 0; i < MAX_BAG; i++)
            {
                if (bag[i].hidden == true)
                    continue;

                bag[i].x += bag[i].vx;
                bag[i].y += bag[i].vy;

                if (bag[i].y > SCREEN_H)
                    bag[i].hidden = true;
            }

            for (i = 0; i < MAX_BAG; i++)
            {
                if (bag[i].hidden == true)
                {
                    break;
                }
            }

            if (i < MAX_BAG) //to show the bag again
            {
                bag[i].hidden = false;
                bag[i].x = enemies1[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies1[i].w);
                bag[i].y = 0;
            }

            //COLLISION
            for (i = 0; i < MAX_BULLET; i++) //if bullets is not hidden, then use for loop
            {
                if (!bullets[i].hidden)
                {
                    for (int j = 0; j < MAX_ENEMY; j++) //if enemies is not hidden, then use for loop
                    {
                        if (!enemies1[j].hidden)
                            //when u know that bullets and enemies are not hidden,
                            //then set the coordinate of the bullets and the enemies
                            //so that when their coordinates align
                            //the enemies will disappear
                            if (bullets[i].x < enemies1[j].x + enemies1[j].w / 2 &&
                                bullets[i].x > enemies1[j].x - enemies1[j].w / 2 &&
                                bullets[i].y < enemies1[j].y + enemies1[j].h / 2 &&
                                bullets[i].y > enemies1[j].y - enemies1[j].h / 2)
                            {
                                al_play_sample(audio_crash, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                                enemies1[j].e_hp -= 1;
                                bullets[i].hidden = true;
                                if (enemies1[j].e_hp <= 0)
                                {
                                    enemies1[j].hidden = true;
                                    plane.score += 20;
                                    kill_count += 1;
                                }
                            }
                    }
                }
            }

            for (int i = 0; i < MAX_ENEMY; i++)
            {
                if (enemies1[i].hidden == true)
                {
                    enemies1[i].e_hp = 3;
                }
            }

            double time_crash = al_get_time();

            //ENEMIES CRASH TO PLANE
            for (i = 0; i < MAX_ENEMY; i++) //if enemy is not hidden, then use for loop
            {
                if (!enemies1[i].hidden)
                {
                    if (!plane.hidden)
                        //when u know that plane and enemies are not hidden,
                        //when their coordinates align
                        //both the plane and enemies will disappear
                        if (plane.x - plane.w / 2 < enemies1[i].x + enemies1[i].w / 2 &&
                            plane.x + plane.w / 2 > enemies1[i].x - enemies1[i].w / 2 &&
                            plane.y - plane.h / 2 < enemies1[i].y + enemies1[i].h / 2 &&
                            plane.y + plane.h / 2 > enemies1[i].y - enemies1[i].h / 2)
                        {
                            al_play_sample(audio_crash, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL); //ok but ganti suara
                            enemies1[i].hidden = true;
                            plane.hidden = true;
                            plane.hp -= 1;
                            plane_crash = time_crash; //to get the time when plane crashes the enemy;
                        }
                }
            }
        }

        if (kill_count >= 15 && kill_count < 18)
        {
            for (i = 0; i < MAX_ENEMY; i++) //to make the enemies move downwards
            {
                if (enemies2[i].hidden == true)
                    continue;

                enemies2[i].x += enemies2[i].vx;
                enemies2[i].y += enemies2[i].vy;

                if (enemies2[i].y > SCREEN_H)
                    enemies2[i].hidden = true;
            }


            for (i = 0; i < MAX_ENEMY; i++)
            {
                if (enemies2[i].hidden == true)
                {
                    break;
                }
            }

            if (i < MAX_ENEMY) //to show the enemies again
            {
                enemies2[i].hidden = false;
                enemies2[i].x = enemies2[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies2[i].w);
                enemies2[i].y = 0;
            }

            for (i = 0; i < MAX_ALIEN_LASER; i++)
            {
                if (alien_laser[i].hidden)
                    continue;

                alien_laser[i].x += alien_laser[i].vx;
                alien_laser[i].y += alien_laser[i].vy;

                if (alien_laser[i].y - alien_laser[i].h / 2 >= SCREEN_H)
                    alien_laser[i].hidden = true;
            }

            for (i = 0; i < MAX_ENEMY; i++)
            {
                if (!enemies2[i].hidden)
                {
                    for (int j = 0; j < MAX_ALIEN_LASER; j++)
                    {
                        if (alien_laser[j].hidden)
                        {
                            alien_laser[j].hidden = false;
                            alien_laser[j].x = enemies2[j].x;
                            alien_laser[j].y = enemies2[j].y + enemies2[j].h / 2;
                        }
                    }
                }
            }


            if (!fuel.hidden)
            {
                fuel.x += fuel.vx;
                fuel.y += fuel.vy;
            }

            if (fuel.y > SCREEN_H)
                fuel.hidden = true;

            if (fuel.hidden == true)
            {
                fuel.hidden = false;
                fuel.x = enemies2[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies2[i].w);
                fuel.y = 0;
            }

            for (i = 0; i < MAX_BAG; i++)
            {
                if (bag[i].hidden == true)
                    continue;

                bag[i].x += bag[i].vx;
                bag[i].y += bag[i].vy;

                if (bag[i].y > SCREEN_H)
                    bag[i].hidden = true;
            }

            for (i = 0; i < MAX_BAG; i++)
            {
                if (bag[i].hidden == true)
                {
                    break;
                }
            }

            if (i < MAX_BAG) //to show the bag again
            {
                bag[i].hidden = false;
                bag[i].x = enemies2[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies2[i].w);
                bag[i].y = 0;
            }

            //COLLISION
            for (i = 0; i < MAX_BULLET; i++) //if bullets is not hidden, then use for loop
            {
                if (!bullets[i].hidden)
                {
                    for (int j = 0; j < MAX_ENEMY; j++) //if enemies is not hidden, then use for loop
                    {
                        if (!enemies2[j].hidden)
                            //when u know that bullets and enemies are not hidden,
                            //then set the coordinate of the bullets and the enemies
                            //so that when their coordinates align
                            //the enemies will disappear
                            if (bullets[i].x < enemies2[j].x + enemies2[j].w / 2 &&
                                bullets[i].x > enemies2[j].x - enemies2[j].w / 2 &&
                                bullets[i].y < enemies2[j].y + enemies2[j].h / 2 &&
                                bullets[i].y > enemies2[j].y - enemies2[j].h / 2)
                            {
                                al_play_sample(audio_crash, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                                enemies2[j].e_hp -= 1;
                                bullets[i].hidden = true;
                                if (enemies2[j].e_hp <= 0)
                                {
                                    enemies2[j].hidden = true;
                                    plane.score += 30;
                                    kill_count += 1;
                                }

                            }
                    }
                }
            }

            //COLLISION LASER TO PLANE
            for (int i = 0; i < MAX_ALIEN_LASER; i++)
            {
                if (!alien_laser[i].hidden)
                {
                    if (!plane.hidden)
                    {
                        //when u know that bullets and enemies are not hidden,
                        //then set the coordinate of the bullets and the enemies
                        //so that when their coordinates align
                        //the enemies will disappear
                        if (alien_laser[i].x < plane.x + plane.w / 2 &&
                            alien_laser[i].x > plane.x - plane.w / 2 &&
                            alien_laser[i].y < plane.y + plane.h / 2 &&
                            alien_laser[i].y > plane.y - plane.h / 2)
                        {
                            al_play_sample(audio_crash, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                            plane.hidden = true;
                            alien_laser[i].hidden = true;
                            plane.hp -= 1;
                        }
                    }
                }
            }

            //COLLISION BULLET PLANE TO ALIEN LASER
            for (i = 0; i < MAX_BULLET; i++) //if bullets is not hidden, then use for loop
            {
                if (!bullets[i].hidden)
                {
                    for (int j = 0; j < MAX_ALIEN_LASER; j++) //if enemies is not hidden, then use for loop
                    {
                        if (!alien_laser[j].hidden)
                            //when u know that bullets and enemies are not hidden,
                            //then set the coordinate of the bullets and the enemies
                            //so that when their coordinates align
                            //the enemies will disappear
                            if (bullets[i].x < alien_laser[j].x + alien_laser[j].w / 2 &&
                                bullets[i].x > alien_laser[j].x - alien_laser[j].w / 2 &&
                                bullets[i].y < alien_laser[j].y + alien_laser[j].h / 2 &&
                                bullets[i].y > alien_laser[j].y - alien_laser[j].h / 2)
                            {
                                al_play_sample(audio_crash, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                                bullets[i].hidden = true;
                                alien_laser[j].hidden = true;
                                plane.score += 3;
                            }
                    }
                }
            }


            for (int i = 0; i < MAX_ENEMY; i++)
            {
                if (enemies2[i].hidden == true)
                {
                    enemies2[i].e_hp = 3;
                }
            }

            double time_crash = al_get_time();
            //ENEMIES CRASH TO PLANE
            for (i = 0; i < MAX_ENEMY; i++) //if enemy is not hidden, then use for loop
            {
                if (!enemies2[i].hidden)
                {
                    if (!plane.hidden)
                        //when u know that plane and enemies are not hidden,
                        //when their coordinates align
                        //both the plane and enemies will disappear
                        if (plane.x - plane.w / 2 < enemies2[i].x + enemies2[i].w / 2 &&
                            plane.x + plane.w / 2 > enemies2[i].x - enemies2[i].w / 2 &&
                            plane.y - plane.h / 2 < enemies2[i].y + enemies2[i].h / 2 &&
                            plane.y + plane.h / 2 > enemies2[i].y - enemies2[i].h / 2)
                        {
                            al_play_sample(audio_crash, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                            plane_crash = time_crash; //to get the time when plane crashes the enemy;
                            enemies2[i].hidden = true;
                            plane.hidden = true;
                            plane.hp -= 1;
                        }
                }
            }

        }

        if (kill_count >= 18)
        {
            for (int i = 0; i < MAX_ALIEN_LASER; i++)
            {
                alien_laser[i].hidden = true;
            }

            if (!bigboss.hidden)
            {
                bigboss.x += bigboss.vx;
                bigboss.y += bigboss.vy;

                if (bigboss.x - bigboss.w / 2 <= 0)
                {
                    bigboss.vx = 5;
                    bigboss.vy = 0;
                }

                else if (bigboss.x + bigboss.w / 2 >= SCREEN_W)
                {
                    bigboss.vx = -5;
                    bigboss.vy = 0;
                }
            }

            for (i = 0; i < MAX_ENEMY; i++) //to make the enemies move downwards
            {
                if (enemies[i].hidden == true)
                    continue;

                enemies[i].x += enemies[i].vx;
                enemies[i].y += enemies[i].vy;

                if (enemies[i].y > SCREEN_H)
                    enemies[i].hidden = true;
            }


            for (i = 0; i < MAX_ENEMY; i++)
            {
                if (enemies[i].hidden == true)
                {
                    break;
                }
            }

            if (i < MAX_ENEMY)   //to show the enemies again
            {
                enemies[i].hidden = false;
                enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
                enemies[i].y = 0;
            }

            //COLLISION BULLET PLANE TO ENEMY
            for (i = 0; i < MAX_BULLET; i++) //if bullets is not hidden, then use for loop
            {
                if (!bullets[i].hidden)
                {
                    for (int j = 0; j < MAX_ENEMY; j++) //if enemies is not hidden, then use for loop
                    {
                        if (!enemies[j].hidden)
                        {
                            //when u know that bullets and enemies are not hidden,
                            //then set the coordinate of the bullets and the enemies
                            //so that when their coordinates align
                            //the enemies will disappear
                            if (bullets[i].x < enemies[j].x + enemies[j].w / 2 &&
                                bullets[i].x > enemies[j].x - enemies[j].w / 2 &&
                                bullets[i].y < enemies[j].y + enemies[j].h / 2 &&
                                bullets[i].y > enemies[j].y - enemies[j].h / 2)
                            {
                                al_play_sample(audio_crash, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                                enemies[j].hidden = true;
                                bullets[i].hidden = true;
                                plane.score += 10;
                                kill_count += 1;
                            }
                        }
                    }
                }
            }


            double time_crash = al_get_time();

            //ENEMIES CRASH TO PLANE
            for (i = 0; i < MAX_ENEMY; i++) //if enemy is not hidden, then use for loop
            {
                if (!enemies[i].hidden)
                {
                    if (!plane.hidden)
                        //when u know that plane and enemies are not hidden,
                        //when their coordinates align
                        //both the plane and enemies will disappear
                        if (plane.x - plane.w / 2 < enemies[i].x + enemies[i].w / 2 &&
                            plane.x + plane.w / 2 > enemies[i].x - enemies[i].w / 2 &&
                            plane.y - plane.h / 2 < enemies[i].y + enemies[i].h / 2 &&
                            plane.y + plane.h / 2 > enemies[i].y - enemies[i].h / 2)
                        {
                            al_play_sample(audio_crash, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL); //ok but ganti suara
                            enemies[i].hidden = true;
                            plane.hidden = true;
                            plane.hp -= 1;
                            plane_crash = time_crash; //to get the time when plane crashes the enemy;
                        }
                }
            }


            if (!fuel.hidden)
            {
                fuel.x += fuel.vx;
                fuel.y += fuel.vy;
            }

            if (fuel.y > SCREEN_H)
                fuel.hidden = true;

            if (fuel.hidden == true)
            {
                fuel.hidden = false;
                fuel.x = bigboss.w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - bigboss.w);
                fuel.y = 0;
            }

            for (i = 0; i < MAX_BAG; i++)
            {
                if (bag[i].hidden == true)
                    continue;

                bag[i].x += bag[i].vx;
                bag[i].y += bag[i].vy;

                if (bag[i].y > SCREEN_H)
                    bag[i].hidden = true;
            }

            for (i = 0; i < MAX_BAG; i++)
            {
                if (bag[i].hidden == true)
                {
                    break;
                }
            }

            if (i < MAX_BAG)   //to show the bag again
            {
                bag[i].hidden = false;
                bag[i].x = bigboss.w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - bigboss.w);
                bag[i].y = 0;
            }

            //COLLISION BULLETS AND BIGBOSS
            for (i = 0; i < MAX_BULLET; i++) //if bullets is not hidden, then use for loop
            {
                if (!bullets[i].hidden)
                {
                    if (!bigboss.hidden)
                        //when u know that bullets and enemies are not hidden,
                        //then set the coordinate of the bullets and the enemies
                        //so that when their coordinates align
                        //the enemies will disappear
                        if (bullets[i].x < bigboss.x + bigboss.w / 2 &&
                            bullets[i].x > bigboss.x - bigboss.w / 2 &&
                            bullets[i].y < bigboss.y + bigboss.h / 2 &&
                            bullets[i].y > bigboss.y - bigboss.h / 2)
                        {
                            bigboss.e_hp -= 1;
                            plane.score += 20;
                            bullets[i].hidden = true;
                            counter += 1;

                            counter_bigboss -= 1;

                            if (bigboss.e_hp <= 0)
                            {
                                bigboss.hidden = true;
                            }
                        }
                }
            }


            //WHEN LASER SHOOT
            if (!laser.hidden)
            {
                if (!bigboss.hidden)
                    //when u know that bullets and enemies are not hidden,
                    //then set the coordinate of the bullets and the enemies
                    //so that when their coordinates align
                    //the enemies will disappear
                    if (laser.x < bigboss.x + bigboss.w / 2 &&
                        laser.x > bigboss.x - bigboss.w / 2 &&
                        laser.y < bigboss.y + bigboss.h / 2 &&
                        laser.y > bigboss.y - bigboss.h / 2)
                    {
                        bigboss.e_hp -= 20;
                        plane.score += 50;
                        laser.hidden = true;
                        if (bigboss.e_hp <= 0)
                        {
                            bigboss.hidden = true;
                        }
                    }
            }

            //COLLISION BULLET PLANE TO BULLET BIGBOSS
            for (i = 0; i < MAX_BULLET; i++) //if bullets is not hidden, then use for loop
            {
                if (!bullets[i].hidden)
                {
                    for (int j = 0; j < MAX_BIGBOSS_BULLET; j++) //if enemies is not hidden, then use for loop
                    {
                        if (!bigboss_bullets[j].hidden)
                            //when u know that bullets and enemies are not hidden,
                            //then set the coordinate of the bullets and the enemies
                            //so that when their coordinates align
                            //the enemies will disappear
                            if (bullets[i].x < bigboss_bullets[j].x + bigboss_bullets[j].w / 2 &&
                                bullets[i].x > bigboss_bullets[j].x - bigboss_bullets[j].w / 2 &&
                                bullets[i].y < bigboss_bullets[j].y + bigboss_bullets[j].h / 2 &&
                                bullets[i].y > bigboss_bullets[j].y - bigboss_bullets[j].h / 2)
                            {
                                bullets[i].hidden = true;
                                bigboss_bullets[j].hidden = true;
                                plane.score += 5;
                            }
                    }
                }
            }

            //bigboss shoot sound
            for (int i = 0; i < MAX_BIGBOSS_BULLET; i++)
            {
                if (!bigboss_bullets[i].hidden)
                {
                    al_play_sample(audio_bigboss_shoot, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
            }

            //COLLISION LASER PLANE TO BULLET BIGBOSS
            for (i = 0; i < MAX_BULLET; i++) //if bullets is not hidden, then use for loop
            {
                if (!bullets[i].hidden)
                {
                    if (!laser.hidden)
                        //when u know that bullets and enemies are not hidden,
                        //then set the coordinate of the bullets and the enemies
                        //so that when their coordinates align
                        //the enemies will disappear
                        if (bullets[i].x < laser.x + laser.w / 2 &&
                            bullets[i].x > laser.x - laser.w / 2 &&
                            bullets[i].y < laser.y + laser.h / 2 &&
                            bullets[i].y > laser.y - laser.h / 2)
                        {
                            bullets[i].hidden = true;
                            laser.hidden = true;
                            plane.score += 7;
                        }
                }
            }

            //COLLISION BULLET BIGBOSS TO PLANE
            for (int i = 0; i < MAX_BIGBOSS_BULLET; i++)
            {
                if (!bigboss_bullets[i].hidden)
                {
                    if (!plane.hidden)
                    {
                        //when u know that bullets and enemies are not hidden,
                        //then set the coordinate of the bullets and the enemies
                        //so that when their coordinates align
                        //the enemies will disappear
                        if (bigboss_bullets[i].x < plane.x + plane.w / 2 &&
                            bigboss_bullets[i].x > plane.x - plane.w / 2 &&
                            bigboss_bullets[i].y < plane.y + plane.h / 2 &&
                            bigboss_bullets[i].y > plane.y - plane.h / 2)
                        {
                            plane.hidden = true;
                            bigboss_bullets[i].hidden = true;
                            plane.hp -= 1;
                        }
                    }
                }
            }


            if (bigboss.hidden == true)
            {
                bigboss.e_hp = 15;
            }

            time_crash = al_get_time();

            //ENEMIES CRASH TO PLANE
            if (!bigboss.hidden)
            {
                if (!plane.hidden)
                    //when u know that plane and enemies are not hidden,
                    //when their coordinates align
                    //both the plane and enemies will disappear
                    if (plane.x - plane.w / 2 < bigboss.x + bigboss.w / 2 &&
                        plane.x + plane.w / 2 > bigboss.x - bigboss.w / 2 &&
                        plane.y - plane.h / 2 < bigboss.y + bigboss.h / 2 &&
                        plane.y + plane.h / 2 > bigboss.y - bigboss.h / 2)
                    {
                        plane.hidden = true;
                        plane.hp -= 1;
                        plane_crash = time_crash; //to get the time when plane crashes the enemy;
                    }
            }
        }

        //FUEL CRASH TO PLANE
        if (!fuel.hidden)
        {
            if (!plane.hidden)
                //when u know that plane and enemies are not hidden,
                //when their coordinates align
                //both the plane and enemies will disappear
                if (plane.x - plane.w / 2 < fuel.x + fuel.w / 2 &&
                    plane.x + plane.w / 2 > fuel.x - fuel.w / 2 &&
                    plane.y - plane.h / 2 < fuel.y + fuel.h / 2 &&
                    plane.y + plane.h / 2 > fuel.y - fuel.h / 2)
                {
                    al_play_sample(audio_fuel, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                    fuel.hidden = true;
                    plane.hidden = false;
                    counter += 1;
                    if (counter == 3)
                    {
                        plane.hp += 1;
                        counter = 0;
                    }
                }
        }

        //BAG CRASH TO PLANE
        for (int i = 0; i < MAX_BAG; i++) //if fuel is not hidden, then use for loop
        {
            if (!bag[i].hidden)
            {
                if (!plane.hidden)
                    //when u know that plane and enemies are not hidden,
                    //when their coordinates align
                    //both the plane and enemies will disappear
                    if (plane.x - plane.w / 2 < bag[i].x + bag[i].w / 2 &&
                        plane.x + plane.w / 2 > bag[i].x - bag[i].w / 2 &&
                        plane.y - plane.h / 2 < bag[i].y + bag[i].h / 2 &&
                        plane.y + plane.h / 2 > bag[i].y - bag[i].h / 2)
                    {
                        al_play_sample(audio_money, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
                        bag[i].hidden = true;
                        plane.hidden = false;
                        plane.score += 3;
                    }
            }
        }

        //lose condition
        if (plane.hp <= 0)
            game_change_scene(SCENE_LOSE);

        //win condition
        if (bigboss.hidden == true && plane.hidden == false && kill_count >= 18) //cek lagi
            game_change_scene(SCENE_WIN);
    }
}


void game_draw(void)
{
    if (active_scene == SCENE_MENU)
    {
        al_draw_bitmap(main_img_background, 0, 0, 0);
        al_draw_bitmap(img_instruction, 950, 50, 0);
        al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 30, ALLEGRO_ALIGN_CENTER, "Space Shooter");
        al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), 20, SCREEN_H - 50, 0, "Press enter key to start");


        if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W - 48, 10, 38, 38))
            al_draw_bitmap(img_settings2, SCREEN_W - 48, 0, 0);
        else
            al_draw_bitmap(img_settings, SCREEN_W - 48, 0, 0);
    }

    else if (active_scene == SCENE_START)
    {
        int i;
        al_draw_bitmap(img_bluesky, 0, 0, 0);
        al_draw_textf(font_earthorbiter_20, al_map_rgb(0, 0, 0), 0, 0, 0, "Lives: %d", plane.hp);
        al_draw_textf(font_earthorbiter_20, al_map_rgb(0, 0, 0), 0, 20, 0, "Score: %d", plane.score);
        al_draw_textf(font_earthorbiter_20, al_map_rgb(0, 0, 0), 0, 40, 0, "Kill Count: %d", kill_count);

        for (i = 0; i < MAX_BULLET; i++)
            draw_movable_object(bullets[i]);

        for (i = 0; i < MAX_ALIEN_LASER; i++)
            draw_movable_object(alien_laser[i]);

        draw_movable_object(plane);

        draw_movable_object(fuel);

        for (i = 0; i < MAX_BAG; i++)
            draw_movable_object(bag[i]);

        if (kill_count < 10)
        {
            for (i = 0; i < MAX_ENEMY; i++)
                draw_movable_object(enemies[i]);
        }

        if (kill_count >= 10 && kill_count < 15)
        {
            for (i = 0; i < MAX_ENEMY; i++)
                draw_movable_object(enemies1[i]);
        }

        if (kill_count >= 15 && kill_count < 18)
        {
            for (i = 0; i < MAX_ENEMY; i++)
                draw_movable_object(enemies2[i]);
        }

        if (kill_count >= 18)
        {
            draw_movable_object(bigboss);

            for (i = 0; i < MAX_ENEMY; i++)
                draw_movable_object(enemies[i]);

            for (int i = 0; i < MAX_BIGBOSS_BULLET; i++)
                draw_movable_object(bigboss_bullets[i]);

            draw_movable_object(laser);
        }
    }

    else if (active_scene == SCENE_SETTINGS)
    {
        al_draw_bitmap(img_background_settings, 0, 0, 0);
        al_draw_bitmap(img_base, 0, 0, 0);
        al_draw_text(font_death_star_70, al_map_rgb(255, 255, 255), SCREEN_W / 2, 30, ALLEGRO_ALIGN_CENTER, "Choose Your Music");
        al_draw_bitmap(img_electronic_fantasy, SCREEN_W / 8, 130, ALLEGRO_ALIGN_CENTER);
        al_draw_bitmap(img_interplanetary_odyssey, SCREEN_W / 8, 250, ALLEGRO_ALIGN_CENTER);
        al_draw_bitmap(img_interstellar_odyssey, SCREEN_W / 8, 370, ALLEGRO_ALIGN_CENTER);
        al_draw_text(font_over_there_70, al_map_rgb(255, 255, 255), SCREEN_W / 2, 550, ALLEGRO_ALIGN_CENTER, "Select a Character");
        al_draw_bitmap(img_arrow_character, SCREEN_W / 2 - 70, 600, ALLEGRO_ALIGN_CENTER);
    }

    else if (active_scene == SCENE_LOSE)
    {
        al_draw_bitmap(img_lose, 0, 0, 0);
        al_draw_text(font_stone_50, al_map_rgb(255, 255, 255), SCREEN_W / 2, 50, ALLEGRO_ALIGN_CENTER, "You Lose");
        al_draw_textf(font_darth_chowder_50, al_map_rgb(255, 255, 255), SCREEN_W / 2, 300, ALLEGRO_ALIGN_CENTER, "High Score: %d", plane.score);
        al_draw_text(font_darth_chowder_50, al_map_rgb(255, 255, 255), SCREEN_W / 2, 450, ALLEGRO_ALIGN_CENTER, "Click to Restart");
    }

    else if (active_scene == SCENE_WIN)
    {
        al_draw_bitmap(img_orangesky, 0, 0, 0);
        al_draw_text(font_stone_50, al_map_rgb(255, 255, 255), SCREEN_W / 2, 50, ALLEGRO_ALIGN_CENTER, "You Win");
        al_draw_textf(font_darth_chowder_50, al_map_rgb(255, 255, 255), SCREEN_W / 2, 300, ALLEGRO_ALIGN_CENTER, "High Score: %d", plane.score);
        al_draw_text(font_darth_chowder_50, al_map_rgb(255, 255, 255), SCREEN_W / 2, 370, ALLEGRO_ALIGN_CENTER, "Click to Restart");
    }

    else if (active_scene == SCENE_CHARACTER)
    {
        al_draw_bitmap(img_shelf, 0, 0, 0);
        al_draw_bitmap(img_base, 0, 0, 0);
        al_draw_text(font_over_there_70, al_map_rgb(255, 255, 255), SCREEN_W / 2, 150, ALLEGRO_ALIGN_CENTER, "Choose Your");
        al_draw_text(font_over_there_70, al_map_rgb(255, 255, 255), SCREEN_W / 2, 210, ALLEGRO_ALIGN_CENTER, "Plane");
        al_draw_bitmap(start_img_plane, 30, 570, 0);
        al_draw_bitmap(img_plane2, 225, 570, 0);
        al_draw_bitmap(img_plane3, 420, 570, 0);
        al_draw_bitmap(img_plane4, 615, 570, 0);
        al_draw_bitmap(img_plane5, 810, 570, 0);

        //hover
        //plane1
       /* if (pnt_in_rect(mouse_x, mouse_y, 30, 570, al_get_bitmap_width(start_img_plane), al_get_bitmap_height(start_img_plane)))
            al_draw_bitmap(start_img_plane, 430, 320, 0);

        //plane2
        if (pnt_in_rect(mouse_x, mouse_y, 225, 570, al_get_bitmap_width(img_plane2), al_get_bitmap_height(img_plane2)))
            al_draw_bitmap(img_plane2, 430, 320, 0);

        //plane3
        if (pnt_in_rect(mouse_x, mouse_y, 420, 570, al_get_bitmap_width(img_plane3), al_get_bitmap_height(img_plane3)))
            al_draw_bitmap(img_plane3, 430, 320, 0);

        //plane4
        if (pnt_in_rect(mouse_x, mouse_y, 615, 570, al_get_bitmap_width(img_plane4), al_get_bitmap_height(img_plane4)))
            al_draw_bitmap(img_plane4, 430, 320, 0);

        //plane5
        if (pnt_in_rect(mouse_x, mouse_y, 810, 570, al_get_bitmap_width(img_plane5), al_get_bitmap_height(img_plane5)))
            al_draw_bitmap(img_plane5, 430, 320, 0); */

            //click
        if (plane1 == 1)
        {
            al_draw_bitmap(start_img_plane, 430, 320, 0);
        }

        if (plane2 == 1)
        {
            al_draw_bitmap(img_plane2, 430, 320, 0);
        }

        if (plane3 == 1)
        {
            al_draw_bitmap(img_plane3, 430, 320, 0);
        }

        if (plane4 == 1)
        {
            al_draw_bitmap(img_plane4, 430, 320, 0);
        }

        if (plane5 == 1)
        {
            al_draw_bitmap(img_plane5, 430, 320, 0);
        }
    }

    if (active_scene == SCENE_INSTRUCTION1)
    {
        al_draw_bitmap(img_instruction1, 0, 0, 0);
        al_draw_bitmap(img_base, 0, 0, 0);
    }

    if (active_scene == SCENE_INSTRUCTION2)
    {
        al_draw_bitmap(img_instruction2, 0, 0, 0);
        al_draw_bitmap(img_base, 0, 0, 0);
    }

    al_flip_display();
}

void game_destroy(void)
{
    // Destroy everything you have created.
    // Free the memories allocated by malloc or allegro functions.
    // Destroy shared resources.
    al_destroy_font(font_pirulen_32);
    al_destroy_font(font_pirulen_24);
    al_destroy_font(font_earthorbiter_20);
    al_destroy_font(font_sega_50);
    al_destroy_font(font_death_star_70);
    al_destroy_font(font_over_there_70);
    al_destroy_font(font_darth_chowder_50);

    /* Menu Scene resources*/
    al_destroy_bitmap(main_img_background);
    al_destroy_sample(main_bgm);
    al_destroy_bitmap(img_settings);
    al_destroy_bitmap(img_settings2);
    al_destroy_bitmap(img_base);

    /* Start Scene resources*/
    al_destroy_bitmap(start_img_background);
    al_destroy_bitmap(start_img_plane);
    al_destroy_bitmap(start_img_enemy);
    al_destroy_sample(start_bgm);
    al_destroy_sample(audio_electronic_fantasy);
    al_destroy_sample(audio_interplanetary_odyssey);
    al_destroy_sample(audio_interstellar_odyssey);
    al_destroy_sample(audio_crash);
    al_destroy_sample(audio_shoot);
    al_destroy_sample(audio_money);
    al_destroy_sample(audio_fuel);
    al_destroy_sample(audio_bigboss_shoot);

    al_destroy_bitmap(img_bullet);
    al_destroy_bitmap(img_bigboss_bullet);
    al_destroy_bitmap(img_background_settings);
    al_destroy_bitmap(img_electronic_fantasy);
    al_destroy_bitmap(img_interplanetary_odyssey);
    al_destroy_bitmap(img_interstellar_odyssey);
    al_destroy_bitmap(img_lose);
    al_destroy_bitmap(img_fuel);
    al_destroy_bitmap(img_bag);
    al_destroy_bitmap(img_laser);
    al_destroy_bitmap(img_alien_laser);

    al_destroy_bitmap(img_plane2);
    al_destroy_bitmap(img_plane3);
    al_destroy_bitmap(img_plane4);
    al_destroy_bitmap(img_plane5);

    al_destroy_bitmap(img_enemies_lvl3);
    al_destroy_bitmap(img_enemies_lvl5);
    al_destroy_bitmap(img_bigboss);

    al_destroy_bitmap(img_bluesky);
    al_destroy_bitmap(img_orangesky);

    al_destroy_bitmap(img_instruction);
    al_destroy_bitmap(img_instruction1);
    al_destroy_bitmap(img_instruction2);


    al_destroy_timer(game_update_timer);
    al_destroy_event_queue(game_event_queue);
    al_destroy_display(game_display);
    free(mouse_state);
}

void game_change_scene(int next_scene)
{
    game_log("Change scene from %d to %d", active_scene, next_scene);
    // TODO: Destroy resources initialized when creating scene.
    if (active_scene == SCENE_MENU)
    {
        al_stop_sample(&main_bgm_id);
        game_log("stop audio (bgm)");
    }

    else if (active_scene == SCENE_START) //balik
    {
        al_stop_sample(&start_bgm_id);
        game_log("stop audio (bgm)");
    }

    else if (active_scene == SCENE_LOSE)
    {
        if (music_flag2 == 1)
        {
            al_stop_sample(&audio_electronic_fantasy_id);
            game_log("stop audio (bgm)");
        }

        if (music_flag3 == 1)
        {
            al_stop_sample(&audio_interplanetary_odyssey_id);
            game_log("stop audio (bgm)");
        }

        if (music_flag4 == 1)
        {
            al_stop_sample(&audio_interstellar_odyssey_id);
            game_log("stop audio (bgm)");
        }
    }

    else if (active_scene == SCENE_WIN)
    {
        if (music_flag2 == 1)
        {
            al_stop_sample(&audio_electronic_fantasy_id);
            game_log("stop audio (bgm)");
        }

        if (music_flag3 == 1)
        {
            al_stop_sample(&audio_interplanetary_odyssey_id);
            game_log("stop audio (bgm)");
        }

        if (music_flag4 == 1)
        {
            al_stop_sample(&audio_interstellar_odyssey_id);
            game_log("stop audio (bgm)");
        }
    }

    active_scene = next_scene;

    // TODO: Allocate resources before entering scene.
    if (active_scene == SCENE_MENU)
    {
        if (!al_play_sample(main_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &main_bgm_id))
            game_abort("failed to play audio (bgm)");
    }

    else if (active_scene == SCENE_START)
    {
        int i;

        if (plane1 == 1)
        {
            plane.score = 0; //starting score
            plane.hp = 3;
            plane.img = start_img_plane;
            plane.x = 400;
            plane.y = 500;
            plane.w = al_get_bitmap_width(plane.img);
            plane.h = al_get_bitmap_height(plane.img);
        }

        if (plane2 == 1)
        {
            plane.score = 0; //starting score
            plane.hp = 3;
            plane.img = img_plane2;
            plane.x = 400;
            plane.y = 500;
            plane.w = al_get_bitmap_width(plane.img);
            plane.h = al_get_bitmap_height(plane.img);
        }

        if (plane3 == 1)
        {
            plane.score = 0; //starting score
            plane.hp = 3;
            plane.img = img_plane3;
            plane.x = 400;
            plane.y = 500;
            plane.w = al_get_bitmap_width(plane.img);
            plane.h = al_get_bitmap_height(plane.img);
        }

        if (plane4 == 1)
        {
            plane.score = 0; //starting score
            plane.hp = 3;
            plane.img = img_plane4;
            plane.x = 400;
            plane.y = 500;
            plane.w = al_get_bitmap_width(plane.img);
            plane.h = al_get_bitmap_height(plane.img);
        }

        if (plane5 == 1)
        {
            plane.score = 0; //starting score
            plane.hp = 3;
            plane.img = img_plane5;
            plane.x = 400;
            plane.y = 500;
            plane.w = al_get_bitmap_width(plane.img);
            plane.h = al_get_bitmap_height(plane.img);
        }

        if (kill_count < 10)
        {
            for (i = 0; i < MAX_ENEMY; i++)
            {
                enemies[i].img = start_img_enemy;
                enemies[i].w = al_get_bitmap_width(start_img_enemy);
                enemies[i].h = al_get_bitmap_height(start_img_enemy);
                enemies[i].e_hp = 1;


                enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
                enemies[i].y = 80;

                enemies[i].vx = 0;
                enemies[i].vy = 2; //enemy's speed
            }
        }

        for (i = 0; i < MAX_ENEMY; i++)
        {
            enemies1[i].img = img_enemies_lvl3;
            enemies1[i].w = al_get_bitmap_width(img_enemies_lvl3);
            enemies1[i].h = al_get_bitmap_height(img_enemies_lvl3);
            enemies1[i].e_hp = 3;

            enemies1[i].x = enemies1[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies1[i].w);
            enemies1[i].y = 80;

            enemies1[i].vx = 0;
            enemies1[i].vy = 2.7; //enemy's speed
        }

        for (i = 0; i < MAX_ENEMY; i++)
        {
            enemies2[i].img = img_enemies_lvl5;
            enemies2[i].w = al_get_bitmap_width(img_enemies_lvl5);
            enemies2[i].h = al_get_bitmap_height(img_enemies_lvl5);
            enemies2[i].e_hp = 3;

            enemies2[i].x = enemies2[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies2[i].w);
            enemies2[i].y = 80;

            enemies2[i].vx = 0;
            enemies2[i].vy = 3.5; //enemy's speed
        }

        bigboss.img = img_bigboss;
        bigboss.w = al_get_bitmap_width(img_bigboss);
        bigboss.h = al_get_bitmap_height(img_bigboss);
        bigboss.e_hp = 35;

        bigboss.x = bigboss.w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - bigboss.w);
        bigboss.y = 80;

        bigboss.vx = 5;
        bigboss.vy = 0; //bigboss speed

        fuel.img = img_fuel;
        fuel.w = al_get_bitmap_width(img_fuel);
        fuel.h = al_get_bitmap_height(img_fuel);
        fuel.x = fuel.w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - fuel.w);
        fuel.y = fuel.h / 2 + (float)rand() / RAND_MAX * (SCREEN_H - fuel.h);

        fuel.vx = 0;
        fuel.vy = 2; //fuel's speed

        for (i = 0; i < MAX_BAG; i++)
        {
            bag[i].img = img_bag;
            bag[i].w = al_get_bitmap_width(img_bag);
            bag[i].h = al_get_bitmap_height(img_bag);
            bag[i].x = bag[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - bag[i].w);
            bag[i].y = bag[i].h / 2 + (float)rand() / RAND_MAX * (SCREEN_H - bag[i].h);

            bag[i].vx = 0;
            bag[i].vy = 1.3; //bag's speed
        }

        for (int i = 0; i < MAX_BULLET; i++)
        {
            bullets[i].w = al_get_bitmap_width(img_bullet);
            bullets[i].h = al_get_bitmap_height(img_bullet);
            bullets[i].img = img_bullet;
            bullets[i].vx = 0;
            bullets[i].vy = -10;
            bullets[i].hidden = true;
        }


        laser.w = al_get_bitmap_width(img_laser);
        laser.h = al_get_bitmap_height(img_laser);
        laser.img = img_laser;
        laser.vx = 0;
        laser.vy = -30;
        laser.hidden = true;

        for (int i = 0; i < MAX_BIGBOSS_BULLET; i++)
        {
            bigboss_bullets[i].w = al_get_bitmap_width(img_bigboss_bullet);
            bigboss_bullets[i].h = al_get_bitmap_height(img_bigboss_bullet);
            bigboss_bullets[i].img = img_bigboss_bullet;
            bigboss_bullets[i].vx = 0;
            bigboss_bullets[i].vy = 3; //speed of bigboss bullet
            bigboss_bullets[i].hidden = true;
        }

        for (i = 0; i < MAX_ALIEN_LASER; i++)
        {
            alien_laser[i].img = img_alien_laser;
            alien_laser[i].w = al_get_bitmap_width(img_alien_laser);
            alien_laser[i].h = al_get_bitmap_height(img_alien_laser);
            alien_laser[i].vx = 0;
            alien_laser[i].vy = 5;
            alien_laser[i].hidden = true;
        }

        if (!al_play_sample(start_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &start_bgm_id))
            game_abort("failed to play audio (bgm)");
    }
}

void on_key_down(int keycode)
{
    if (active_scene == SCENE_MENU)
    {
        if (keycode == ALLEGRO_KEY_ENTER)
        {
            game_change_scene(SCENE_START);
            plane.hp = 3;
            plane.score = 0;
            kill_count = 0;
            counter_bigboss = 10;
        }

    }

    if (active_scene == SCENE_INSTRUCTION1)
    {
        if (keycode == ALLEGRO_KEY_RIGHT)
        {
            game_change_scene(SCENE_INSTRUCTION2);
        }

    }

    if (active_scene == SCENE_INSTRUCTION2)
    {
        if (keycode == ALLEGRO_KEY_LEFT)
        {
            game_change_scene(SCENE_INSTRUCTION1);
        }

    }

    if (active_scene == SCENE_START)
    {
        if (keycode == ALLEGRO_KEY_1)
            kill_count = 18;

        if (keycode == ALLEGRO_KEY_2)
            kill_count = 15;

        if (keycode == ALLEGRO_KEY_3)
            kill_count = 10;

        if (keycode == ALLEGRO_KEY_5)
            game_change_scene(SCENE_WIN);

        if (keycode == ALLEGRO_KEY_7)
            game_change_scene(SCENE_LOSE);
    }

}

void on_mouse_down(int btn, int x, int y)
{
    if (active_scene == SCENE_MENU)
    {
        if (btn == true)
        {
            if (pnt_in_rect(x, y, SCREEN_W - 48, 10, 38, 38))
                game_change_scene(SCENE_SETTINGS);

            if (pnt_in_rect(x, y, 950, 50, al_get_bitmap_width(img_instruction), al_get_bitmap_height(img_instruction)))
                game_change_scene(SCENE_INSTRUCTION1);
        }
    }

    if (active_scene == SCENE_LOSE)
    {
        if (btn == true) //btn = button
        {
            if (pnt_in_rect(x, y, 0, 0, 1000, 800))
                game_change_scene(SCENE_MENU);
        }
    }

    if (active_scene == SCENE_WIN)
    {
        if (btn == true)
        {
            if (pnt_in_rect(x, y, 0, 0, 1000, 800))
                game_change_scene(SCENE_MENU);
        }
    }

    if (active_scene == SCENE_SETTINGS)
    {
        if (btn == true)
        {
            if (pnt_in_rect(x, y, SCREEN_W / 2 - 70, 600, al_get_bitmap_width(img_arrow_character), al_get_bitmap_height(img_arrow_character)))
                game_change_scene(SCENE_CHARACTER); //choose character

            if (pnt_in_rect(x, y, 0, 0, al_get_bitmap_width(img_base), al_get_bitmap_height(img_base)))
                game_change_scene(SCENE_MENU); //click base

            if (pnt_in_rect(x, y, SCREEN_W / 8, 130, al_get_bitmap_width(img_electronic_fantasy), al_get_bitmap_height(img_electronic_fantasy)))
            {
                if (!al_play_sample(audio_electronic_fantasy, 0.8, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &audio_electronic_fantasy_id))
                    game_abort("failed to play audio electronic fantasy (bgm)");

                al_stop_sample(&audio_interplanetary_odyssey_id);
                al_stop_sample(&audio_interstellar_odyssey_id);
                al_stop_sample(&start_bgm_id);
                game_log("stop audio (bgm)");
                music_flag2 = 1;
                music_flag1 = music_flag3 = music_flag4 = 0;
            }

            if (pnt_in_rect(x, y, SCREEN_W / 8, 250, al_get_bitmap_width(img_interplanetary_odyssey), al_get_bitmap_height(img_interplanetary_odyssey)))
            {
                if (!al_play_sample(audio_interplanetary_odyssey, 0.8, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &audio_interplanetary_odyssey_id))
                    game_abort("failed to play audio interplanetary odyssey (bgm)");

                al_stop_sample(&audio_electronic_fantasy_id);
                al_stop_sample(&audio_interstellar_odyssey_id);
                al_stop_sample(&start_bgm_id);
                game_log("stop audio (bgm)");
                music_flag3 = 1;
                music_flag1 = music_flag2 = music_flag4 = 0;
            }

            if (pnt_in_rect(x, y, SCREEN_W / 8, 370, al_get_bitmap_width(img_interstellar_odyssey), al_get_bitmap_height(img_interstellar_odyssey)))
            {
                if (!al_play_sample(audio_interstellar_odyssey, 0.8, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &audio_interstellar_odyssey_id))
                    game_abort("failed to play audio interstellar odyssey (bgm)");

                al_stop_sample(&audio_electronic_fantasy_id);
                al_stop_sample(&audio_interplanetary_odyssey_id);
                al_stop_sample(&start_bgm_id);
                game_log("stop audio (bgm)");
                music_flag4 = 1;
                music_flag1 = music_flag2 = music_flag3 = 0;
            }
        }
    }

    if (active_scene == SCENE_CHARACTER)
    {
        if (btn == true)
        {
            if (pnt_in_rect(x, y, 0, 0, al_get_bitmap_width(img_base), al_get_bitmap_height(img_base)))
                game_change_scene(SCENE_MENU);

            if (pnt_in_rect(x, y, 30, 570, al_get_bitmap_width(start_img_plane), al_get_bitmap_height(start_img_plane)))
            {
                plane1 = 1;
                plane2 = plane3 = plane4 = plane5 = 0;
                al_draw_bitmap(start_img_plane, 430, 320, 0);
            }

            if (pnt_in_rect(x, y, 225, 570, al_get_bitmap_width(img_plane2), al_get_bitmap_height(img_plane2)))
            {
                plane2 = 1;
                plane1 = plane3 = plane4 = plane5 = 0;
                al_draw_bitmap(img_plane2, 430, 320, 0);
            }

            if (pnt_in_rect(x, y, 420, 570, al_get_bitmap_width(img_plane3), al_get_bitmap_height(img_plane3)))
            {
                plane3 = 1;
                plane1 = plane2 = plane4 = plane5 = 0;
                al_draw_bitmap(img_plane3, 430, 320, 0);
            }

            if (pnt_in_rect(x, y, 615, 570, al_get_bitmap_width(img_plane4), al_get_bitmap_height(img_plane4)))
            {
                plane4 = 1;
                plane1 = plane3 = plane2 = plane5 = 0;
                al_draw_bitmap(img_plane4, 430, 320, 0);
            }

            if (pnt_in_rect(x, y, 810, 570, al_get_bitmap_width(img_plane5), al_get_bitmap_height(img_plane5)))
            {
                plane5 = 1;
                plane1 = plane3 = plane4 = plane2 = 0;
                al_draw_bitmap(img_plane5, 430, 320, 0);
            }
        }
    }

    if (active_scene == SCENE_INSTRUCTION1)
    {
        if (btn == true) //btn = button
        {
            if (pnt_in_rect(x, y, 0, 0, al_get_bitmap_width(img_base), al_get_bitmap_height(img_base)))
                game_change_scene(SCENE_MENU);
        }
    }

    if (active_scene == SCENE_INSTRUCTION2)
    {
        if (btn == true) //btn = button
        {
            if (pnt_in_rect(x, y, 0, 0, al_get_bitmap_width(img_base), al_get_bitmap_height(img_base)))
                game_change_scene(SCENE_MENU);
        }
    }
}

void draw_movable_object(MovableObject obj)
{
    if (obj.hidden)
        return;
    al_draw_bitmap(obj.img, round(obj.x - obj.w / 2), round(obj.y - obj.h / 2), 0);
}

ALLEGRO_BITMAP* load_bitmap_resized(const char* filename, int w, int h)
{
    ALLEGRO_BITMAP* loaded_bmp = al_load_bitmap(filename);
    if (!loaded_bmp)
        game_abort("failed to load image: %s", filename);
    ALLEGRO_BITMAP* resized_bmp = al_create_bitmap(w, h);
    ALLEGRO_BITMAP* prev_target = al_get_target_bitmap();

    if (!resized_bmp)
        game_abort("failed to create bitmap when creating resized image: %s", filename);
    al_set_target_bitmap(resized_bmp);
    al_draw_scaled_bitmap(loaded_bmp, 0, 0,
        al_get_bitmap_width(loaded_bmp),
        al_get_bitmap_height(loaded_bmp),
        0, 0, w, h, 0);
    al_set_target_bitmap(prev_target);
    al_destroy_bitmap(loaded_bmp);

    game_log("resized image: %s", filename);

    return resized_bmp;
}

bool pnt_in_rect(int px, int py, int x, int y, int w, int h)
{
    if (x <= px && px <= x + w && y <= py && py <= y + h)
        return 1;

    else
        return 0;
}

// +=================================================================+
// | Code below is for debugging purpose, it's fine to remove it.    |
// | Deleting the code below and removing all calls to the functions |
// | doesn't affect the game.                                        |
// +=================================================================+

void game_abort(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
    fprintf(stderr, "error occured, exiting after 2 secs");
    // Wait 2 secs before exiting.
    al_rest(2);
    // Force exit program.
    exit(1);
}

void game_log(const char* format, ...)
{
#ifdef LOG_ENABLED
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
#endif
}

void game_vlog(const char* format, va_list arg)
{
#ifdef LOG_ENABLED
    static bool clear_file = true;
    vprintf(format, arg);
    printf("\n");
    // Write log to file for later debugging.
    FILE* pFile = fopen("log.txt", clear_file ? "w" : "a");
    if (pFile)
    {
        vfprintf(pFile, format, arg);
        fprintf(pFile, "\n");
        fclose(pFile);
    }
    clear_file = false;
#endif
}
