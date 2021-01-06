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

    /**
     * @copydoc LMB::Calculator::GetProgress()
     * 
     */
    const Progress<0,100> GetProgress() const override
    {
        size_t completed = 0;

        for(size_t i=0;i<m_jobs.size();i++)
            if(m_jobs[i]->Completed())
                completed++;
        
        return Progress<0,100>(to_real(completed) / to_real(m_jobs.size()) * 100);
    }

    /**
     * @copydoc ICalculable::EndCalc()
     * 
     */
    void EndCalc() override
    { 
        while(1)
        {
            if(GetProgress().GetVal() >= 100)
                break;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }


protected:
    
    std::vector<std::shared_ptr<Job>> m_jobs;
};

}