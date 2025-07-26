#pragma once

#include <QWebEngineUrlScheme>
#include <qwebengineurlscheme.h>

void register_nullrpc_scheme() {
  QWebEngineUrlScheme scheme("nullrpc");
  scheme.setFlags(
      QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::LocalScheme |
      QWebEngineUrlScheme::LocalAccessAllowed | QWebEngineUrlScheme::ServiceWorkersAllowed |
      QWebEngineUrlScheme::ContentSecurityPolicyIgnored | QWebEngineUrlScheme::FetchApiAllowed);
  scheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
  QWebEngineUrlScheme::registerScheme(scheme);
}

void register_all_schemes() { register_nullrpc_scheme(); }
