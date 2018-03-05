from pprint import pprint

import gi
gi.require_version('Gtk', '3.0')
gi.require_version('Gst', '1.0')
gi.require_version('GstVideo', '1.0')

from gi.repository import Gtk, Gst
Gst.init(None)
Gst.init_check(None)


class GstWidget(Gtk.Box):
    def __init__(self, pipeline):
        super().__init__()
        self.connect('realize', self._on_realize)
        self._bin = Gst.parse_bin_from_description('videotestsrc', True)

    def _on_realize(self, widget):
        pipeline = Gst.Pipeline()
        factory = pipeline.get_factory()
        gtksink = factory.make('gtksink')
        pipeline.add(gtksink)
        pipeline.add(self._bin)
        self._bin.link(gtksink)
        self.pack_start(gtksink.props.widget, True, True, 0)
        gtksink.props.widget.show()
        pipeline.set_state(Gst.State.PLAYING)


window = Gtk.ApplicationWindow()

header_bar = Gtk.HeaderBar()
header_bar.set_show_close_button(True)
window.set_titlebar(header_bar)  # Place 2
