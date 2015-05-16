#include "pebble.h"
#include "libs/pebble-assist.h"
#include "elements.h"

static void update_bg(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  
#ifdef PBL_COLOR
  bat = battery_state_service_peek().charge_percent / 10;
#endif

#ifdef PBL_COLOR
  if (bat >= 7) {
    graphics_context_set_fill_color(ctx, GColorIslamicGreen);
  }
  else if (bat >= 4) {
    graphics_context_set_fill_color(ctx, GColorIcterine);
  }
  else {
    graphics_context_set_fill_color(ctx, GColorRed);
  }
#else 
    graphics_context_set_fill_color(ctx, GColorBlack);
#endif
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
}

static void update_time(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  
  time_t epoch = time(NULL); 
  struct tm *t = localtime(&epoch);
  
  int16_t hour = (t->tm_hour) % 12;
  int16_t min = t->tm_min;
  
  int16_t hourX = 74;
  int16_t hourY = 22;
  
  while (hour >= 1) {
    GRect bigsquare = GRect(hourX, hourY, 28, 28);
    
    graphics_context_set_fill_color(ctx, GColorWhite);
#ifdef PBL_COLOR
    if (bat >= 4 && bat <= 6) {
      graphics_context_set_fill_color(ctx, GColorBlack);
    }
#endif
    graphics_fill_rect(ctx, bigsquare, 0, GCornerNone);
    
    if (hourX < 106 && hourY == 22) {
      hourX += 32;
    }
    else if (hourX == 106 && hourY < 118) {
      hourY += 32;
    }
    else if (hourY == 118 && hourX > 10) {
      hourX -= 32;
    }
    else if (hourX == 10 && hourY > 22) {
      hourY -= 32;
    }
    else {
      hourX = 42;
    }
    
    hour -= 1;
  }
  
  int16_t minX = 74;
  int16_t minY = 54;
  
  while (min >= 1) {
    GRect smallsquare = GRect(minX, minY, 12, 12);
    
    graphics_context_set_fill_color(ctx, GColorWhite);
#ifdef PBL_COLOR
    if (bat >= 4 && bat <= 6) {
      graphics_context_set_fill_color(ctx, GColorBlack);
    }
#endif
    graphics_fill_rect(ctx, smallsquare, 0, GCornerNone);
    
    if (minX < 90 && minY == 54) {
      minX += 16;
    }
    else if (minX == 90 && minY < 102) {
      minY += 16;
    }
    else if (minY == 102 && minX > 42) {
      minX -= 16;
    }
    else if (minX == 42 && minY > 54) {
      minY -= 16;
    }
    else {
      minX = 58;
    }
    
    min -= 5;
  }
  
  strftime(s_day_buffer, sizeof(s_day_buffer), "%d", t);
  text_layer_set_text(s_day_label, s_day_buffer);
  
#ifdef PBL_COLOR
  if (bat >= 4 && bat <= 6) {
    text_layer_set_colors(s_day_label, GColorBlack, GColorClear);
  }
  else {
    text_layer_set_colors(s_day_label, GColorWhite, GColorClear);
  }
#endif
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_time_layer);
  layer_mark_dirty(s_background_layer);
}

static void main_window_load(Window *window) {
  GRect bounds = window_get_bounds(window);
  
  s_day_font = fonts_load_resource_font(RESOURCE_ID_BLACKOUT_34);
  
  s_background_layer = layer_create(bounds);
  s_time_layer = layer_create(bounds);
  
  s_day_label = text_layer_create(GRect(59,64,40,40));
  
  text_layer_set_colors(s_day_label, GColorWhite, GColorClear);
#ifdef PBL_COLOR
  if (bat >= 4 && bat <= 6) {
    text_layer_set_colors(s_day_label, GColorBlack, GColorClear);
  }
#endif
  
  text_layer_set_font(s_day_label, s_day_font);
  
  layer_set_update_proc(s_background_layer, update_bg);
  layer_set_update_proc(s_time_layer, update_time);
  
  layer_add_to_window(s_background_layer, window);
  layer_add_to_window(s_time_layer, window);
  
  text_layer_add_to_layer(s_day_label, s_time_layer);
}

static void main_window_unload(Window *window) {
  layer_destroy_safe(s_time_layer);
  layer_destroy_safe(s_background_layer);
  
  text_layer_destroy_safe(s_day_label);
  
  fonts_unload_custom_font(s_day_font);
}

static void init() {
  s_main_window = window_create();
  window_handlers(s_main_window, main_window_load, main_window_unload);
  window_stack_push(s_main_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy_safe(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}