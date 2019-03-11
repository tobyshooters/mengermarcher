#ifndef __THREADING_H__
#define __THREADING_H__

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>

class Semaphore {
  public:
    Semaphore(int n=0) : counter(n) {}

    void wait() {
      std::lock_guard<std::mutex> lg(m);
      cv.wait(m, [this] { return counter > 0; });
      counter--;
    }

    void signal() {
      std::lock_guard<std::mutex> lg(m);
      counter++;
      if (counter == 1) cv.notify_all();
    }

  private:
    std::condition_variable_any cv;
    std::mutex m;
    int counter;
};

class ThreadPool {
  public:
    ThreadPool(size_t num)
      : wts(num), s_workers(num), num_active(0), s_function(0) {
      // Spawn Dispatcher and Worker threads
      dt = std::thread([this] { dispatcher(); });
      for (size_t wid = 0; wid < num; wid++)
        wts[wid].t = std::thread([this] (size_t wid) { worker(wid); }, wid);
    }

    ~ThreadPool() {
      wait();
      exit = true;
      for (size_t wid = 0; wid < wts.size(); wid++) {
        // Start all workers to receive exit signal
        wts[wid].s.signal();
        wts[wid].t.join();
      }
      // Start dispatcher to receive exit signal
      s_function.signal();
      dt.join();
    }

    void schedule(std::function<void()> fn) {
      // Add function to queue
      m_todo.lock();
      todo.push(fn);
      m_todo.unlock();
      // Increment number of functions
      s_function.signal();
      // Increment number of active workers
      m_active.lock();
      num_active++;
      m_active.unlock();
    }

    void wait() {
      // Wait til no more active workers
      m_active.lock();
      cv_active.wait(m_active, [this] { return num_active == 0; });
      m_active.unlock();
    }
    
  private:

    // Spawned Helper Threads
    // ----------------------

    void dispatcher() {
      while (!exit) {
        // Wait for free function and free worker
        s_function.wait();
        if (exit) break;
        s_workers.wait();

        m_todo.lock();
        // Find the free worker
        size_t id = 0;
        for (; id < wts.size(); id++)
          if (wts[id].free) break;
        wts[id].free = false;

        // Give worker the function
        wts[id].fn = todo.front();
        todo.pop();
        m_todo.unlock();

        // Start the worker
        wts[id].s.signal();
      }
    };

    void worker(size_t id) {
      while (!exit) {
        // Wait til worker is started, indicating function assigned
        wts[id].s.wait();
        if (exit) break;
        // Call function
        wts[id].fn();

        // Free worker and decrement number of workers active
        m_active.lock();
        num_active--;
        if (num_active == 0) cv_active.notify_all();
        m_active.unlock();

        wts[id].free = true;
        // Increment number of workers
        s_workers.signal();
      }
    }

    // Types and Private Variables
    // ---------------------------

    std::thread dt;

    typedef struct worker_t {
      worker_t() : s(0), free(true) {}
      std::thread t;            // Thread
      Semaphore s;              // Activator
      bool free;                // Availability
      std::function<void()> fn; // Function to call
    } worker_t;

    std::vector<worker_t> wts;
    Semaphore s_workers;

    bool exit; // Used to kill off processes

    int num_active;
    std::mutex m_active;
    std::condition_variable_any cv_active;
    Semaphore s_function;

    std::mutex m_todo;
    std::queue<std::function<void()>> todo;
};

#endif //__THREADING_H__
