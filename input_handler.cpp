#include "input_handler.h"
#include "keyboardButton.h"
#include "asyncTaskManager.h"

AsyncTask::DoneStatus
InputHandler::poll_task(GenericAsyncTask* /*task*/, void* data) {
    static_cast<InputHandler*>(data)->poll();
    return AsyncTask::DS_cont;
}

void InputHandler::poll() {
    // Convenience lambda so each line stays readable.
    auto down = [this](ButtonHandle btn) {
        return m_watcher->is_button_down(btn);
    };
 
    m_sprint = down(KeyboardButton::shift());
 
    // Movement — WASD and arrow keys map to the same flags.
    m_forward  = down(KeyboardButton::ascii_key('w')) || down(KeyboardButton::up());
    m_backward = down(KeyboardButton::ascii_key('s')) || down(KeyboardButton::down());
    m_left     = down(KeyboardButton::ascii_key('a')) || down(KeyboardButton::left());
    m_right    = down(KeyboardButton::ascii_key('d')) || down(KeyboardButton::right());
 
    // System
    m_quit    = down(KeyboardButton::escape());
    m_restart = down(KeyboardButton::ascii_key('r'));
}

void InputHandler::init(WindowFramework* window) {
    m_watcher = DCAST(MouseWatcher, window->get_mouse().node());
 
    PT(GenericAsyncTask) task = new GenericAsyncTask("input_poll", &InputHandler::poll_task, this);
    task->set_sort(-10);
    AsyncTaskManager::get_global_ptr()->add(task);
}
