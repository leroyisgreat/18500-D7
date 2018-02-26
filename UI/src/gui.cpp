#include "gui.h"
#include <iostream>

Gui::Gui()
: l1_box(Gtk::ORIENTATION_VERTICAL, 4),
  l2_box_top(Gtk::ORIENTATION_HORIZONTAL, 4),
  l2_box_bottom(Gtk::ORIENTATION_HORIZONTAL, 4),
  l3_box_left(Gtk::ORIENTATION_VERTICAL, 4),
  m_button1("Button 1"),
  m_button2("Button 2")
{
  // set title of new window.
  set_title("18-500 Team D7 GUI");
  // set border width of the window.
  set_border_width(10);
  set_default_size(800,600);

  // add highest level container (vertical box)
  add(l1_box);

  // add second level containers (two horizontal boxes)
  l1_box.pack_start(l2_box_top, true, true);
  l1_box.pack_start(l2_box_bottom, false, false);

  // add left panel to top l2 box
  l2_box_top.pack_start(l3_box_left, false, false);

  // add viewfinder to top l2 box
  l2_box_top.pack_start(l3_viewfinder, true, true);


  // Now when the button is clicked, we call the "on_button_clicked" function
  // with a pointer to "button 1" as it's argument
  m_button1.signal_clicked().connect(sigc::bind<Glib::ustring>(
              sigc::mem_fun(*this, &Gui::on_button_clicked), "button 1"));

  // instead of gtk_container_add, we pack this button into the invisible
  // box, which has been packed into the window.
  // note that the pack_start default arguments are Gtk::EXPAND | Gtk::FILL, 0
  l2_box_bottom.pack_start(m_button1);

  // always remember this step, this tells GTK that our preparation
  // for this button is complete, and it can be displayed now.
  m_button1.show();

  // call the same signal handler with a different argument,
  // passing a pointer to "button 2" instead.
  m_button2.signal_clicked().connect(sigc::bind<-1, Glib::ustring>(
              sigc::mem_fun(*this, &Gui::on_button_clicked), "button 2"));
  
  l3_box_left.pack_start(m_button2);


  show_all_children();
}

Gui::~Gui() {}

// Our new improved signal handler.  The data passed to this method is
// printed to stdout.
void Gui::on_button_clicked(Glib::ustring data)
{
  std::cout << "Hello World - " << data << " was pressed" << std::endl;
}

