
#include "base/thread_pool.hpp"

#include "glog/logging.h"
#include "gmock/gmock.h"

namespace principia {
namespace base {

class ThreadPoolTest : public ::testing::Test {
 protected:
  ThreadPoolTest() : pool_(std::thread::hardware_concurrency()) {
    LOG(ERROR) << "Concurrency is " << std::thread::hardware_concurrency();
  }

  ThreadPool<void> pool_;
};

// Check that execution occurs in parallel.  If things were sequential, the
// integers in |numbers| would be non-decreasing.
TEST_F(ThreadPoolTest, ParallelExecution) {
  static constexpr int number_of_calls = 1'000'000;

  std::mutex lock;
  std::vector<std::int64_t> numbers;
  std::vector<std::future<void>> futures;
  for (std::int64_t i = 0; i < number_of_calls; ++i) {
    futures.push_back(pool_.Add([i, &lock, &numbers]() {
      std::lock_guard<std::mutex> l(lock);
      numbers.push_back(i);
    }));
  }

  for (auto const& future : futures) {
    future.wait();
  }

  bool decreasing = false;
  for (std::int64_t i = 1; i < numbers.size(); ++i) {
    if (numbers[i] < numbers[i - 1]) {
      decreasing = true;
    }
  }
  EXPECT_TRUE(decreasing);
}

}  // namespace base
}  // namespace principia
