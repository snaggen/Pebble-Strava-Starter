#include <pebble.h>
#include "main.h"
#include "libs/bitmap-loader/bitmap-loader.h"
#include "libs/pebble-assist/pebble-assist.h"

static void layer_action_bar_click_config_provider(void *context);
static void action_bar_layer_select_handler(ClickRecognizerRef recognizer, void *context);
static void action_bar_layer_up_handler(ClickRecognizerRef recognizer, void *context);
static void action_bar_layer_down_handler(ClickRecognizerRef recognizer, void *context);
	
static Window* window;

static TextLayer* text_layer;
static Layer* layer;
static ActionBarLayer* action_bar;

enum {
	STRAVA_TOGGLE = 5,
	STRAVA_START = 1,
	STRAVA_STOP = 2,
};


void pebble_strava_show(void) {
  window_stack_push(window, true);
}

void pebble_strava_destroy(void) {
  action_bar_layer_destroy(action_bar);
  layer_destroy(layer);
  window_destroy(window);
}

static void layer_action_bar_click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_UP, action_bar_layer_up_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, action_bar_layer_down_handler);
	//window_single_click_subscribe(BUTTON_ID_SELECT, action_bar_layer_select_handler);
}

static void sent_callback(DictionaryIterator *iter, void *context) {
	  APP_LOG(APP_LOG_LEVEL_DEBUG,"MyStrava App Message Sent!");
 }

static void action_bar_layer_select_handler(ClickRecognizerRef recognizer, void *context) {
	//spedire comando ad app android
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	uint x = STRAVA_TOGGLE;
	Tuplet value = TupletInteger(1, x);
  dict_write_tuplet(iter, &value);
	app_message_outbox_send();
	APP_LOG(APP_LOG_LEVEL_DEBUG,"Pebble Strava Select Up handler!");
	
	window_stack_pop_all(true);
}

static void action_bar_layer_up_handler(ClickRecognizerRef recognizer, void *context) {
	//spedire comando ad app android
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	uint x = STRAVA_START;
	Tuplet value = TupletInteger(1, x);
  dict_write_tuplet(iter, &value);
	app_message_outbox_send();
	APP_LOG(APP_LOG_LEVEL_DEBUG,"Pebble Strava Starter Up handler!");
	
	window_stack_pop_all(true);
}

static void action_bar_layer_down_handler(ClickRecognizerRef recognizer, void *context) {
	//spedire comando ad app android
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	uint x = STRAVA_STOP;
	Tuplet value = TupletInteger(1, x);
  dict_write_tuplet(iter, &value);
	app_message_outbox_send();
	APP_LOG(APP_LOG_LEVEL_DEBUG,"Pebble Strava Starter Down handler!");
	
	window_stack_pop_all(true);
}

//! Program main entry point.
int main(void) {
	pebble_strava_init();
  app_event_loop();
	pebble_strava_deinit();
}

void pebble_strava_init() {
	app_message_register_outbox_sent(sent_callback);
	app_message_open(256, 256);
	bitmaps_init();
	
  window = window_create();
  layer = layer_create_fullscreen(window);
  layer_add_to_window(layer, window);

  action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar, window);
  action_bar_layer_set_click_config_provider(action_bar,layer_action_bar_click_config_provider);
  //action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, bitmaps_get_bitmap(RESOURCE_ID_ICON_OK));
  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, bitmaps_get_bitmap(RESOURCE_ID_ICON_PLAY));
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, bitmaps_get_bitmap(RESOURCE_ID_ICON_STOP));
	
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	text_layer = text_layer_create((GRect) { .origin = { 0, 50 }, .size = { bounds.size.w - 20, bounds.size.h/2 } });
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_text(text_layer, "Strava Starter");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
	
	pebble_strava_show();
}

//! App deinitialise functions
void pebble_strava_deinit() {
  pebble_strava_destroy();
	bitmaps_cleanup();
}
