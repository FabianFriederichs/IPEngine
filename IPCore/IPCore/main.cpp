#include <iostream>
#include "Deque.h"
#include <chrono>
#include <thread>
#include <string>
#include "ThreadPool.h"
#include "TaskHandle.h"
#include <chrono>
#include "function.h"
#include <mutex>
#include <type_traits>

std::mutex mut;

//TODO: implement small object optimization for any and function

struct fib
{
	uint64_t result;
	uint64_t n;
	//ThreadPool* pool;
};

void nothing(TaskContext& context)
{
	//at.fetch_add(1);
}

std::atomic<size_t> tsks = 0;

void parallelfib(TaskContext& context)
{	
	fib* mfib = context;
	if (mfib->n < 2)
	{
		mfib->result=mfib->n;
		return;
	}
	else
	{
		fib fnm1;
		fnm1.n = mfib->n - 1;
		fnm1.result=0;

		fib fnm2;
		fnm2.n = mfib->n - 2;
		fnm2.result=0;

		/*TaskContext tcfnm1(&fnm1);
		TaskContext tcfnm2(&fnm2);*/

		//tsks.fetch_add(2);

		TaskHandle tfnm1 = context.getPool()->createTask(TaskFunction::make_func<parallelfib>(), TaskContext(&fnm1));
		TaskHandle tfnm2 = context.getPool()->createTask(TaskFunction::make_func<parallelfib>(), TaskContext(&fnm2));

		tfnm1.spawn(&context);
		tfnm2.spawn(&context);

		tfnm1.wait(&context);
		tfnm2.wait(&context);

		mfib->result = (tfnm1.getContext()->get<fib*>()->result + tfnm2.getContext()->get<fib*>()->result);
		return;
	}
}

uint64_t standardfib(uint64_t n)
{
	if (n < 2)
	{
		return n;
	}
	else
	{
		return standardfib(n - 1) + standardfib(n - 2);
	}
}

int main()
{
	
	/*TaskContext blubb;
	for (int i = 0; i < 30000000; i++)
	{
		TaskContext bla = fib();
		blubb = bla;
	}*/


	int64_t ms1 = 0;
	int64_t ms2 = 0;
	
		ThreadPool pool(std::thread::hardware_concurrency() - 1);
		pool.startWorkers();
		while (true)
		{
			fib myfib;
			myfib.n = 30;
			myfib.result = 0;
			//TaskContext context(&myfib);
			//tsks.fetch_add(1);
			TaskHandle task = pool.createTask(TaskFunction::make_func<parallelfib>(), TaskContext(&myfib));
			auto ts1 = std::chrono::high_resolution_clock::now();
			task.submit();
			task.wait();
			auto te1 = std::chrono::high_resolution_clock::now();
			ms1 = std::chrono::duration_cast<std::chrono::milliseconds>(te1 - ts1).count();
			std::cout << ms1 << "ms " << "Result: " << myfib.result << "\n";// Tasks: " << tsks.load() << "\n";
			//tsks.store(0);
		}
		pool.stopWorkers();
	
	return 0;
}

