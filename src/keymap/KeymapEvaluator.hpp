#pragma once

#include <QWidget>
#include <QtCore>
#include <functional>
#include <unordered_map>

#include "keymap/KeySeqParser.hpp"
#include "utils.hpp"

using KeyAction = std::function<void()>;

struct KeyMap {
  KeySequence key_sequence;
  KeyAction action;
};

using KeyMode = QString;

struct KeyModeConfig {
  bool passthrough;
};

struct KeyModeState {
  QList<KeyMap> keymap;
  KeyModeConfig config;
};

class KeymapEvaluator : public QObject {
  Q_OBJECT

public:
  KeymapEvaluator() = default;

  static KeymapEvaluator &instance() {
    static KeymapEvaluator keymap_evaluator;
    return keymap_evaluator;
  }

  void add_keymap(const KeyMode &mode, const QString &key, KeyAction action);
  bool evaluate(Qt::KeyboardModifiers modifiers, Qt::Key key);
  void define_mode(const KeyMode &mode, const KeyModeConfig &config);

  DEFINE_SETTER(set_current_mode, current_mode)
  DEFINE_GETTER(get_current_mode, current_mode)

protected:
  const QList<KeyMap> *current_mode_keys();
  bool is_passthrough_mode();

private:
  std::unordered_map<KeyMode, KeyModeState> modal_keys;
  KeySeqParser key_seq_parser;
  KeyMode current_mode = "i";
  KeySequence active_key_sequence;
};
