#include <mutex>
#include <thread>
#include <memory>
#include <mutex>
#include <functional>

class ThreadManager {
public:
    ThreadManager() = default;
    virtual ~ThreadManager();
    virtual void start() final;
    virtual void run() = 0;
    virtual void stop() final;
    virtual void wait() final;
    virtual void detach() final;
protected:
    virtual bool allowRunning() final;

private:

    bool running_ = false;
    std::mutex mtx_;
    std::unique_ptr<std::thread> thread_ = nullptr;
};

ThreadManager::~ThreadManager() {
    stop();
    wait();
};

void ThreadManager::start() {
    if (running_) {
        return;
    }
    auto func = std::bind(&ThreadManager::run, this);

    std::lock_guard<std::mutex> lck(mtx_);
    if (!running_) {
        running_ = true;
        thread_ = std::unique_ptr<std::thread>(new std::thread(func));
    }
    return;
}

void ThreadManager::detach() {
    if (thread_ != nullptr) {
        thread_->detach();
    }
}
void ThreadManager::stop() {
    running_ = false;
}

void ThreadManager::wait() {
    if (thread_ == nullptr) {
        return;
    }
    if (thread_->joinable()) {
        thread_->join();
    }
}
bool ThreadManager::allowRunning() {
    return running_;
}