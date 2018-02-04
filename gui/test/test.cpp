#include <gtkmm.h>

int main(int argc, char* argv[]){
    auto app =
        Gtk::Application::create(argc, argv,
                "org.gtkmm.example.base");

    Gtk::Window win;
    win.set_default_size(320, 480);

    return app->run(win);
}
