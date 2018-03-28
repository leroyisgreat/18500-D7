#include "gui.h"
#include <gtkmm.h>
#include <glibmm.h>
#include <iostream>

int main (int argc, char *argv[]) {
  auto app = Gtk::Application::create(argc, argv, "org.gtkmm.examples.base");

  // call to implicit constructor
  // this took me a minute to wrap my head around
  Gui gui;

  // shows the window and returns when it is closed.
  return app->run(gui);
}
