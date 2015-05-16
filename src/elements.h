#define TEMPERATURE 0

static Window *s_main_window;
static Layer *s_background_layer, *s_box_layer;
static TextLayer *s_day_label;

static GFont s_day_font;

static char s_day_buffer[] = "XX";

int8_t bat;
int16_t temp;