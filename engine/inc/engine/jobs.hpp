#pragma once

#include <condition_variable>
#include <functional>
#include <vector>
#include <thread>
#include <atomic>
#include <queue>

struct Job
{
    std::function<void()> run;
};

class JobSystem
{
    std::vector<std::thread> workers;

    std::queue<Job> jobQueue;

    std::mutex queueMutex;

    std::condition_variable convar;

    std::atomic<bool> stopping{false};

public:
    JobSystem();

    ~JobSystem();

    void push(std::function<void()> task);

private:
    void update();
};