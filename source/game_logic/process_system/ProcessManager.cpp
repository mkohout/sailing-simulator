#include "ProcessManager.h"

using namespace Game_Logic;

unsigned int ProcessManager::UpdateProcesses(
	unsigned int deltaMilliseconds)
{
	unsigned short int successCount = 0;
	unsigned short int failCount = 0;

	ProcessList::iterator it = m_processes.begin();

	while (it != m_processes.end())
	{
		StrongProcessPtr pCurrProcess = (*it);

		ProcessList::iterator thisIt = it;
		++it;

		if (pCurrProcess->GetState() == Process::UNINTIALIZED)
		{
			pCurrProcess->VInit();
		}

		if (pCurrProcess->GetState() == Process::RUNNING)
		{
			pCurrProcess->VOnUpdate(deltaMilliseconds);
		}

		if (pCurrProcess->IsDead())
		{
			switch (pCurrProcess->GetState())
			{
                case Process::SUCCEEDED:
                {
                    pCurrProcess->VOnSuccess();
                    StrongProcessPtr pChild =
                        pCurrProcess->RemoveChild();

                    if (pChild)
                    {
                        AttachProcess(pChild);
                    }
                    else
                    {
                        successCount++;
                    }
                    break;
                }
                case Process::FAILED:
                {
                    pCurrProcess->VOnFail();
                    failCount++;
                    break;
                }
                case Process::ABORTED:
                {
                    pCurrProcess->VOnAbort();
                    failCount++;
                    break;
                }
                default:
                    break;
                
			}
			m_processes.erase(thisIt);
		}
	}

	return ((successCount << 16 | failCount));
}

void ProcessManager::AttachProcess(StrongProcessPtr pProcess)
{
	m_processes.push_back(pProcess);
}

void ProcessManager::AbortAllProcesses(bool immediate)
{
    for(StrongProcessPtr pProcess : m_processes)
    {
        pProcess->Abort();
    }
}

void ProcessManager::ClearAllProcesses()
{
    m_processes.clear();
}