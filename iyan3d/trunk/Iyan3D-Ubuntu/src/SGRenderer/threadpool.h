/**

ThreadPool
==========

== Introduction

This small library implements a pool of threads in C++ for a unix system with
support for POSIX threads. It is meant to run CPU-intensive tasks among the
different cores of the CPU.

Let's say the program has to perform an expensive task that can be splitted into
several parallel tasks. In this case you would like to spawn as many threads as
cores the CPU has and send a portion of the work to each of them. Then wait
until all threads have finished and, if needed, put all the answers together.

Creating threads is expensive. Each time a new thread is created the program
must ask the operating system to create the thread and allocate the resources
needed. The same situation occurs when threads are destroyed.  Depending on the
type or problem being solved this could be an issue or not.  For real-time
applications all these operations will create unnecesary latency.

Another option is to create a pool of threads when the application is started
and send them some work as needed. This is actually cleaner, more efficient and
faster than creating (and destroying) threads every time they are needed. It has
also an interesting consequence: we can easily do load balance across the
threads. Since the thread pool basically consists of a queue of jobs, we can
split the work in smaller units that will be assigned to any thread that is
available to perform the work. This will distribute the work evenly among the
threads and improve the performance when there is only a sub-set of CPU cores
performing the work.

The current implementation does not pretend to be a general solution for any
threading system.  On the contrary it is straightforward solution that works on
linux and is easy to understand.

== Usage

Work units

The threadpool receives jobs and saves them in an internal queue, these jobs
must be a subclass of the provided `ThreadPoolWorker`:

For example, if we want to define a work unit that takes one Body* parameter we
can define this class:

[source,cpp]
--------------------------------------------------------------------------------
class MyWorker: public ThreadPoolWorker {
	Body* body;
public:
	MyWorker(Body* _body) : body(_body) {}
	~MyWorker() {}
	void operator()()
	{
		// Do the work
	}
};
--------------------------------------------------------------------------------

Once we have defined the worker class we can use it:

[source,cpp]
--------------------------------------------------------------------------------
ThreadPool pool(4); // Initialize the pool with a maximum of 4 OS threads

// ...

for (int i=0; i < bodies.size()) {
	pool.enqueueWork(new MyWorker(bodies[i])); // Enqueue work units
}
pool.waitEnd(); // Waint until all the work units have finished

// ...

--------------------------------------------------------------------------------

In this example 4 OS threads are created: the resources for 4 threads are
allocated and those will be competing for the real CPU cores. If the machine
where it runs has only 2 cores, 2 of those 4 threads will be executing at the
same time and the other 2 will remain waiting. Note that creating more threads
than the real number of cores is not necessarily a bad thing: if one of those
threads blocks (by an I/O operation for example) another thread will take its
CPU time to progress doing its work.

== Thread Local Storage

It is a common problem to need of a local variable to the thread this is usually
provided by some kind of Thread Local Storage. There is no ThreadLocalStorage
class in our code, instead we rely on the compiler's qualifier `__thread`, which
is implemented in our two target compilers GCC and CLANG. By using the
`__thread` qualifier we can have global variables than can only be used from a
specific thread. This is specially useful for cached data.

[source,cpp]
--------------------------------------------------------------------------------
// In the global scope
__thread MyCache* thread_cache = NULL;

// In the worker:
if (thread_cache == NULL) {
	thread_cache = new MyCache();
}

// ...

--------------------------------------------------------------------------------

== MIT-License

Copyright (c) 2012 David Sinuela Pastor <siu.4coders@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_

#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <vector>

class ThreadPoolWorker
  /// A work unit for the TreadPool
  /// Create subclasses to send work to the threadpool
{
public:
	virtual ~ThreadPoolWorker() {}
	  /// Destroy

	virtual void operator()() = 0;
	  /// Perform the work
};


class ThreadPool
  /// ThreadPool class: holds a list of threads and a queue of workers
  /// It provides two main operations:
  ///   enqueueWork(ThreadPoolWorker* worker)
  ///   waitEnd()
{
public:
	explicit ThreadPool(int _maxThreads = 2);
	  /// Create the pool and all the synchronization primitives with 
	  /// _maxTreads OS threads

	~ThreadPool();
	  /// Delete all the associated resources

	bool enqueueWork(ThreadPoolWorker* worker);
	  /// Add worker to the queue, and start processing it
	  /// ThreadPool takes ownership of the worker and will destroy it when
	  /// finished

	void waitEnd();
	  /// Wait until all workers have finished

	static void* threadExecute(void* _pool);
	  /// Static entry for threads (thread entry point)

private:
	int maxThreads;
	  /// Number of OS threads

	int incompleteWork;
	  /// Number of UNFINISHED work units: this is NOT the number of items in
	  /// the workerQueue

	bool requestThreadEnd;
	  /// Request thead workers to finish

	std::vector<pthread_t*> threads;
	  /// Collection of threads

	std::queue<ThreadPoolWorker*> workerQueue;
	  /// Job queue

	pthread_mutex_t mutexWorkCompletion;
	  /// Protects: incompleteWork

	pthread_mutex_t mutexQueue;
	  /// Protects: workerQueue

	pthread_cond_t incompleteWorkCond;
	  /// Signals: changes in incompleteWork

	sem_t availableWork;
	  /// Semaphore holding the number of work units available

	void initializeThreads();
	bool fetchWork(ThreadPoolWorker** worker);
};

#endif // __THREADPOOL_H_
