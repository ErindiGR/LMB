#pragma once

#include "lmb/calculator.h"
#include "lmb/base_type.h"
#include "lmb/thread_job.h"

namespace LMB
{

class JobBaseCalculator : public Calculator
{

public:

    ~JobBaseCalculator()
    {
        EndCalc();
    }

    const size_t GetProgress()
    {
        size_t completed = 0;

        for(size_t i=0;i<m_jobs.size();i++)
            if(m_jobs[i]->Completed())
                completed++;
        
        return ( (real_t)completed / (real_t)m_jobs.size()) * 100;
    }

    void EndCalc()
    { 
        while(1)
        {
            if(GetProgress() >= 100)
                break;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }


protected:
    
    std::vector<std::shared_ptr<Job>> m_jobs;
};

}