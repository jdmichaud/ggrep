#include <mutex>
#include <cstring>
#include "logmacros.h"
#include "buffer.h"
#include "buffer_model.h"
#include "processor.h"

BufferModel::BufferModel(std::shared_ptr<IBuffer> &&buffer) :
  m_filter(*this), m_file_buffer(std::dynamic_pointer_cast<FileBuffer>(buffer))
{
  LOGDBG("BufferModel creator " << this);
  // Set the current buffer as the file buffer
  set_current_buffer(std::dynamic_pointer_cast<IBuffer>(m_file_buffer));
  // By default, we will OR the filtering matches
  m_filter_set.land = false;
  // Before starting the filter thread, initialize the FilteredBuffer which will
  // be zeroed by the thread. This is not optimal as we will allocate memory
  // that we won't be using now if ever.
  m_filtered_buffer = std::make_shared<FilteredBuffer>(buffer);
  // Should be started elsewhere. In the controller ?
  m_filter.start();
}

BufferModel::~BufferModel() {
  LOGDBG("BufferModel desctructor " << this);
}

void BufferModel::set_current_buffer(std::shared_ptr<IBuffer> buffer) {
  m_current_buffer = buffer;
  notify_observers();
}

char ** BufferModel::get_text() {
  return m_current_buffer->get_text();
}

uint BufferModel::get_number_of_line() const {
  return m_current_buffer->get_number_of_line();
}

void BufferModel::set_number_of_line(uint i) {
  return m_current_buffer->set_number_of_line(i);
  notify_observers();
}

uint BufferModel::get_first_line_displayed() const {
  return m_current_buffer->get_first_line_displayed();
}

void BufferModel::set_first_line_displayed(uint i) {
  m_current_buffer->set_first_line_displayed(i);
  notify_observers();
}

const attr_list &BufferModel::get_attrs() {
  return m_current_buffer->get_attrs();
}

Update<attr_list> BufferModel::set_attrs() {
  notify_callback_t f = std::bind(&BufferModel::notify_observers, this);
  return Update<attr_list>(m_current_buffer->get_attrs(), f);
}

void BufferModel::clear_filtered_line() {
  m_filtered_buffer->clear();
  notify_observers();
}

void BufferModel::add_filtered_line(char *line) {
  m_filtered_buffer->add_line(line);
  notify_observers();
}

void BufferModel::enable_filtering() {
  LOGDBG("switch buffer to filtered view");
  m_current_buffer = m_filtered_buffer;
  notify_observers();
}

void BufferModel::disable_filtering() {
  LOGDBG("switch buffer to UNfiltered view");
  m_current_buffer = m_file_buffer;
  notify_observers();
}

std::shared_ptr<FileBuffer> BufferModel::get_file_buffer() {
  return m_file_buffer;
}

void BufferModel::retrieve_filter_set(filter_set_t &filter_set) {
  std::lock_guard<std::mutex> lock(m_filter_set_mutex);
  filter_set = m_filter_set;
}

void BufferModel::add_filter(const std::string &filter) {
  std::lock_guard<std::mutex> lock(m_filter_set_mutex);
  // Add the new regex to the set of filters of the current buffer
  set_filter_set().update().filters.emplace_back(
    std::move(std::make_pair(filter, std::regex(filter))));
  // Signal the filtering processor
  m_filter.signal();
}

void BufferModel::update_last_filter(const std::string &filter) {
  std::lock_guard<std::mutex> lock(m_filter_set_mutex);
  {
    Update<filter_set_t> update = set_filter_set();
    // Remove the current filter...
    update.update().filters.pop_back();
    // ... and replace it with the new value.
    update.update().filters.emplace_back(
      std::move(std::make_pair(filter, std::regex(filter))));
  }
  // Signal the filtering processor
  m_filter.signal();
}

void BufferModel::remove_last_filter() {
  std::lock_guard<std::mutex> lock(m_filter_set_mutex);
  set_filter_set().update().filters.pop_back();
  // Signal the filtering processor
  m_filter.signal();
}

