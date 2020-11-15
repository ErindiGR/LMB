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

class JobManager final : public Singleton<JobManager>
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

    JobManager();

    ~JobManager();

    void End();

    void Push(std::shared_ptr<Job> job);

    inline const size_t GetNumThreads()
    {
        return m_num_threads;
    }

protected:

    void ThreadExec(size_t thread_index);

protected:

    std::vector<std::thread>            m_threads;
    std::vector<EThreadState>           m_threads_state;
    std::vector<std::shared_ptr<Job>>   m_jobs;
    std::mutex                          m_mutex;
    std::condition_variable             m_cv;
    size_t                              m_job_start;
    size_t                              m_num_threads;
    bool                                m_end;
};

}