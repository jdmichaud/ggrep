/*! \brief Represent the content to be displayed
 *
 * The browser is the main view of the screen. It contains buffers that can be
 * switched into view or not.
 */

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>
#include <string.h>
#include <exception>
#include <algorithm>

#include "logmacros.h"
#include "types.h"
#include "model.h"

#define LINE_STATIC_SIZE 256

/*
 * This structure describes the text attributes to apply to a certain contiguous
 * text zone
 */
struct tattr_t {
  int attrs_mask;
  uint start_pos;
  uint end_pos;

  tattr_t() : attrs_mask(0), start_pos(0), end_pos(0) {}
  tattr_t(int am, uint sp, uint ep) : attrs_mask(am), start_pos(sp), end_pos(ep) {}
};

/*
 * An array of array
 */
typedef tattr_t *attr_list_t;

/*
 * Custom exception raised when an error occurs on opening a file
 */
class OpenFileException : public std::exception {
public:
  OpenFileException(const std::string &filepath) :
    m_filepath(filepath), m_reason("") {
      LOGERR("OpenFileException raised filepath: " << filepath);
    }
  OpenFileException(const std::string &filepath, const std::string &reason) :
    m_filepath(filepath), m_reason(reason) {
      LOGERR("OpenFileException raised filepath: " << filepath <<
             " reason: " << m_reason);
    }
  const char *what() const noexcept
  {
    static char msg[1024];
    snprintf(msg, 1024, "Cannot open file: %s (%s)",
             m_filepath.c_str(), m_reason.c_str());
    return msg;
  }
private:
  const std::string  m_filepath;
  const std::string  m_reason;
};

class IBuffer {
public:
  /*!
   * Is the content of the buffer is binary (or text)
   */
  virtual bool is_binary() = 0;
  /*!
   * Provide a pointer to a pointer to the text.
   * Increment this pointer to get the next line.
   * Didn't use iterator to have no overhead at all.
   */
  virtual char **get_text() const = 0;
  /*!
   * Get/Set the number of lines in the buffer.
   */
  virtual uint get_number_of_line() const = 0;
  virtual void set_number_of_line(uint) = 0;
  /*!
   * Get/Set attributes of the buffer
   */
  virtual attr_list_t get_attrs() = 0;
  virtual void clear_attrs() = 0;
  /*!
   * Get/Set the current pointer (first line displayed) of the buffer
   */
  virtual uint get_first_line_displayed() const = 0;
  virtual void set_first_line_displayed(uint) = 0;
};

class Buffer : public IBuffer {
public:
  Buffer() : m_first_line_displayed(0) {  }
  virtual attr_list_t get_attrs() { return m_attrs; }
  virtual uint get_first_line_displayed() const { return m_first_line_displayed; }
  virtual void set_first_line_displayed(uint i) { m_first_line_displayed = i; }
protected:
  attr_list_t m_attrs;
private:
  uint m_first_line_displayed;
};

class FileBuffer : public Buffer {
public:
  FileBuffer(const std::string &filepath) : Buffer(), m_filepath(filepath) {
    LOGDBG("FileBuffer constructor " << this);
    // Check the file type (binary or text)
    if (is_binary())
      throw OpenFileException(m_filepath, "unsupported file type: binary");
    // If text open it
    m_file.open(m_filepath);
    if (!m_file.is_open()) throw OpenFileException(m_filepath);
    // Allocate the buffer
    m_nb_lines = get_number_of_line();
    // Null terimated string of pointers to char *
    m_buffer = new char*[m_nb_lines + 1];
    memset(m_buffer, 0, (m_nb_lines + 1) * sizeof (char*));
    m_buffer[m_nb_lines] = 0;
    // Load the whole file
    (*this).load({0, -1});
    // Initialize attrs array
    m_attrs = new tattr_t[m_nb_lines + 1];
    (*this).clear_attrs();
    LOGINF("FileBuffer loaded (" << this << ")");
  }

  ~FileBuffer() {
    LOGDBG("FileBuffer destructor " << this);
    m_file.close();
    for (uint i = 0; i < m_nb_lines; ++i)
      if (m_buffer[i]) delete m_buffer[i];
    delete[] m_buffer;
    delete[] m_attrs;
  }

  /*
   * Get pointers to the line indicated by line_number
   */
  char** get_text() const {
    LOGDBG("get_text");
    return m_buffer;
  }

  /*
   * Reset the attributes array
   */
  virtual void clear_attrs() {
    memset(m_attrs, 0, (m_nb_lines + 1) * sizeof (tattr_t));
  }

  /*
   * Get the number of lines of the text file.
   * At the end of this call, file header is at the beginning of the file
   * This function is a memoizer.
   */
  uint get_number_of_line() const {
    static uint line_count = get_number_of_line_();
    return line_count;
  }

  uint get_number_of_line_() const {
    std::ifstream ifs(m_filepath);
    // new lines will be skipped unless we stop it from happening:
    //m_file.unsetf(std::ios_base::skipws); really ???
    // count the newlines
    uint line_count = 0;
    while (!ifs.eof()) {
      int c = ifs.get();
      if (c == '\n') line_count++;
    }
    return line_count;
  }

  /*
   * This functions does nothing on a file buffer
   */
  void set_number_of_line(uint) { /* Not applicable */ };

  /*
   * This method is just an heuristic. Could be completely wrong.
   * If it is, there is a bigger chance that it returns false instead of true,
   * which is acceptable. We can always open binary file as text, but not being
   * able to open legitimate text file would be a problem.
   * This function is a memoizer.
   */
  bool is_binary() {
    static bool is_bin = is_file_binary(m_filepath, 100);
    return is_bin;
  }

  /*
   * Return the filepath of the FileBuffer
   */
  inline const std::string &get_filepath() const { return (*this).m_filepath; }

protected:
  bool is_file_binary(const std::string &filepath, size_t test_len) {
    // Open the file in bniary
    std::ifstream ifs(filepath, std::ifstream::binary);
    // Check file is opened
    if (!ifs.is_open()) throw OpenFileException(filepath);
    // get pointer to associated buffer object
    std::filebuf* pbuf = ifs.rdbuf();
    // get file size using buffer's members
    std::size_t size = pbuf->pubseekoff(0, ifs.end, ifs.in);
    pbuf->pubseekpos(0, ifs.in);
    // Read at most test_len bytes, that should be enougth
    std::size_t read_size = std::min(test_len, size);
    // allocate memory to contain file data
    char* buffer = new char[read_size];
    // get file data
    pbuf->sgetn(buffer, read_size);
    // Get the answer
    bool is_bin = (*this).is_data_binary(buffer, read_size);
    delete[] buffer;
    return is_bin;
  }

  bool is_data_binary(const void *data, size_t len) {
    const char *pc = (const char *) memchr(data, '\0', len);
    if (pc != NULL) {
      LOGERR("found character \\0 at "
             << (unsigned long) (pc - (const char *) data));
    }
    return (pc != NULL);
  }

  /*
   * Load the interval from the file into the buffer
   */
  bool load(std::pair<uint, uint> interval) {
    // put read head at the beginning of the file
    m_file.seekg(m_file.beg);
    // pass the first lines
    char line[LINE_STATIC_SIZE];
    for (uint i = 0; i < interval.first && !m_file.eof(); ) {
      m_file.getline(line, LINE_STATIC_SIZE, '\n');
      // Did we return because we found a carriage return or because the line is
      // too long ?
      if (!m_file.fail()) i++;
    }
    // load lines up to line interval.second.
    // If pass -1, the we suppose 2^32-1 is the maximum of number of lines for
    // the file we manage
    for (uint i = 0; i < interval.second && !m_file.eof() && !m_file.fail(); ++i)
    {
      // Get next line size
      uint next_line_size = get_next_line_size();
      // Allocate the new string + 1 for the carriage return
      char *newline = new char[next_line_size];
      // retrieve it
      m_file.getline(newline, next_line_size);
      // set it in the buffer
      m_buffer[i] = newline;
    }
    return true;
  }

  /*
   * Get the size of the next line (up to the character \n or eof)
   */
  uint get_next_line_size() {
    // If already end of file, return 0
    if (m_file.eof()) return 0;
    // Get current position
    std::streampos pos = m_file.tellg();
    // Statically allocate the receiving memory
    char line[LINE_STATIC_SIZE];
    // block_count is used if several getline are necessary to get the full line
    // up to the next \n ot eof.
    uint block_count = 0;
    bool found_new_line = false;
    while (!m_file.eof() && !found_new_line) {
      m_file.getline(line, LINE_STATIC_SIZE, '\n');
      // as long as we don't find a \n (fail) or end of file (eof)
      found_new_line = !m_file.fail();
      // Reset the fail bit so ifstream can continue to read...
      m_file.clear(m_file.rdstate() & ~std::ios::failbit);
      if (!found_new_line) block_count++;
    }
    // Total number of count for the line
    uint char_count = m_file.gcount() + block_count * LINE_STATIC_SIZE;
    // reset position where is used to be.
    // Since c++11, ifstreams can seekg away from eof.
    // However, if no character has been read and eof is reached, don't seek back.
    if (char_count != 0 || !m_file.eof()) m_file.seekg(pos);
    return char_count;
  }

protected:
  std::string         m_filepath;
  std::ifstream       m_file;
  char                **m_buffer;
  uint                m_nb_lines;

private:
  FileBuffer(const FileBuffer &b) = delete;
};

#endif // __BUFFER_H__