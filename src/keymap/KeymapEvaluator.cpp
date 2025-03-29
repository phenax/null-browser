#include <QWidget>
#include <QtCore>
#include <utility>

#include "keymap/KeySeqParser.hpp"
#include "keymap/KeymapEvaluator.hpp"

// TODO: Clear mapping after some time

void KeymapEvaluator::add_keymap(KeyMode mode, const QString &key,
                                 KeyAction action) {
  if (!modal_keys.contains(mode))
    modal_keys.insert(mode, {});

  qDebug() << "    " << mode << key;

  auto key_seq = key_seq_parser.parse(key);
  modal_keys[mode].append(
      KeyMap{.key_sequence = key_seq, .action = std::move(action)});
}

bool KeymapEvaluator::evaluate(Qt::KeyboardModifiers modifiers, Qt::Key key) {
  if (key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Meta ||
      key == Qt::Key_Alt)
    return true;

  const auto *keymaps = current_mode_keys();
  auto found_pending_matches = false;

  active_key_sequence.append(KeyChord{.mod = modifiers, .key = key});

  for (const auto &keymap : *keymaps) {
    auto match_type = KeySeqParser::key_sequence_match(keymap.key_sequence,
                                                       active_key_sequence);

    if (match_type == KeyMatchType::Match) {
      qDebug() << "CALLED" << key;
      keymap.action();
      active_key_sequence.clear();
      return true;
    }
    if (match_type == KeyMatchType::Pending) {
      found_pending_matches = true;
    }
  }

  if (!found_pending_matches)
    active_key_sequence.clear();

  if (is_insertable_mode())
    return found_pending_matches;

  return true;
}

bool KeymapEvaluator::is_insertable_mode() {
  return current_mode == KeyMode::Insert;
}

const QList<KeyMap> *KeymapEvaluator::current_mode_keys() {
  if (!modal_keys.contains(current_mode))
    return new QList<KeyMap>();

  return &modal_keys[current_mode];
}

KeyMode KeymapEvaluator::mode_from_string(const QString &mode_string) {
  if (mode_string == "n")
    return KeyMode::Normal;
  if (mode_string == "i")
    return KeyMode::Insert;
  return KeyMode::Normal;
}
