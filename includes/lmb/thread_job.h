#pragma once

#include "lmb/singleton.h"

#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <memory>


namespace LMB
{

class JobManager;

class Job
{

public:

    Job()
    : m_completed(false)
    {
    }

    bool Completed()
    {
        return m_completed;
    }

    virtual void Execute() = 0;

private:

    friend JobManager;

    bool m_completed;

};

class JobManager
{

public:

    enum class EThreadState
    {
        Waiting,
        Executing,
        Ended
    };

protected:

    using hi_time_point = std::chrono::steady_clock::time_point;

public:

    static void Init();

    static void Term();

    static void Push(const std::shared_ptr<Job> &job);

    static inline const size_t GetNumThreads()
    {
        return m_num_threads;
    }

protected:

    static void ThreadExec(size_t thread_index);

protected:

    static std::vector<std::thread>            m_threads;
    static std::vector<EThreadState>           m_threads_state;
    static std::vector<std::shared_ptr<Job>>   m_jobs;
    static std::mutex                          m_mutex;
    static std::condition_variable             m_cv;
    static size_t                              m_job_start;
    static size_t                              m_num_threads;
    static bool                                m_end;
};

}