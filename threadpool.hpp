#include <condition_variable>
#include <deque>
#include <future>
#include <mutex>
#include <thread>
#include <vector>

class thread_pool {
public:
	thread_pool()
	{
	}

	template<class F, class R = std::result_of_t<F &()>>
	void queue(F &&f)
	{
		std::packaged_task<R()> p(std::forward<F>(f));

		{
			std::unique_lock<std::mutex> l(m);
			work.emplace_back(std::move(p));
		}

		v.notify_one();
	}

	void start_threads(std::size_t n)
	{
		for (std::size_t i = 0; i < n; i++)
			done.push_back(std::async(std::launch::async, [this] { thread_task(); }));
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
	}

	std::vector<std::thread> workers;
	std::deque<std::packaged_task<void()>> work;
	std::mutex m;
	std::condition_variable v;
	std::vector<std::future<void>> done;

protected:
	void thread_task()
	{
		while (true) {
			std::packaged_task<void()> f;
			{
				{
					std::unique_lock<std::mutex> l(m);

					if (work.empty()) {
						v.wait(l, [&] { return !work.empty(); });
					}

					f = std::move(work.front());
					work.pop_front();
				}

				if (!f.valid())
					return;

				f();
			}
		}
	}
};
