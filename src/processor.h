/*! \brief Processors declarations
 *
 * Processors are class that run a thread, analyze the model and modify it for
 * a result to be displayed on screen.
 */

#include <list>
#include <string>

#include "buffer_model.h"

typedef struct {
  std::list<std::string> filters;
  bool  land; // logical and if true, logical or otherwise
} filter_set;

class Processor {
public:
  /* Start the thread */
  virtual void start() = 0;
  /* Stop and destroy the thread */
  virtual void stop() = 0;
  /* Signal the update, in the model, of the input parameter to the processor */
  virtual void signal() = 0;
};
 
class FilterEngine : Processor {
public:
  FilterEngine(BufferModel &buffer_model);
  void start();
  void stop();
  void signal();
  /*
   * Start once the signal is raised (or on first call to start).
   * Check the content of the filter set. If the filter set is empty, wait for a
   * signal.
   */
  void filter();
private:
  BufferModel &m_buffer_model;
};