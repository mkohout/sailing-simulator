#include "Process.h"

using namespace Game_Logic;

void Process::VOnAbort()
{
}

void Process::VOnSuccess()
{
}
void Process::VOnFail()
{
}

void Process::Succeed()
{
	SetState(SUCCEEDED);
}

void Process::Abort()
{
    SetState(ABORTED);
}

void Process::Fail()
{
	SetState(FAILED);
}

void Process::Pause()
{
	SetState(PAUSED);
}

void Process::UnPause()
{
	SetState(RUNNING);
}