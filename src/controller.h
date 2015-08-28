/*! \brief Controller for the ggrep application
 *
 *  The controller gathers the input from the view and update the models
 *  accordingly.
 */

#ifndef  __CONTROLLER_H__
#define __CONTROLLER_H__

#include <map>
#include <list>
#include <vector>
#include <memory>
#include <fstream>
#include <thread>

#include "work_queue.h"
#include "producer.h"
#include "consumer.h"

#include "view.h"
#include "browser_model.h"
#include "fbar_model.h"
#include "prompt_model.h"
#include "state_model.h"
#include "state.h"
#include "buffer_factory.h"
#include "context.h"
#include "input.h"


class Controller {
{
public:
  Controller(BrowserModel &browser_model,
             FBarModel &fbar_model,
             PromptModel &prompt_model,
             StateModel &state_model,
             IBufferFactory &buffer_factory);

  /**
   * A view injects a character through this api.
   */
  void inject(int key);
  /** Bind a view to the controller. The controller is supposed to interact only
    * with models and not the view. But this priviledged access to the view is
    * restricted to special interaction like requesting an input (prompt).
    */
  void bind_view(IView &);
  /** These functions route the model updates call to either:
    * a. all the views binded to the controller if the calling thread is the
    *    main thread
    * b. an REDRAW event injected in the Controller state machine which will be
    *    handled by the main thread (see a)
    */
  void route_browser_callback(IObservable &observable);
  void route_fbar_callback(IObservable &observable);
  void route_prompt_callback(IObservable &observable);
  void route_state_callback(IObservable &observable);
  /* thread related functions */
  inline bool is_interrupted() { return _interrupted; }
  inline void interrupt() { _interrupted = true; }
  /* Start the controller engine */
  void start();
  /**
   * Context APIs
   */
  void change_state(state_e state, const IEvent &event);
  void enter_state(state_e state, const IEvent &event);
  void exit_state(const IEvent &event);
  void backtrack(const IEvent &event);

public:
  /*
   * Open a file and add it to the list of buffers in the model.
   * The StreamType is used by the Buffer class to read a file.
   */
  bool create_buffer(const std::string &filepath);
  /*
   * Return the currently active buffer in the model (i.e. displayed buffer).
   */
  buffer_list::const_iterator get_active_buffer();
  /*
   * Return the nth buffer in the buffer list
   */
  Update<buffer_list::iterator> set_nth_buffer(uint idx) { return _browser_model.set_nth_buffer(idx); }
  buffer_list::const_iterator get_nth_buffer(uint idx) const ;
  /*
   * Set the active BufferModel (i.e. the one the view shall display)
   */
  void activate_buffer(uint idx);
  void activate_buffer(buffer_list::const_iterator it);

  /* Prompt */
  void set_prompt(const std::string &prompt);
  void set_prompt_cursor_position(const pos &p);
  void clear_prompt();
  const std::string &get_prompt() const;
  /* Prompt messages */
  void raise_error(const std::string &error_msg);

  /* Browsing APIs */
  /*
   * Set the size of the view. In a "normal" MVC model, the view would handle
   * directly some user actions like scrolling. In out design, the view is
   * minimal and the controller is handling the user actions. For this reason,
   * we keep some characteristics of the view in the model so we can properly
   * operate on the view.
   */
  void set_view_size(uint nlines, uint ncolumns);
  void scroll_buffer_up(uint shift);
  void scroll_buffer_down(uint shift);
  void scroll_buffer_page_up();
  void scroll_buffer_page_down();
  void scroll_buffer_end();

  /* Filter APIs */
  /** Add a new filter into the filter_set and signal the filter thread */
  void add_filter(const std::string &filter);
  /** Reset the buffer with the file content, discarding modification we might
    * have made while filtering
    */
  void reinit_current_buffer();

private:
  /* Start the view engine */
  void view_start();

private:
  // List of binded views
  std::list<IView *>  _views;
  bool                _interrupted;
  // For now let's assume only one set of models
  BrowserModel        &_browser_model;
  FBarModel           &_fbar_model;
  PromptModel         &_prompt_model;
  StateModel          &_state_model;
  // The factory from which new buffer will be constructed
  IBufferFactory      &_buffer_factory;
  // The state machine
  Context             _context;
  // Factory generating inputs from keys
  InputFactory        _input_factory;
  // Event work queue
  WorkQueue<Event>    _event_queue;
  // Producer for user inputs
  Producer<Event>     _user_event_producer;
  // Producer for Processors
  Producer<Event>     _processor_input_producer;
  // Event consumer, driving the view thread
  Consumer<Event>     _event_consumer;
  // This is the id of the main thread, the one that interprets event and acts
  // on models and views
  std::thread::id     _main_thread_id;

private:
  Controller(const Controller &&) = delete;
};

#endif //__CONTROLLER_H__