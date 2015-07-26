#include "prompt_model.h"

PromptModel::PromptModel() : m_prompt("") {
  m_cursor_position.x = 0;
  m_cursor_position.y = 0;
}
