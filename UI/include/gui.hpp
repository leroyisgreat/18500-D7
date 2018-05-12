/**
 * @file    gui.hpp
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

#ifndef GUI_HPP
#define GUI_HPP

#include "camera_mode.hpp"
#include <gtkmm.h>
#include "viewfinder.hpp"

class Gui : public Gtk::Window {
public:
  Gui();
  virtual ~Gui();

private:
  // Signal handlers:
  bool on_capture(GdkEventButton *event);
  void on_exposure_change();
  void on_file_chooser_is();
  void on_file_chooser_pan();
  void on_iso_change();
  void on_mode_change(CameraMode mode);
  void on_off();
  void on_save();
  void on_selection_changed();

  // mode functions
  void set_mode(CameraMode mode);
  void hdr();
  void gallery();

  // helper functions
  void populate_toolbar();
  void add_entry(const std::string& filename, bool is_vid = false);
  Gtk::FileChooserDialog create_dialog(Glib::RefPtr<Gtk::FileFilter> filter);

  /** 
   * @brief simple function to preface prints with more information
   *
   * @param input information string to be printed
   */
  inline void print(std::string input) {
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

  // Tree model columns:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
      ModelColumns() {
        add(col_filename);
        add(col_pixbuf);
        add(col_is_vid);
      }

      Gtk::TreeModelColumn<std::string> col_filename;
      Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf>> col_pixbuf;
      Gtk::TreeModelColumn<bool> col_is_vid;
  };

  ModelColumns model_columns;

  // Child widgets:
  Gtk::Box                         l1_box;
  Gtk::Box                         l2_box_top;
  Gtk::Toolbar                     l2_toolbar;
  Gtk::Stack                       l3_stack;
  Viewfinder                       l3_viewfinder;
  Gtk::Box                         l4_options_VIDEO;
  Gtk::Box                         l4_options_STILL;
  Gtk::Box                         l4_options_HDR;
  Gtk::Box                         l4_options_PANORAMA;
  Gtk::Box                         l4_options_GALLERY;
  Gtk::Box                         l4_options_STABILIZE;
  Gtk::Button                      save_SC, save_HDR, save_VID;
  Gtk::Button                      file_chooser_PAN, file_chooser_IS;
  Glib::RefPtr<Gtk::Adjustment>    adjustment_exposure, adjustment_iso;
  Gtk::SpinButton                  exposure, iso;
  Gtk::Label                       exposure_label, iso_label;
  Gtk::ScrolledWindow              scrolled_window;
  Gtk::TreeView                    icon_view;
  Glib::RefPtr<Gtk::ListStore>     list_model;
  Glib::RefPtr<Gtk::TreeSelection> icon_view_selection;

	/** @brief camera Mode information */
	CameraMode current_mode;

  /** @brief current user home directory */
  std::string HOME_PATH;

  /** @brief location of all resource images */
  const std::string IMG_RESOURCE_PATH = "/workspace/18500-D7/UI/resources/";

  /** @brief location of all manually saved images */
  const std::string IMG_SAVE_PATH = "/workspace/18500-D7/saves/";

  /** @brief location of a refrence image placeholder */
  const std::string VID_ICO_PATH = "/workspace/18500-D7/UI/resources/stock_video.png";

  /** @brief location of panorama stitching app */
  const std::string PANO_PATH = "/workspace/18500-D7/panorama/";

  /** @brief location of hdr app */
  const std::string HDR_PATH = "/workspace/18500-D7/hdr/";

  /** @brief location of image stabilization app */
  const std::string IS_PATH = "/workspace/18500-D7/ImageStabilization/";
};

#endif // GUI_HPP
