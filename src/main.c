#include <pebble.h>
#include "main.h"
#include "gbitmap_color_palette_manipulator.h"

#define DEBUG true
	
#ifdef PBL_COLOR
	#define BACKGROUND_COLOR GColorBlack
	#define TEXT_COLOR GColorWhite
	#define ACTIONBAR_COLOR GColorOrange
	#define ICON_COLOR GColorWhite
#endif
	
static void layer_action_bar_click_config_provider(void *context);
static void action_bar_layer_select_handler(ClickRecognizerRef recognizer, void *context);
static void action_bar_layer_up_handler(ClickRecognizerRef recognizer, void *context);
static void action_bar_layer_down_handler(ClickRecognizerRef recognizer, void *context);
	
static Window* window;
static GBitmap *image_all,*img_bike,*img_run,*img_stop;
static TextLayer *text_layer, *gps_text_layer;
static Layer* layer;
static ActionBarLayer* action_bar;

enum {
	GPS_COOR = 6,
	GPS_ERR = 0,
	STRAVA_TOGGLE = 5,
	STRAVA_START_RIDE = 1,
	STRAVA_START_RUN = 3,
	STRAVA_STOP = 2,
};

void pebble_strava_show(void) {
  window_stack_push(window, true);
}

void pebble_strava_destroy(void) {
	text_layer_destroy(gps_text_layer);
  text_layer_destroy(text_layer);
  action_bar_layer_destroy(action_bar);
  layer_destroy(layer);
  window_destroy(window);
}

static void layer_action_bar_click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_UP, action_bar_layer_up_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, action_bar_layer_down_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, action_bar_layer_select_handler);
}

static void received_handler(DictionaryIterator *iter, void *context){
	Tuple *tuple_log = dict_read_first(iter);
	if (DEBUG) APP_LOG(APP_LOG_LEVEL_DEBUG,"Pebble Strava Starter Message Received: %lu",tuple_log->key);
	
	if (tuple_log->key == GPS_COOR){
		Tuple *tuple = dict_find(iter, GPS_COOR);
		if (tuple && tuple->type == TUPLE_CSTRING) {
			if (DEBUG) APP_LOG(APP_LOG_LEVEL_DEBUG, "GPS: %s", tuple->value->cstring);
			text_layer_set_text(gps_text_layer, tuple->value->cstring);
			
			vibes_long_pulse();
		}
  } else if (tuple_log->key == GPS_ERR){
		Tuple *tuple = dict_find(iter, GPS_ERR);
		if (tuple && tuple->type == TUPLE_CSTRING) {
			if (DEBUG) APP_LOG(APP_LOG_LEVEL_DEBUG, "GPS_ERROR: %s", tuple->value->cstring);
			text_layer_set_text(gps_text_layer, tuple->value->cstring);
		
			static const uint32_t segments[] = { 70, 50, 70, 50, 70 };
			VibePattern pat = {
				.durations = segments,
				.num_segments = ARRAY_LENGTH(segments),
			};
			vibes_enqueue_custom_pattern(pat);
		}
	}
}

static void sent_callback(DictionaryIterator *iter, void *context) {
	if (DEBUG) APP_LOG(APP_LOG_LEVEL_DEBUG,"Pebble Strava Starter Message Sent!");
	window_stack_pop_all(true);
 }

static void outbox_failed_handler(DictionaryIterator *iter, AppMessageResult reason, void *context) {
  //text_layer_set_text(s_output_layer, "Error!");
  if (DEBUG) APP_LOG(APP_LOG_LEVEL_ERROR, "Fail reason: %d", (int)reason);
}

static void action_bar_layer_select_handler(ClickRecognizerRef recognizer, void *context) {
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	uint x = STRAVA_START_RUN;
	Tuplet value = TupletInteger(1, x);
  dict_write_tuplet(iter, &value);
	int res = app_message_outbox_send();
	if (DEBUG) APP_LOG(APP_LOG_LEVEL_DEBUG,"Pebble Strava Select Select handler! RESULT:%d", res);
}

static void action_bar_layer_up_handler(ClickRecognizerRef recognizer, void *context) {
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	uint x = STRAVA_START_RIDE;
	Tuplet value = TupletInteger(1, x);
  dict_write_tuplet(iter, &value);
	//if (DEBUG) APP_LOG(APP_LOG_LEVEL_DEBUG,"%d",dict_calc_buffer_size_from_tuplet(value,1));
	app_message_outbox_send();
	if (DEBUG) APP_LOG(APP_LOG_LEVEL_DEBUG,"Pebble Strava Starter Up handler!");
}

static void action_bar_layer_down_handler(ClickRecognizerRef recognizer, void *context) {
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	uint x = STRAVA_STOP;
	Tuplet value = TupletInteger(1, x);
  dict_write_tuplet(iter, &value);
	app_message_outbox_send();
	if (DEBUG) APP_LOG(APP_LOG_LEVEL_DEBUG,"Pebble Strava Starter Down handler!");
}

//! Program main entry point.
int main(void) {
	pebble_strava_init();
  app_event_loop();
	pebble_strava_deinit();
}

void pebble_strava_init() {
	app_message_register_inbox_received(received_handler);
	app_message_register_outbox_sent(sent_callback);
	app_message_register_outbox_failed(outbox_failed_handler);
	app_message_open(256, 256);
	
  window = window_create();
	
	layer = layer_create(layer_get_bounds(window_get_root_layer(window)));
	layer_add_child(window_get_root_layer(window), layer);
	
	image_all = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ALL);
	img_bike = gbitmap_create_as_sub_bitmap(image_all, GRect(0,0,16,16) );
	img_run = gbitmap_create_as_sub_bitmap(image_all, GRect(0,16,16,16) );
	img_stop = gbitmap_create_as_sub_bitmap(image_all, GRect(0,32,16,16) );

  action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar, window);
  action_bar_layer_set_click_config_provider(action_bar,layer_action_bar_click_config_provider);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, img_run);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, img_bike);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, img_stop);
	
	#ifdef PBL_COLOR
		action_bar_layer_set_background_color(action_bar, ACTIONBAR_COLOR);
		replace_gbitmap_color(GColorWhite, ACTIONBAR_COLOR, image_all, NULL);
		replace_gbitmap_color(GColorBlack, ICON_COLOR, image_all, NULL);
	
		window_set_background_color(window, BACKGROUND_COLOR);
	#endif
	
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	text_layer = text_layer_create((GRect) { .origin = { 0, (bounds.size.h/2)-13 }, .size = { bounds.size.w - ACTION_BAR_WIDTH, 26 } });
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_text(text_layer, "Strava Starter");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	text_layer_set_background_color(text_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
	
	gps_text_layer = text_layer_create((GRect) { .origin = { 0, bounds.size.h - 18 }, .size = { bounds.size.w - ACTION_BAR_WIDTH, 18 } });
	text_layer_set_font(gps_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	//text_layer_set_text(gps_text_layer, "GPS alignment...");
  text_layer_set_text_alignment(gps_text_layer, GTextAlignmentCenter);
	text_layer_set_background_color(gps_text_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(gps_text_layer));
	
	#ifdef PBL_COLOR
		text_layer_set_text_color(text_layer, TEXT_COLOR);
		text_layer_set_text_color(gps_text_layer, TEXT_COLOR);
	#endif
	pebble_strava_show();
}

//! App deinitialise functions
void pebble_strava_deinit() {
  pebble_strava_destroy();
	animation_unschedule_all();
	
	gbitmap_destroy(image_all);
	gbitmap_destroy(img_run);
	gbitmap_destroy(img_bike);
	gbitmap_destroy(img_stop);
	image_all = NULL;
	img_run = NULL;
	img_bike = NULL;
	img_stop = NULL;
}
