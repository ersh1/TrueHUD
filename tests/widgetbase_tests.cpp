#include "PCH.h"

#include <catch2/catch_all.hpp>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

#include "TrueHUDAPI.h"

namespace {
  struct TestWidget final : TRUEHUD_API::WidgetBase {
    void Update(float) override {}
    void Initialize() override {}
    void Dispose() override {}

    void Enqueue(TRUEHUD_API::WidgetBase::WidgetTask t) { AddWidgetTask(std::move(t)); }
  };
}

TEST_CASE("ProcessDelegates skips empty tasks and does not throw", "[widgetbase][empty]") {
  TestWidget w;
  std::atomic<int> executed{0};

  w.Enqueue([&] { ++executed; });
  w.Enqueue(TRUEHUD_API::WidgetBase::WidgetTask{}); // empty
  w.Enqueue([&] { ++executed; });

  REQUIRE_NOTHROW(w.ProcessDelegates());
  REQUIRE(executed.load() == 2);
}

TEST_CASE("ProcessDelegates executes tasks exactly once and in FIFO order (per drained batch)", "[widgetbase][fifo]") {
  TestWidget w;
  std::vector<int> order;
  order.reserve(3);

  w.Enqueue([&] { order.push_back(1); });
  w.Enqueue([&] { order.push_back(2); });
  w.Enqueue([&] { order.push_back(3); });

  w.ProcessDelegates();

  REQUIRE(order.size() == 3);
  REQUIRE(order[0] == 1);
  REQUIRE(order[1] == 2);
  REQUIRE(order[2] == 3);
}

TEST_CASE("Reentrancy: tasks enqueued by a running task are executed in the next pass", "[widgetbase][reentrancy]") {
  TestWidget w;
  std::atomic<int> executed{0};

  w.Enqueue([&] {
    ++executed; // 1st
    // Enqueue another task during processing; should run on next pass
    w.Enqueue([&] { ++executed; }); // 2nd
  });

  // First drain: runs first task; second task is enqueued after drain started
  w.ProcessDelegates();
  REQUIRE(executed.load() == 1);

  // Second drain: picks up the newly enqueued task
  w.ProcessDelegates();
  REQUIRE(executed.load() == 2);
}

TEST_CASE("Concurrent producers: all tasks are executed without data races", "[widgetbase][concurrency]") {
  TestWidget w;
  constexpr int Producers = 4;
  constexpr int TasksPerProducer = 250;
  constexpr int Total = Producers * TasksPerProducer;

  std::atomic<int> executed{0};

  // Producers push tasks concurrently
  std::vector<std::thread> threads;
  threads.reserve(Producers);
  for (int p = 0; p < Producers; ++p) {
    threads.emplace_back([&] {
      for (int i = 0; i < TasksPerProducer; ++i) {
        w.Enqueue([&] { executed.fetch_add(1, std::memory_order_relaxed); });
      }
    });
  }

  using namespace std::chrono_literals;
  // Consumer: drain periodically while producers run
  for (int i = 0; i < 10; ++i) {
    w.ProcessDelegates();
    std::this_thread::sleep_for(5ms);
  }

  for (auto& t : threads) t.join();
  w.ProcessDelegates();

  REQUIRE(executed.load() == Total);
}
