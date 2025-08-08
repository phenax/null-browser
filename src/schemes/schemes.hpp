#pragma once

#include <QWebEngineUrlScheme>
#include <qwebengineurlscheme.h>

void register_nullrpc_scheme() {
  QWebEngineUrlScheme scheme("nullrpc");
  scheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
  scheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::LocalScheme |
                  QWebEngineUrlScheme::LocalAccessAllowed |
                  QWebEngineUrlScheme::ContentSecurityPolicyIgnored |
                  QWebEngineUrlScheme::FetchApiAllowed);
  QWebEngineUrlScheme::registerScheme(scheme);
}

void register_null_scheme() {
  QWebEngineUrlScheme scheme("null");
  scheme.setSyntax(QWebEngineUrlScheme::Syntax::Host);
  scheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::LocalScheme |
                  QWebEngineUrlScheme::LocalAccessAllowed);
  QWebEngineUrlScheme::registerScheme(scheme);
}

void register_all_schemes() {
  register_nullrpc_scheme();
  register_null_scheme();
}
