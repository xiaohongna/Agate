#pragma once
#include <future>
#include <vector>
#include <queue>
#include <atomic>
#include <assert.h>
#include "MainTaskQueue.h"
namespace Agate
{
    class ThreadPool
    {
    public:
        ThreadPool(size_t threads = 1) :stop(false)
        {
            for (size_t i = 0; i < threads; ++i)
                workers.emplace_back(
                    [this]
                    {
                        while (true)
                        {
                            std::function<void()> task;
                            {
                                std::unique_lock<std::mutex> lock(this->queue_mutex);
                                this->condition.wait(lock,
                                    [this] { return this->stop || !this->tasks.empty(); });
                                if (this->stop && this->tasks.empty())
                                    return;
                                task = std::move(this->tasks.front());
                                this->tasks.pop();
                            }

                            task();
                        }
                    }
                    );
        }

        template<typename F>
        void AddTask(F&& task)
        {
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                assert(stop == false);
                tasks.emplace(std::move(task));
            }
            condition.notify_one();
        }

        template<typename F>
        void AddTask(F&& task, std::function<void(decltype(task()))>&& then)
        {
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                assert(stop == false);
                tasks.emplace([tk = std::move(task), tn = std::move(then), this]{
                    auto result = tk();
                    main_Queue.Async([result, t = std::move(tn)]{
                        t(result);
                        });
                    });
            }
            condition.notify_one();
        }

        ~ThreadPool()
        {
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                stop = true;
            }
            condition.notify_all();
            for (std::thread& worker : workers)
                worker.join();
        }
    private:
        std::vector< std::thread > workers;
        std::queue< std::function<void()> > tasks;

        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop;

        MainTaskQueue    main_Queue;
    };
}

