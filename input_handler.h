#pragma once
#include "common.h"
#include "mouseWatcher.h"
#include "genericAsyncTask.h"

class InputHandler {
public:
    InputHandler() = default;

    // Grabs the MouseWatcher from the window and registers the poll task.
    void init(WindowFramework* window);

    bool is_forward()  const { return m_forward; }
    bool is_sprint()   const { return m_sprint; }
    bool is_backward() const { return m_backward; }
    bool is_left()     const { return m_left; }
    bool is_right()    const { return m_right; }
    bool is_quit()     const { return m_quit; }
    bool is_restart()  const { return m_restart; }

private:
    PT(MouseWatcher) m_watcher;

    bool m_sprint   = false;
    bool m_forward  = false;
    bool m_backward = false;
    bool m_left     = false;
    bool m_right    = false;
    bool m_quit     = false;
    bool m_restart  = false;

    void poll();

    // Task entry point required by GenericAsyncTask.
    static AsyncTask::DoneStatus poll_task(GenericAsyncTask* task, void* data);
};