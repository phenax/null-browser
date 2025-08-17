#include <optional>
#include <vector>

#include "Configuration.hpp"
#include "LuaRuntime.hpp"
#include "WindowActionRouter.hpp"
#include "testUtils.h"
#include "widgets/BrowserWindow.hpp"

// NOLINTBEGIN
class WindowActionRouterSpec : public QObject {
  Q_OBJECT

  class WindowActionRouterPub : public WindowActionRouter {
  public:
    using WindowActionRouter::WindowActionRouter, WindowActionRouter::get_relevant_windows;
  };

private slots:
  void beforeTestCase() { LuaRuntime::instance().start_event_loop(); }
  void cleanupTestCase() { LuaRuntime::instance().stop_event_loop(); }

  void test_add_window() {
    describe("#add_window");

    it("adds new window with incrementing assigned ids") {
      WindowActionRouterPub window_action_router;
      Configuration configuration;
      window_action_router.initialize(&configuration);

      BrowserWindow win1(configuration), win2(configuration), win3(configuration);
      window_action_router.add_window(&win1);
      window_action_router.add_window(&win2);
      window_action_router.add_window(&win3);

      QCOMPARE(window_action_router.windows(), (WindowMap{{1, &win1}, {2, &win2}, {3, &win3}}));
      QCOMPARE(win1.get_id(), 1);
      QCOMPARE(win2.get_id(), 2);
      QCOMPARE(win3.get_id(), 3);
    }
  }

  void test_get_relevant_windows() {
    describe("#get_relevant_windows");

    context("when no filters are provided");
    it("returns all defined windows") {
      WindowActionRouterPub window_action_router;
      Configuration configuration;
      window_action_router.initialize(&configuration);
      BrowserWindow win1(configuration), win2(configuration), win3(configuration);
      window_action_router.add_window(&win1);
      window_action_router.add_window(&win2);
      window_action_router.add_window(&win3);

      const auto &windows = window_action_router.get_relevant_windows();

      QCOMPARE(windows.size(), 3);
      QCOMPARE(windows, (std::vector<BrowserWindow *>{&win3, &win2, &win1}));
    }

    context("when win_id filter is 0");
    it("returns only the active window") {
      WindowActionRouterPub window_action_router;
      Configuration configuration;
      window_action_router.initialize(&configuration);
      BrowserWindow win1(configuration), win2(configuration), win3(configuration);
      window_action_router.add_window(&win1);
      window_action_router.add_window(&win2);
      window_action_router.add_window(&win3);
      activate_window(&win3);

      const auto &windows = window_action_router.get_relevant_windows(std::make_optional(0));

      QCOMPARE(windows.size(), 1);
      QCOMPARE(windows, (std::vector<BrowserWindow *>{&win3}));
    }

    context("when win_id filter is provided (non-zero)");
    it("returns only the window with that id") {
      WindowActionRouterPub window_action_router;
      Configuration configuration;
      window_action_router.initialize(&configuration);
      BrowserWindow win1(configuration), win2(configuration), win3(configuration);
      window_action_router.add_window(&win1);
      window_action_router.add_window(&win2);
      window_action_router.add_window(&win3);

      const auto &windows = window_action_router.get_relevant_windows(std::make_optional(2));

      QCOMPARE(windows.size(), 1);
      QCOMPARE(windows, (std::vector<BrowserWindow *>{&win2}));
    }

    context("when view_id filter is provided");
    it("returns only the window containing the view") {
      WindowActionRouterPub window_action_router;
      Configuration configuration;
      window_action_router.initialize(&configuration);
      BrowserWindow win1(configuration), win2(configuration), win3(configuration);
      window_action_router.add_window(&win1);
      window_action_router.add_window(&win2);
      window_action_router.add_window(&win3);

      auto view_id = win2.get_webview_list()[0].id; // Using first view from window 2
      const auto &windows =
          window_action_router.get_relevant_windows(std::nullopt, std::make_optional(view_id));

      QCOMPARE(windows.size(), 1);
      QCOMPARE(windows, (std::vector<BrowserWindow *>{&win2}));
    }

    context("when both win_id and view_id are provided");
    context("when given view exists inside given window");
    it("returns only the window containing the view") {
      WindowActionRouterPub window_action_router;
      Configuration configuration;
      window_action_router.initialize(&configuration);
      BrowserWindow win1(configuration), win2(configuration), win3(configuration);
      window_action_router.add_window(&win1);
      window_action_router.add_window(&win2);
      window_action_router.add_window(&win3);

      auto view_id = win2.get_webview_list()[0].id; // Using first view from window 2
      auto win_id = win2.get_id();
      const auto &windows = window_action_router.get_relevant_windows(std::make_optional(win_id),
                                                                      std::make_optional(view_id));

      QCOMPARE(windows.size(), 1);
      QCOMPARE(windows, (std::vector<BrowserWindow *>{&win2}));
    }

    context("when both win_id and view_id are provided");
    context("when given view exists inside given window");
    it("returns only the window containing the view") {
      WindowActionRouterPub window_action_router;
      Configuration configuration;
      window_action_router.initialize(&configuration);
      BrowserWindow win1(configuration), win2(configuration), win3(configuration);
      window_action_router.add_window(&win1);
      window_action_router.add_window(&win2);
      window_action_router.add_window(&win3);

      auto view_id = win2.get_webview_list()[0].id; // Using first view from window 2
      auto win_id = win2.get_id();
      const auto &windows = window_action_router.get_relevant_windows(std::make_optional(win_id),
                                                                      std::make_optional(view_id));

      QCOMPARE(windows.size(), 1);
      QCOMPARE(windows, (std::vector<BrowserWindow *>{&win2}));
    }

    context("when both win_id and view_id are provided");
    context("when given view is not inside given window");
    it("returns only the window containing the view") {
      WindowActionRouterPub window_action_router;
      Configuration configuration;
      window_action_router.initialize(&configuration);
      BrowserWindow win1(configuration), win2(configuration), win3(configuration);
      window_action_router.add_window(&win1);
      window_action_router.add_window(&win2);
      window_action_router.add_window(&win3);

      auto view_id = win3.get_webview_list()[0].id; // Using first view from window 3
      auto win_id = win2.get_id();
      const auto &windows = window_action_router.get_relevant_windows(std::make_optional(win_id),
                                                                      std::make_optional(view_id));

      QCOMPARE(windows.size(), 0);
      QCOMPARE(windows, (std::vector<BrowserWindow *>{}));
    }
  }
};

QTEST_REGISTER(WindowActionRouterSpec)
#include "WindowActionRouterSpec.moc"
// NOLINTEND
