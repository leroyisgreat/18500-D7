// thanks https://git.gnome.org//browse/gstreamermm/tree/examples/media_player_gtkmm/player_window.cc

#include "gui.hpp"
#include "exceptions.hpp"
#include <iostream>
#include <gdk/gdkx.h>
#include <opencv2/opencv.hpp>
#include <python2.7/Python.h>
#include "boost/filesystem.hpp"

namespace {
  class IconEntry {
  public:
    IconEntry()
    {}

    IconEntry(const std::string& filename)
    : m_filename(filename)
    {}

    const std::string m_filename;
  };
}

// PUBLIC METHODS {{{
Gui::Gui()
: l1_box(              Gtk::ORIENTATION_VERTICAL,  4),
  l2_box_top(          Gtk::ORIENTATION_HORIZONTAL,4),
  l4_options_VIDEO(    Gtk::ORIENTATION_VERTICAL,  4),
  l4_options_STILL(    Gtk::ORIENTATION_VERTICAL,  4),
  l4_options_HDR(      Gtk::ORIENTATION_VERTICAL,  4),
  l4_options_PANORAMA( Gtk::ORIENTATION_VERTICAL,  4),
  l4_options_GALLERY(  Gtk::ORIENTATION_VERTICAL,  4),
  l4_options_STABILIZE(Gtk::ORIENTATION_VERTICAL,  4),
  save_SC("Save"),
  save_HDR("Save"),
  save_VID("Save"),
  save_PAN("Save"),
  save_IS("Save"),
  file_chooser_PAN("Select File(s)"),
  adjustment_exposure(Gtk::Adjustment::create(50.0, 1.0, 100.0, 5.0, 10.0, 0.0)),
  adjustment_iso(Gtk::Adjustment::create(50.0, 1.0, 100.0, 5.0, 10.0, 0.0)),
  exposure(adjustment_exposure),
  iso(adjustment_iso),
  exposure_label("Exposure [1-100]", Gtk::ALIGN_START),
  iso_label("ISO [1-100]", Gtk::ALIGN_START)
{
  // set title of new window.
  set_title("18-500 Team D7 GUI");
  // set window characteristics.
  set_border_width(10);
  set_default_size(800,600);

  const char *home = std::getenv("HOME");
  if (home == NULL) {
    error(Exceptions::PATH_ERROR, "Error getting user home path");
  } else {
    HOME_PATH = std::string(home);
  }

  // ASSEMBLE GUI {{{
  // add highest level container (vertical box)
  add(l1_box);

  // add second level containers (two horizontal boxes)
  l1_box.pack_start(l2_box_top, true, true);
  l1_box.pack_start(l2_toolbar, false, false);
  Gui::populate_toolbar();

  // add left panel to top l2 box
  l2_box_top.pack_start(l3_stack, false, false);
  l3_stack.set_size_request(200);

  // add viewfinder to top l2 box
  l2_box_top.pack_start(l3_viewfinder, true, true);
  l3_viewfinder.set_events(Gdk::BUTTON_PRESS_MASK);
  l3_viewfinder.signal_button_press_event().connect(
          sigc::mem_fun(*this, &Gui::on_capture));

  // add control options for each mode
  l3_stack.add(l4_options_VIDEO, "Video options");
  l4_options_VIDEO.pack_start(save_VID);
  save_VID.signal_clicked().connect(sigc::mem_fun(*this, &Gui::on_save));

  l3_stack.add(l4_options_STILL, "Still Capture options");
  l4_options_STILL.pack_start(save_SC);
  l4_options_STILL.pack_start(exposure_label);
  l4_options_STILL.pack_start(exposure);
  l4_options_STILL.pack_start(iso_label);
  l4_options_STILL.pack_start(iso);
  save_SC.signal_clicked().connect(sigc::mem_fun(*this, &Gui::on_save));
  exposure.signal_changed().connect(sigc::mem_fun(*this, &Gui::on_exposure_change));
  iso.signal_changed().connect(sigc::mem_fun(*this, &Gui::on_iso_change));

  l3_stack.add(l4_options_HDR, "HDR options");
  l4_options_HDR.pack_start(save_HDR);
  save_HDR.signal_clicked().connect(sigc::mem_fun(*this, &Gui::on_save));

  l3_stack.add(l4_options_PANORAMA, "Panorama options");
  l4_options_PANORAMA.pack_start(save_PAN);
  l4_options_PANORAMA.pack_start(file_chooser_PAN);
  save_PAN.signal_clicked().connect(sigc::mem_fun(*this, &Gui::on_save));
  file_chooser_PAN.signal_clicked().connect(sigc::mem_fun(*this, &Gui::on_file_chooser_pan));

  l3_stack.add(l4_options_STABILIZE, "Image Stabilization options");
  l4_options_STABILIZE.pack_start(save_IS);
  l4_options_STABILIZE.pack_start(file_chooser_IS);
  save_IS.signal_clicked().connect(sigc::mem_fun(*this, &Gui::on_save));
  file_chooser_PAN.signal_clicked().connect(sigc::mem_fun(*this, &Gui::on_file_chooser_is));

  l3_stack.add(l4_options_GALLERY, "Gallery options");
  l4_options_GALLERY.pack_start(scrolled_window);
  scrolled_window.add(icon_view);

	list_model = Gtk::ListStore::create(m_Columns);
	list_model->set_sort_column( m_Columns.m_col_filename, Gtk::SortType::SORT_ASCENDING);

  icon_view.set_model(list_model);
	icon_view.set_pixbuf_column(m_Columns.m_col_pixbuf);
  icon_view.set_columns(1);
  // }}}

  // set visibility to all children to be seen
  show_all_children();
  // Switch to the still camera mode
  on_mode_change(CameraMode::STILL);

  // PYTHON SETUP {{{
  // Initializing Python environment
  print("Initializing Python environment");
  Py_Initialize();
  PyRun_SimpleString("import os");
  PyRun_SimpleString("import sys");
  // append script folder to PATH so imports work properly
  PyRun_SimpleString("sys.path.append('/home/pi/workspace/18500-D7/hdr/')");
  // }}}

  print("Setup finished.");
}

Gui::~Gui() {
  // destroy Python environment
  Py_Finalize();
}
// }}}

// PRIVATE METHODS {{{
// SIGNAL HANDLERS {{{
bool Gui::on_capture(GdkEventButton *event) {
  switch (current_mode) {
    case CameraMode::STILL:
      l3_viewfinder.set_frame(l3_viewfinder.get_frame(true));
      l3_viewfinder.set_mode(ViewfinderMode::CAPTURE);
      l3_viewfinder.queue_draw();
      break;
    case CameraMode::VIDEO:
      if (l3_viewfinder.get_mode() == ViewfinderMode::VIDEO_RECORD) {
        l3_viewfinder.stop_writer();
        l3_viewfinder.set_mode(ViewfinderMode::STREAM);
      } else {
        std::stringstream ss;
        ss << HOME_PATH;
        ss << IMG_SAVE_PATH;
        ss << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        ss << ".avi";
        l3_viewfinder.start_writer(ss.str());
        l3_viewfinder.set_mode(ViewfinderMode::VIDEO_RECORD);
      }
      break;
    case CameraMode::HDR:
      hdr();
      break;
    case CameraMode::PANORAMA:
      break;
    case CameraMode::GALLERY:
      gallery();
      break;
    default:
      error(Exceptions::GUI_UNKOWN_MODE, 
            "GUI entered unknown camera mode in on_capture()");
  }
  return true;;
}

void Gui::on_exposure_change() {
  print("Changing Exposure...");
  l3_viewfinder.set_property(
      CV_CAP_PROP_AUTO_EXPOSURE,0);
  l3_viewfinder.set_property(
      CV_CAP_PROP_EXPOSURE, exposure.get_value_as_int());
}

void Gui::on_iso_change() {
  print("Changing ISO...");
  l3_viewfinder.set_property(
      CV_CAP_PROP_GAIN, iso.get_value_as_int());
}

void Gui::on_mode_change(CameraMode mode) {
  print("Changing Camera mode");

  Gui::set_mode(mode);
  l3_viewfinder.set_mode(ViewfinderMode::STREAM);

  switch (mode) {
    case CameraMode::STILL:
      l3_stack.set_visible_child(l4_options_STILL);
      l3_viewfinder.hud_info = "📷";
      break;
    case CameraMode::VIDEO:
      l3_stack.set_visible_child(l4_options_VIDEO);
      l3_viewfinder.hud_info = "📹";
      break;
    case CameraMode::HDR:
      l3_stack.set_visible_child(l4_options_HDR);
      hdr();
      break;
    case CameraMode::PANORAMA:
      l3_stack.set_visible_child(l4_options_PANORAMA);
      break;
    case CameraMode::STABILIZE:
      l3_stack.set_visible_child(l4_options_STABILIZE);
      break;
    case CameraMode::GALLERY:
      l3_stack.set_visible_child(l4_options_GALLERY);
      l3_viewfinder.get_frame(true);
      gallery();
      break;
    default:
      error(Exceptions::GUI_UNKOWN_MODE, 
            "GUI entered unknown camera mode in on_mode_change()");
  }
}

void Gui::on_save() {
  ViewfinderMode mode = l3_viewfinder.get_mode();
  if (mode == ViewfinderMode::CAPTURE) {
    std::stringstream ss;
    ss << HOME_PATH;
    ss << IMG_SAVE_PATH;
    ss << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    ss << ".jpg";
    cv::Mat frame = l3_viewfinder.get_frame();
    cv::imwrite(ss.str().c_str(), frame);
    l3_viewfinder.set_mode(ViewfinderMode::STREAM);
  } else if (mode == ViewfinderMode::VIDEO_RECORD) {
    l3_viewfinder.stop_writer();
    l3_viewfinder.set_mode(ViewfinderMode::VIDEO_CAPTURE);
  }
}

void Gui::on_off() {
  Gtk::Main::quit();
}

void Gui::on_file_chooser_pan() {
  Gtk::FileChooserDialog dialog("Please choose a file",
          Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_select_multiple(true);
  dialog.set_current_folder(HOME_PATH + IMG_SAVE_PATH);
  dialog.set_transient_for(*this);

  //Add response buttons the the dialog:
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Open", Gtk::RESPONSE_OK);

  //Add filters, so that only certain file types can be selected:
  auto filter_images = Gtk::FileFilter::create();
  filter_images->set_name("images");
  filter_images->add_mime_type("image/jpeg");
  filter_images->add_mime_type("image/png");
  dialog.add_filter(filter_images);

  auto filter_any = Gtk::FileFilter::create();
  filter_any->set_name("Any files");
  filter_any->add_pattern("*");
  dialog.add_filter(filter_any);

  //Show the dialog and wait for a user response:
  int result = dialog.run();

  //Handle the response:
  switch(result) {
    case Gtk::RESPONSE_OK: {
      print("Open clicked.");

      //Notice that this is a std::string, not a Glib::ustring.
      std::vector<std::string> filenames = dialog.get_filenames();
      std::stringstream ss;
      ss << HOME_PATH;
      ss << PANO_PATH;
      ss << "opencv_stitching.app ";
      for (auto it = filenames.begin(); it != filenames.end(); ++it) {
        ss << *it;
        ss << " ";
      }
      ss << "--output ";
      ss << HOME_PATH;
      ss << IMG_SAVE_PATH;
      ss << "pano.jpg";
      print(ss.str().c_str());
      system(ss.str().c_str());

      cv::Mat image = cv::imread(HOME_PATH + IMG_SAVE_PATH + "pano.jpg");
      l3_viewfinder.set_frame(image);
      l3_viewfinder.set_mode(ViewfinderMode::CAPTURE);
      
      break;
    } case Gtk::RESPONSE_CANCEL: {
      print("Cancel clicked.");
      break;
    } default:
      print("Unexpected button clicked.");
      break;
  }
}

void Gui::on_file_chooser_is() {
  Gtk::FileChooserDialog dialog("Please choose a file",
          Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_current_folder(HOME_PATH + IMG_SAVE_PATH);
  dialog.set_transient_for(*this);

  //Add response buttons the the dialog:
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Open", Gtk::RESPONSE_OK);

  //Add filters, so that only certain file types can be selected:
  auto filter_vid = Gtk::FileFilter::create();
  filter_vid->set_name("video");
  filter_vid->add_mime_type("video/x-msvideo");
  dialog.add_filter(filter_vid);

  auto filter_any = Gtk::FileFilter::create();
  filter_any->set_name("Any files");
  filter_any->add_pattern("*");
  dialog.add_filter(filter_any);

  //Show the dialog and wait for a user response:
  int result = dialog.run();

  //Handle the response:
  switch(result) {
    case Gtk::RESPONSE_OK: {
      print("Open clicked.");

      //Notice that this is a std::string, not a Glib::ustring.
      std::string filename = dialog.get_filename();

      // run the script
      std::stringstream ss;
      ss << HOME_PATH;
      ss << IS_PATH;
      ss << "python_video_stab.py --video ";
      ss << filename;
      ss << " --output ";
      ss << HOME_PATH;
      ss << IMG_SAVE_PATH;
      ss << "is.avi";
      const char *is_app = ss.str().c_str();
      FILE *file = fopen(hdr_app, "r");

      PyRun_SimpleFile(file, is_app);
      fclose(file);

      // get the result back and dsiplay
      ss.clear();
      ss << HOME_PATH;
      ss << IMG_SAVE_PATH;
      ss << "is.jpg";
      cv::Mat image = cv::imread(ss.str().c_str());
      l3_viewfinder.set_frame(image);

      std::stringstream ss;
      ss << HOME_PATH;
      ss << PANO_PATH;
      ss << "opencv_stitching.app ";
      ss << "--output ";

      ss << HOME_PATH;
      ss << IMG_SAVE_PATH;
      ss << "pano.jpg";
      print(ss.str().c_str());
      system(ss.str().c_str());

      cv::Mat image = cv::imread(HOME_PATH + IMG_SAVE_PATH + "pano.jpg");
      l3_viewfinder.set_frame(image);
      l3_viewfinder.set_mode(ViewfinderMode::CAPTURE);
      
      break;
    } case Gtk::RESPONSE_CANCEL: {
      print("Cancel clicked.");
      break;
    } default:
      print("Unexpected button clicked.");
      break;
  }
}
// }}}

// MODE FUNCTIONS {{{
void Gui::set_mode(CameraMode mode) {
  current_mode = mode;
}

void Gui::hdr() {
  // let go of the camera to allow python script to take control
  l3_viewfinder.uninitialize_camera();

  // run the script
  std::stringstream ss;
  ss << HOME_PATH;
  ss << HDR_PATH;
  ss << "runhdrpi.py";
  const char *hdr_app = ss.str().c_str();
  FILE *file = fopen(hdr_app, "r");

  ss.clear();
  ss << "os.chdir('";
  ss << HOME_PATH;
  ss << HDR_PATH;
  ss << "')";
  const char *py_str;
  // change working directory so calls and file operations work properly
  PyRun_SimpleString(py_str);
  PyRun_SimpleFile(file, hdr_app);
  fclose(file);

  // get the result back and dsiplay
  ss.clear();
  ss << HOME_PATH;
  ss << HDR_PATH;
  ss << "output.jpg";
  cv::Mat image = cv::imread(ss.str().c_str());
  l3_viewfinder.set_frame(image);

  // re-take camera
  l3_viewfinder.initialize_camera();
}

void Gui::gallery() {
  print("Populating Gallery...");
  list_model->clear();
  std::string location = HOME_PATH + IMG_SAVE_PATH;
  print(location.c_str());

  if(!boost::filesystem::exists(location) || 
     !boost::filesystem::is_directory(location))
    error(Exceptions::PATH_ERROR,"Error when populating gallery");

  boost::filesystem::recursive_directory_iterator it(location);
  boost::filesystem::recursive_directory_iterator endit;

  const std::string ext1(".jpg");
  const std::string ext2(".avi");

  std::stringstream ss;
  std::string s;
	while(it != endit) {
    if(boost::filesystem::is_regular_file(*it) && 
       it->path().extension() == ext1) {
      s = it->path().string();
      const char* filename = s.c_str();
      ss << filename;
      ss << " ";
      add_entry(filename);
    }
    ++it;
  }

  print(ss.str().c_str());
}
// }}}

// HELPER FUNCTIONS {{{
void Gui::populate_toolbar() {
  // create the photo capture button
  auto pc_image = new Gtk::Image(
      HOME_PATH + IMG_RESOURCE_PATH + "shoot.ico");
  auto pc_button = new Gtk::ToolButton(*pc_image, "shoot");
  pc_button->set_tooltip_text("Take photo");
  // link photo capture button to function
  pc_button->signal_clicked().connect(sigc::bind<CameraMode>(
      sigc::mem_fun(*this, &Gui::on_mode_change),
      CameraMode::STILL));
  l2_toolbar.append(*pc_button);

  // create the photo capture button
  auto live_image = new Gtk::Image(
      HOME_PATH + IMG_RESOURCE_PATH + "live.ico");
  auto live_button = new Gtk::ToolButton(*live_image, "live");
  live_button->set_tooltip_text("Video Mode");
  // link photo capture button to change camera mode 
  live_button->signal_clicked().connect(sigc::bind<CameraMode>(
      sigc::mem_fun(*this, &Gui::on_mode_change),
      CameraMode::VIDEO));
  l2_toolbar.append(*live_button);

  // create the HDR mode button
  auto hdr_image = new Gtk::Image(
      HOME_PATH + IMG_RESOURCE_PATH + "hdr.ico");
  auto hdr_button = new Gtk::ToolButton(*hdr_image, "hdr");
  hdr_button->set_tooltip_text("Toggle HDR mode");
  hdr_button->signal_clicked().connect(sigc::bind<CameraMode>(
      sigc::mem_fun(*this, &Gui::on_mode_change),
      CameraMode::HDR));
  l2_toolbar.append(*hdr_button);

  // create the Panorama mode button
  auto panorama_image = new Gtk::Image(
      HOME_PATH + IMG_RESOURCE_PATH + "panorama.ico");
  auto panorama_button = new Gtk::ToolButton(*panorama_image, "panorama");
  panorama_button->set_tooltip_text("Toggle Panorama mode");
  panorama_button->signal_clicked().connect(sigc::bind<CameraMode>(
      sigc::mem_fun(*this, &Gui::on_mode_change),
      CameraMode::PANORAMA));
  l2_toolbar.append(*panorama_button);

  // create the Stabilize mode button
  auto stabilize_image = new Gtk::Image(
      HOME_PATH + IMG_RESOURCE_PATH + "stabilize.ico");
  auto stabilize_button = new Gtk::ToolButton(*stabilize_image, "stabilize");
  stabilize_button->set_tooltip_text("Toggle Image Stabilization mode");
  stabilize_button->signal_clicked().connect(sigc::bind<CameraMode>(
      sigc::mem_fun(*this, &Gui::on_mode_change),
      CameraMode::STABILIZE));
  l2_toolbar.append(*stabilize_button);

  // create the gallery button
  auto gallery_image = new Gtk::Image(
      HOME_PATH + IMG_RESOURCE_PATH + "gallery.ico");
  auto gallery_button = new Gtk::ToolButton(*gallery_image, "gallery");
  gallery_button->set_tooltip_text("Toggle Gallery mode");
  gallery_button->signal_clicked().connect(sigc::bind<CameraMode>(
      sigc::mem_fun(*this, &Gui::on_mode_change),
      CameraMode::GALLERY));
  l2_toolbar.append(*gallery_button);

  // create the OFF button
  auto off_image = new Gtk::Image(
      HOME_PATH + IMG_RESOURCE_PATH + "off.ico");
  auto off_button = new Gtk::ToolButton(*off_image, "off");
  off_button->set_tooltip_text("Turn camera OFF");
  off_button->signal_clicked().connect(sigc::mem_fun(*this, &Gui::on_off));
  l2_toolbar.append(*off_button);
}

void Gui::add_entry(const std::string& filename) {
  auto row = *(list_model->append());
  row[m_Columns.m_col_filename] = filename;

  try
  {
    auto p = Gdk::Pixbuf::create_from_file(filename);
    int w = p->get_width();
    int h = p->get_height();
    double scale = 100.0 / w;
    row[m_Columns.m_col_pixbuf] = p->scale_simple(w*scale,h*scale,Gdk::INTERP_BILINEAR);
  }
  catch (const Gdk::PixbufError& ex)
  {
    std::cerr << "Gdk::PixbufError: " << ex.what() << std::endl;
  }
  catch (const Glib::FileError& ex)
  {
    std::cerr << "Glib::FileError: " << ex.what() << std::endl;
  }
}
// }}}
// }}}
