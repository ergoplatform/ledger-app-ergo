#pragma once

/**
 * Show main menu (ready screen, version, about, quit).
 */
void ui_menu_main(void);

#ifdef HAVE_BAGL
/**
 * Show settings submenu (blind signing toggle).
 */
void ui_menu_settings(void);
#endif

/**
 * Show about submenu (copyright, date).
 */
void ui_menu_about(void);
