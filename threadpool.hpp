#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <mutex>
#include <vector>

class thread_pool {
public:
	thread_pool()
		: waiting(false)
	{
	}

	template<class F, class R = std::result_of_t<F &()>>
	void queue(F &&f)
	{
		std::function<R()> p = f;

		{
			std::unique_lock<std::mutex> l(m);
			work.emplace_back(std::move(p));
		}

		v.notify_one();
	}

	void start_threads(std::size_t n)
	{
		for (std::size_t i = 0; i < n; i++) {
			workers.emplace_back(std::move(std::thread([this] { thread_task(); })));
		}
	}

	void abort()
	{
		cancel_pending();
		finish();
	}

	void cancel_pending()
	{
		std::unique_lock<std::mutex> l(m);
		work.clear();
	}

	void finish()
	{
		{
			std::unique_lock<std::mutex> l(m);
			for (auto i = 0u; i < workers.size(); i++)
				work.push_back({});
		}
		v.notify_all();

		for (auto &&worker : workers)
			worker.join();

		workers.clear();
	}

	std::vector<std::thread> workers;
	std::deque<std::function<void()>> work;
	std::mutex m;
	std::condition_variable v;

	bool waiting;

protected:
	void thread_task()
	{
		while (true) {
			std::function<void()> f;
			{
				{
					std::unique_lock<std::mutex> l(m);
					waiting = false;

					if (work.empty()) {
						v.wait(l, [&] { return !work.empty(); });
					}

					f = std::move(work.front());
					work.pop_front();
				}

				if (!f)
					return;

				f();
			}
		}
	}
};
