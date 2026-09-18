#include "pansy/screen.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include "ftxui/component/app.hpp"
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/dom/elements.hpp"

static ftxui::Component _window(const std::string& title,
                                ftxui::Component component) {
  return ftxui::Renderer(component, [component, title] {
    return ftxui::window(ftxui::text(title), component->Render()) | ftxui::flex;
  });
}

void pansy::Screen::render() const {
  const uint16_t PORT = 8000;
  const auto nodes = this->_server.nodes();
  const auto lines_view = std::views::keys(nodes);
  const std::vector<std::string> line_options{lines_view.begin(),
                                              lines_view.end()};
  std::vector<std::string> port_options;
  for (int i = 0; i < 10; i++) {
    const std::string it = std::format("{}", PORT + i);
    port_options.push_back(it);
  }

  bool is_running = false;
  int line_selected = 0;
  int port_selected = 0;
  std::string password;

  {
    auto screen = ftxui::App::TerminalOutput();

    auto line_box = ftxui::Menu(&line_options, &line_selected);
    auto port_box = ftxui::Menu(&port_options, &port_selected);
    auto exit_box = ftxui::Button("Start", screen.ExitLoopClosure());

    ftxui::InputOption password_input_option;
    password_input_option.password = true;
    password_input_option.multiline = false;
    auto password_box = ftxui::Input(&password, "Type your password here...",
                                     password_input_option);

    auto settings_pane = ftxui::Container::Horizontal(
        {_window("Line", line_box), _window("Port", port_box),
         _window("Password", password_box)});

    auto status_box = ftxui::Renderer([&] {
      const std::string message =
          std::format("listen {} on http://0.0.0.0:{}",
                      line_options[line_selected], (PORT + port_selected));

      return ftxui::window(ftxui::text("Status"),
                           ftxui::vbox({ftxui::text(message)})) |
             ftxui::flex;
    });

    auto control_pane = ftxui::Container::Horizontal({status_box, exit_box});
    auto global_pane =
        ftxui::Container::Vertical({settings_pane, control_pane});
    screen.Loop(global_pane);
  }

  this->_server.startup(line_options[line_selected], "0.0.0.0",
                        PORT + port_selected, password);
}
