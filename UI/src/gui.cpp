#include "gui.h"
#include <iostream>

Gui::Gui()
: l2_viewfinder("16x9", /* label */
    Gtk::ALIGN_CENTER,  /* align x */
    Gtk::ALIGN_CENTER,  /* align y */
    1.77,               /* xsize/ysize (16x9) */
    false               /* ignore child's aspect */),
  m_button1("Button 1"),
  m_button2("Button 2")
{
  // set title of new window.
  set_title("18-500 Team D7 GUI");
  // set border width of the window.
  set_border_width(10);
  set_default_size(800,600);

  // add highest level container
  add(l1_grid);

  // add viewfinder as Aspect Frame
  l1_grid.add(l2_viewfinder);
  // let viewfinder panel grow vertically & horizontally with window
  l2_viewfinder.set_hexpand();
  l2_viewfinder.set_vexpand();
  // add a stock photo to the viewfinder
  // TODO


  // add left panel
  l1_grid.attach_next_to(
      l2_box_left,    // child to attach
      l2_viewfinder,  // the child already existing to place relatively
      Gtk::POS_LEFT,  // how the child will relate to the existing
      2,              // width
      8               // height
      );   
  // let left panel grow vertically with window
  l2_box_left.set_vexpand();

  // add bottom panel
  l1_grid.attach_next_to(
      l2_box_bottom,  // child to attach
      l2_box_left,  // the child already existing to place relatively
      Gtk::POS_BOTTOM,// how the child will relate to the existing
      8,              // width
      4               // height
      );   
  // let bottom panel grow horizontally with window
  l2_box_bottom.set_hexpand();

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
  
  l2_box_left.pack_start(m_button2);

  // Show the widgets.
  // They will not really be shown until this Window is shown.
  //m_button2.show();
  //m_box1.show();

  show_all_children();
}

Gui::~Gui()
{
}

// Our new improved signal handler.  The data passed to this method is
// printed to stdout.
void Gui::on_button_clicked(Glib::ustring data)
{
  std::cout << "Hello World - " << data << " was pressed" << std::endl;
}

