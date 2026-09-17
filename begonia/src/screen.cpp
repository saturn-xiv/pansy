#include "pansy/screen.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include "ftxui/component/app.hpp"
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/dom/elements.hpp"

void pansy::Screen::render() const {
  // auto ssh_hosts_view = std::views::keys(this->_config._nodes);
  // std::vector<std::string> ssh_hosts{ssh_hosts_view.begin(),
  //                                    ssh_hosts_view.end()};

  // bool is_running = false;
  // std::string status_message = "状态: 未运行";

  // auto screen = ftxui::ScreenInteractive::TerminalOutput();
}
/*
void pansy::Screen::render() const {
  auto screen = ftxui::ScreenInteractive::TerminalOutput();


  int selected_env = 0;
  std::vector<std::string> env_options = {
      "生产环境 (Production)", "测试环境 (Testing)", "开发环境 (Development)"};
  bool show_dropdown = false;  // 控制下拉菜单的展开/折叠

  // 密码输入框的状态
  std::string password;
  ftxui::InputOption input_option;
  input_option.password = true;  // 设置为密码模式（隐藏明文）

  // 启动/停止按钮的状态
  bool is_running = false;
  std::string status_message = "状态: 未运行";

  // ---- 组件定义 ----
  // 密码输入组件
  auto input_password = ftxui::Input(&password, "请输入密码...", input_option);

  // 下拉选择组件（使用 Menu 来模拟下拉内容）
  auto menu_dropdown = ftxui::Menu(&env_options, &selected_env);

  // 自定义下拉框按钮：点击时切换展开状态
  auto btn_dropdown_toggle =
      ftxui::Button("", [&]() { show_dropdown = !show_dropdown; });

  // 启动/停止按钮：点击时切换运行状态
  auto btn_toggle_run = ftxui::Button("", [&]() {
    is_running = !is_running;
    if (is_running) {
      status_message = "状态: 正在运行 [" + env_options[selected_env] + "] ...";
    } else {
      status_message = "状态: 已停止";
    }
  });

  // ---- 布局与渲染 ----
  // 将所有需要交互的组件组合成一个容器
  auto container = ftxui::Container::Vertical({
      btn_dropdown_toggle,
      menu_dropdown,
      input_password,
      btn_toggle_run,
  });

  // 动态渲染界面
  auto renderer = ftxui::Renderer(container, [&]() {
    // 动态修改按钮的文本标签
    btn_dropdown_toggle->SetActiveChild(ftxui::Renderer([&]() {
      return ftxui::text(env_options[selected_env] + (show_dropdown ? "  ▲" : "
▼"));
    }));

    btn_toggle_run->SetActiveChild(ftxui::Renderer(
        [&]() { return ftxui::text(is_running ? " 🛑 停止 " : " ▶ 启动 "); }));

    // 构造下拉框的 UI 样式
    ftxui::Element dropdown_ui = ftxui::vbox({
        ftxui::text("目标环境:"),
        btn_dropdown_toggle->Render() | border |
ftxui::bgcolor(ftxui::Color::GrayDark),
    });

    // 如果下拉框处于展开状态，则将菜单渲染在下方
    if (show_dropdown) {
      dropdown_ui = ftxui::vbox({dropdown_ui, menu_dropdown->Render() |
ftxui::border | ftxui::bgcolor(ftxui::Color::BlueDark)});
    }

    // 构造整体表单布局
    return ftxui::vbox({
               ftxui::text(" FTXUI 跨平台 TUI 表单示例 ") | ftxui::bold |
ftxui::hcenter, ftxui::separator(),

               // 下拉选择框部分
               dropdown_ui,
               ftxui::vbox().size(ftxui::HEIGHT, ftxui::EQUAL, 1),  // 空行

               // 密码输入框部分
               ftxui::text("安全密码:"),
               input_password->Render() | border,
               ftxui::vbox().size(ftxui::HEIGHT, ftxui::EQUAL, 1),  // 空行

               // 按钮与状态显示部分
               ftxui::hbox({btn_toggle_run->Render() | border,
                     ftxui::vbox().size(ftxui::WIDTH, ftxui::EQUAL, 2),
                     ftxui::text(status_message) | ftxui::bold | ftxui::center |
                         ftxui::color(is_running ? ftxui::Color::Green :
ftxui::Color::Red)}),
           }) |
           ftxui::borderStyled(ftxui::BorderStyle::ROUNDED)  // 外层圆角边框
           | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 50)            // 固定宽度
           | ftxui::center;                           // 居中显示
  });

  // 2. 启动事件主循环
  screen.Loop(renderer);
}
*/
