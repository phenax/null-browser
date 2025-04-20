#include <QWidget>
#include <QtCore>
#include <utility>

#include "keymap/KeySeqParser.hpp"
#include "keymap/KeymapEvaluator.hpp"

// TODO: Clear mapping after some time
// TODO: Forward current keysequence with partial key after timeout (only passthrough kind)

void KeymapEvaluator::add_keymap(const KeyMode &mode, const QString &key, KeyAction action) {
  if (!modal_keys.contains(mode))
    modal_keys[mode] = {.keymap = {}, .config = {.passthrough = false}};

  auto key_seq = key_seq_parser.parse(key);
  modal_keys[mode].keymap.append(KeyMap{.key_sequence = key_seq, .action = std::move(action)});
}

bool KeymapEvaluator::evaluate(Qt::KeyboardModifiers modifiers, Qt::Key key) {
  if (key == Qt::Key_Control || key == Qt::Key_Shift || key == Qt::Key_Meta || key == Qt::Key_Alt)
    return true;

  const auto *keymaps = current_mode_keys();
  auto found_pending_matches = false;

  active_key_sequence.append(KeyChord{.mod = modifiers, .key = key});

  for (const auto &keymap : *keymaps) {
    auto match_type = KeySeqParser::key_sequence_match(keymap.key_sequence, active_key_sequence);

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

  if (is_passthrough_mode())
    return found_pending_matches;

  return true;
}

bool KeymapEvaluator::is_passthrough_mode() {
  if (!modal_keys.contains(current_mode))
    return false;

  return modal_keys[current_mode].config.passthrough;
}

const QList<KeyMap> *KeymapEvaluator::current_mode_keys() {
  if (!modal_keys.contains(current_mode))
    return new QList<KeyMap>();

  return &modal_keys[current_mode].keymap;
}

void KeymapEvaluator::define_mode(const KeyMode &mode, const KeyModeConfig &config) {
  if (!modal_keys.contains(current_mode))
    modal_keys[current_mode] = {.keymap = {}, .config = config};
  else
    modal_keys[mode].config = config;
}
