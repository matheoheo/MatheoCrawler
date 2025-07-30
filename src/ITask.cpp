#include "pch.h"
#include "ITask.h"

bool ITask::isComplete() const
{
    return mIsCompleted;
}

void ITask::complete()
{
    mIsCompleted = true;
}
