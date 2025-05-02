#include <QList>
#include <QWidget>
#include <QtCore>
#include <qwebengineprofile.h>

#include "WindowMediator.hpp"
#include "widgets/WebViewStack.hpp"

WindowMediator::WindowMediator(WebViewStack *webview_stack) : webview_stack(webview_stack) {
  connect(webview_stack, &WebViewStack::new_window_requested, this,
          &WindowMediator::new_window_requested);
  connect(webview_stack, &WebViewStack::close_window_requested, this,
          &WindowMediator::close_window_requested);
}

void WindowMediator::update_user_agent(const QString &user_agent) {
  auto *profile = webview_stack->get_profile();
  profile->setHttpUserAgent(user_agent);
}

void WindowMediator::update_downloads_dir(const QString &downloads_dir) {
  auto *profile = webview_stack->get_profile();
  profile->setDownloadPath(downloads_dir);
}

void WindowMediator::update_permissions_persistance(const QString &persistance) {
  auto *profile = webview_stack->get_profile();
  auto persistance_policy = Configuration::to_permission_persistance_policy(persistance);
  profile->setPersistentPermissionsPolicy(persistance_policy);
}

WindowMediator::~WindowMediator() {
  disconnect(this);
  delete webview_stack;
}
