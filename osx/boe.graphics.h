
void adjust_window_mode();
void plop_fancy_startup();
void fancy_startup_delay();
void init_startup();
void reload_startup();
void init_animation();
void next_animation_step() ;
void draw_startup(short but_type);
void draw_anim();
void place_anim(); 
void draw_start_button(short which_position,short which_button);
void main_button_click(short mode,Rect button_rect);
void arrow_button_click(Rect button_rect);
void end_startup();
void Set_up_win ();
void load_main_screen();
void create_clip_region();
void set_gworld_fonts(short font_num);
void redraw_screen();
void put_background();
void draw_main_screen();
//void refresh_screen(short mode);
void draw_buttons(short mode);
void draw_text_area(short mode);
void reset_text_bar();
void draw_text_bar(short mode);
void put_text_bar(char *str);
void load_area_graphics();
void add_to_wish_list(short which_g);
bool reserve_graphic_num_in_array(short which_g);
void place_graphic_num_in_array(short which_g);
//GWorldPtr load_bmp(unsigned char *data, long length);
void add_one_graphic(short which_g);
void add_terrain_to_wish_list(unsigned short ter);
void load_outdoor_graphics();
void add_monst_graphic(unsigned short m,short mode);
void load_town_graphics();
void update_pc_graphics();
void put_graphics_in_template();
//GWorldPtr load_pict(short picture_to_get);
void draw_terrain(short	mode = 0);
void place_trim(short q,short r,location where,unsigned short ter_type);
void draw_trim(short q,short r,short which_trim,short which_mode);
bool extend_road_terrain(unsigned short ter);
void place_road(short q,short r,location where,bool here);
void draw_rest_screen();
void boom_space(location where,short mode,short type,short damage,short sound);
void draw_pointing_arrows() ;
void redraw_terrain();
void draw_targets(location center);
void frame_space(location where,short mode,short width,short height);
void erase_spot(short i,short j);
void undo_clip();
void draw_targeting_line(Point where_curs);
bool party_toast();
void redraw_partial_terrain(Rect redraw_rect);
void dump_gworld();
//void final_process_dialog(short which_dlog);
bool is_nature(char i, char j, unsigned char ground_t);
void put_dialog_graphic(short graphic_num,short spec_g,Rect draw_rect);
void draw_startup_stats();
void HideMenuBar( void );
void ShowMenuBar( void );
void FlushAndPause(unsigned long ticks);
void draw_trim(short q,short r,short which_trim,unsigned short ground_ter);

unsigned short get_ground_from_ter(unsigned short ter);
unsigned short get_ter_from_ground(unsigned char ground);

void draw_startup_anim();
short cd_create_custom_dialog(WindowPtr parent, Str255 strs[6],short pic_num,short buttons[3]);
void init_dialogs();
