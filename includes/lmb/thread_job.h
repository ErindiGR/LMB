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
    {
        m_completed=false;
    }

    virtual void Execute()=0;

    bool Completed()
    {
        return m_completed;
    }

private:

    friend JobManager;
    bool m_completed;

};

class JobManager : public Singleton<JobManager>
{

    using hi_time_point = std::chrono::steady_clock::time_point;

public:


    void ThreadExec(size_t thread_index);

    JobManager();

    ~JobManager();

    void End();

    void Push(std::shared_ptr<Job> job);


    inline const int GetNumThreads()
    {
        return m_num_threads;
    }


protected:


    enum EThreadState
    {
        Waiting,
        Executing,
        Ended
    };

    std::vector<std::thread> m_threads;
    std::vector<EThreadState> m_threads_state;

    std::vector<std::shared_ptr<Job>> m_jobs;
    size_t m_job_start;

    bool m_end;

    size_t m_num_threads;

    std::mutex m_mutex;
    std::condition_variable m_cv;

};

}