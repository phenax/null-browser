#pragma once

#include <QWidget>
#include <QtCore>
#include <cstdint>
#include <functional>

#include "keymap/KeySeqParser.hpp"
#include "utils.hpp"

using KeyAction = std::function<void()>;

struct KeyMap {
  KeySequence key_sequence;
  KeyAction action;
};

enum KeyMode : uint8_t { Normal, Insert };

class KeymapEvaluator : public QObject {
  Q_OBJECT

public:
  KeymapEvaluator() = default;

  static KeymapEvaluator *instance() {
    static auto *keymap_evaluator = new KeymapEvaluator;
    return keymap_evaluator;
  }

  void add_keymap(KeyMode mode, const QString &key, KeyAction action);
  bool evaluate(Qt::KeyboardModifiers modifiers, Qt::Key key);
  KeyMode mode_from_string(const QString &mode_string);

  DEFINE_SETTER(set_current_mode, current_mode)
  DEFINE_GETTER(get_current_mode, current_mode)

protected:
  const QList<KeyMap> *current_mode_keys();
  bool is_insertable_mode();

private:
  QMap<KeyMode, QList<KeyMap>> modal_keys;
  KeySeqParser key_seq_parser;
  KeyMode current_mode = KeyMode::Normal;
  KeySequence active_key_sequence;
};
