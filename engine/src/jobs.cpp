#include <engine/jobs.hpp>

JobSystem::JobSystem()
{
    unsigned int workerCount = std::thread::hardware_concurrency();

    if (workerCount > 1) workerCount--;

    workers.reserve(workerCount);
    for (unsigned int i = 0; i < workerCount; ++i)
    {
        workers.emplace_back(&JobSystem::update, this);
    }
}

JobSystem::~JobSystem()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        stopping = true;
    }

    convar.notify_all();

    for (std::thread &worker : workers)
    {
        if (worker.joinable()) worker.join();
    }
}

void JobSystem::push(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        jobQueue.push(Job{std::move(task)});
    }

    convar.notify_one();
}

void JobSystem::update()
{
    while (true)
    {
        Job job;

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            convar.wait(lock, [this]() { return !jobQueue.empty() || stopping; });

            if (stopping && jobQueue.empty()) return;

            job = std::move(jobQueue.front());

            jobQueue.pop();
        }

        job.run();
    }
}