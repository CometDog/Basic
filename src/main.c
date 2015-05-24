#include "pebble.h"
#ifdef PBL_COLOR
	#include "gcolor_definitions.h"
#endif
#include "libs/pebble-assist.h"
#include "elements.h"

static void do_animation() {
	
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *t = dict_read_first(iterator);

  while(t != NULL) {
    switch(t->key) {
    case COUNTRY:
      country = (int)t->value->int8;
      break;
    case TEMPERATURE:
      if (country == 0) {
        degree = ((int)t->value->int16) + 10;
      }
      else if (country == 1) {
        degree = ((int)(32 + (((int)t->value->int16) * 1.8))) + 10;
      }
      break;
    case CONDITIONS:
		conditions = t->value->cstring;
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "] %d not recognized!", (int)t->key);
      break;
    }

    t = dict_read_next(iterator);
  }
  
  snprintf(s_weather_buffer, sizeof(s_weather_buffer), "%s, %d°", conditions, degree);
  LOG("Conditions are %s", conditions);

  text_layer_set_text(s_weather_label, s_weather_buffer);
  
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void update_battery(Layer* layer, GContext *ctx) {
	bat = battery_state_service_peek().charge_percent / 10;
	#ifdef PBL_COLOR
		if (bat >= 7) {
			window_set_background_color(s_main_window, GColorIslamicGreen);
		}
		else if (bat >= 4) {
			window_set_background_color(s_main_window, GColorYellow);
			text_layer_set_colors(s_weather_label, GColorBlack, GColorClear);
			text_layer_set_colors(s_time_label, GColorBlack, GColorClear);
			text_layer_set_colors(s_date_label, GColorBlack, GColorClear);
		}
		else {
			window_set_background_color(s_main_window, GColorDarkCandyAppleRed);
		}
	#else
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_fill_rect(ctx, GRect(0,163,bat * 14.4, 5), GCornerNone, 0);
	#endif
}

static void update_time(Layer *layer, GContext *ctx) {
	time_t epoch = time(NULL);
	struct tm *t = localtime(&epoch);
	
	if (clock_is_24h_style() == true) {
		print_time(s_time_buffer, "%H:%M", t);
	}
	else {
		print_time(s_time_buffer, "%I:%M", t);
	}
	print_time(s_date_buffer, "%a, %b %d", t);
	
	text_layer_set_text(s_time_label, s_time_buffer);
	text_layer_set_text(s_date_label, s_date_buffer);
	
	layer_set_update_proc(s_info_layer, update_battery);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	layer_mark_dirty(window_get_root_layer(s_main_window));
}

static void bt_handler(bool connected) {
	if (connected) {
		vibes_short_pulse();
	}
	else {
		vibes_double_pulse();
	}
}

static void main_window_load(Window *window) {
	GRect bounds = window_get_bounds(window);
	
	window_set_background_color(window, GColorBlack);
	#ifdef PBL_COLOR
		window_set_background_color(window, GColorIslamicGreen);
	#endif
	
	s_info_layer = layer_create(bounds);
	
	s_weather_label = text_layer_create(GRect(0,36,144,30));
	s_time_label = text_layer_create(GRect(0,52,144,50));
	s_date_label = text_layer_create(GRect(0,104,144,30));
	
	text_layer_set_colors(s_weather_label, GColorWhite, GColorClear);
	text_layer_set_colors(s_time_label, GColorWhite, GColorClear);
	text_layer_set_colors(s_date_label, GColorWhite, GColorClear);
	
	bat = battery_state_service_peek().charge_percent / 10;
	#ifdef PBL_COLOR
		if (bat >= 7) {
			window_set_background_color(s_main_window, GColorIslamicGreen);
		}
		else if (bat >= 4) {
			window_set_background_color(s_main_window, GColorYellow);
			text_layer_set_colors(s_weather_label, GColorBlack, GColorClear);
			text_layer_set_colors(s_time_label, GColorBlack, GColorClear);
			text_layer_set_colors(s_date_label, GColorBlack, GColorClear);
		}
		else {
			window_set_background_color(s_main_window, GColorDarkCandyAppleRed);
		}
	#endif
	
	text_layer_set_system_font(s_weather_label, FONT_KEY_ROBOTO_CONDENSED_21);
	text_layer_set_system_font(s_time_label, FONT_KEY_ROBOTO_BOLD_SUBSET_49);
	text_layer_set_system_font(s_date_label, FONT_KEY_ROBOTO_CONDENSED_21);
	
	text_layer_set_text(s_weather_label, "Loading...");
	
	text_layer_set_text_alignment(s_weather_label, GTextAlignmentCenter);
	text_layer_set_text_alignment(s_time_label, GTextAlignmentCenter);
	text_layer_set_text_alignment(s_date_label, GTextAlignmentCenter);
	
	layer_set_update_proc(s_info_layer, update_battery);
	layer_set_update_proc(s_info_layer, update_time);
	
	layer_add_to_window(s_info_layer, window);
	
	text_layer_add_to_layer(s_weather_label, s_info_layer);
	text_layer_add_to_layer(s_time_label, s_info_layer);
	text_layer_add_to_layer(s_date_label, s_info_layer);
}

static void main_window_unload(Window *window) {
	layer_destroy_safe(s_info_layer);
	
	text_layer_destroy_safe(s_weather_label);
	text_layer_destroy_safe(s_time_label);
	text_layer_destroy_safe(s_date_label);
}

static void init() {
	s_main_window = window_create();
	window_handlers(s_main_window, main_window_load, main_window_unload);
	window_stack_push(s_main_window, true);
	
	app_message_register_inbox_received(inbox_received_callback);
  	app_message_register_inbox_dropped(inbox_dropped_callback);
  	app_message_register_outbox_failed(outbox_failed_callback);
  	app_message_register_outbox_sent(outbox_sent_callback);
  	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	bluetooth_connection_service_subscribe(bt_handler);
}

static void deinit() {
	animation_unschedule_all();
	bluetooth_connection_service_unsubscribe();
	window_destroy_safe(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}