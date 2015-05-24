#define COUNTRY 0
#define TEMPERATURE 1
#define CONDITIONS 2

#define ANIM_DURATION 1000
#define ANIM_DELAY 0

static Window *s_main_window;
static Layer *s_info_layer;
static TextLayer *s_weather_label, *s_time_label, *s_date_label;

static PropertyAnimation *s_animation;

static int8_t bat;
static int8_t country;
static int16_t degree;
static char *conditions;

static char s_weather_buffer[32];
static char s_time_buffer[] = "XX:XX";
static char s_date_buffer[] = "XXX, XXX XX";

static void timer_callback(void *data);