/**
 * @file    gui.h
 *
 * @brief   top level class for manaing GTK UI elements
 *
 *  ----------------------------------------------
 *  | window                                     |
 *  |                                            |
 *  | ------------------------------------------ |
 *  | | l1_box                                 | |
 *  | |                                        | |
 *  | | -------------------------------------- | |
 *  | | | l2_box_top                         | | |
 *  | | |                                    | | |
 *  | | | ---------------  ----------------- | | |
 *  | | | | l3_box_left |  | l3_viewfinder | | | |
 *  | | | |             |  |               | | | |
 *  | | | |             |  |               | | | |
 *  | | | ---------------  ----------------- | | |
 *  | | -------------------------------------- | |
 *  | |                                        | |
 *  | | -------------------------------------- | |
 *  | | | m_Toolbar                          | | |
 *  | | -------------------------------------- | |
 *  | ------------------------------------------ |
 *  ----------------------------------------------
 *
 * @date    2018/02/25
 * @author  LeRoy Gary <lgary@andrew.cmu.edu>
 */

#ifndef GUI_H 
#define GUI_H

#include "camera_mode.h"
#include <gtkmm.h>
#include "viewfinder.h"

class Gui : public Gtk::Window {
public:
  Gui();
  virtual ~Gui();

private:
  // Signal handlers:
  void on_mode_change(CameraMode mode);
  void on_exposure_change();
  void on_iso_change();
  void on_save();
  void on_next_gallery();
  void on_off();

  // regular functions
  void populate_toolbar();
  void set_current_mode(CameraMode mode);
  void hdr();
  void gallery();

  /** 
   * @brief simple function to preface prints with more information
   *
   * @param input information string to be printed
   */
  inline void print(const char *input) {
    std::cout << "GUI: " << input << std::endl;
  }

  /** 
   * @brief simple function to easily throw meaningful errors
   *
   * @param info information string to be printed
   * @param err exception to be thrown
   */
  inline void error(const char *err, const char *info) {
    std::cout << "[E] GUI: " << info << std::endl;
    std::string error_str("GUI: ");
    error_str += err;
    throw std::runtime_error(error_str);
  }

  // Child widgets:
  Gtk::Box                      l1_box;
  Gtk::Box                      l2_box_top;
  Gtk::Toolbar                  l2_toolbar;
  Gtk::Stack                    l3_stack;
  Viewfinder                    l3_viewfinder;
  Gtk::Box                      l4_options_CONTINUOUS;
  Gtk::Box                      l4_options_SINGLE_CAPTURE;
  Gtk::Box                      l4_options_HDR;
  Gtk::Box                      l4_options_GALLERY;
  Gtk::Button                   save_SC, save_HDR;
  Gtk::Button                   next_G;
  Glib::RefPtr<Gtk::Adjustment> adjustment_exposure, adjustment_iso;
  Gtk::SpinButton               exposure, iso;
  Gtk::Label                    exposure_label, iso_label;

	/** @brief camera Mode information */
	CameraMode current_mode;

  /** @brief location of all resource images */
  const std::string IMG_RESOURCE_PATH = "/home/pi/workspace/18500-D7/UI/resources/";

  /** @brief location of all manually saved images */
  const std::string IMG_SAVE_PATH = "/home/pi/workspace/18500-D7/saves/";

  std::vector<const char*> saved_files;
};

#endif // GUI_H
