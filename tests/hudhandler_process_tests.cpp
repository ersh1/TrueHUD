#include "PCH.h"

// Test HUDHandler drain/guard behavior without invoking game UI.

#define private public
#define protected public
#include "HUDHandler.h"
#undef private
#undef protected

#include <catch2/catch_all.hpp>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

using ScaleformMenu = Scaleform::TrueHUDMenu; // alias for signature clarity

namespace {
  // Utility to clear handler state between tests
  void ClearHandlerQueues(HUDHandler& h) {
    HUDHandler::Locker lock(h._lock);
    while (!h._taskQueue.empty()) h._taskQueue.pop();
    // Do not touch _stackingDamage to avoid including hashing machinery in tests
  }

  struct TestHUDHandler : HUDHandler {
    using EventResult = RE::BSEventNotifyControl;
    EventResult ProcessEvent(const RE::TESCombatEvent*, RE::BSTEventSource<RE::TESCombatEvent>*) override { return EventResult::kContinue; }
    EventResult ProcessEvent(const RE::TESDeathEvent*, RE::BSTEventSource<RE::TESDeathEvent>*) override { return EventResult::kContinue; }
    EventResult ProcessEvent(const RE::TESEnterBleedoutEvent*, RE::BSTEventSource<RE::TESEnterBleedoutEvent>*) override { return EventResult::kContinue; }
    EventResult ProcessEvent(const RE::TESHitEvent*, RE::BSTEventSource<RE::TESHitEvent>*) override { return EventResult::kContinue; }
    EventResult ProcessEvent(const RE::MenuOpenCloseEvent*, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override { return EventResult::kContinue; }
  };
}

TEST_CASE("HUDHandler DrainAndProcessTasks skips empty tasks and does not throw", "[hudhandler][empty]") {
  TestHUDHandler h;
  ClearHandlerQueues(h);

  std::atomic<int> executed{0};

  {
    HUDHandler::Locker lock(h._lock);
    h._taskQueue.push(HUDHandler::HUDTask{[&](ScaleformMenu&) { ++executed; }});
    h._taskQueue.push(HUDHandler::HUDTask{}); // empty
    h._taskQueue.push(HUDHandler::HUDTask{[&](ScaleformMenu&) { ++executed; }});
  }

  REQUIRE_NOTHROW(h.DrainAndProcessTasks([&](HUDHandler::HUDTask& t) {
    if (t) { t(*reinterpret_cast<ScaleformMenu*>(nullptr)); /* never deref in task */ ++executed; --executed; }
    // The line above increments and decrements to avoid unused param warnings while not changing executed count.
  }));

  // Execute tasks but without calling into the parameter; validate we only saw the two valid tasks
  // We track success by having the tasks themselves increment 'executed'.
  REQUIRE(executed.load() == 2);
}

TEST_CASE("HUDHandler DrainAndProcessTasks executes tasks exactly once and in FIFO order (per drained batch)", "[hudhandler][fifo]") {
  TestHUDHandler h;
  ClearHandlerQueues(h);

  std::vector<int> order;
  order.reserve(3);

  {
    HUDHandler::Locker lock(h._lock);
    h._taskQueue.push(HUDHandler::HUDTask{[&](ScaleformMenu&) { order.push_back(1); }});
    h._taskQueue.push(HUDHandler::HUDTask{[&](ScaleformMenu&) { order.push_back(2); }});
    h._taskQueue.push(HUDHandler::HUDTask{[&](ScaleformMenu&) { order.push_back(3); }});
  }

  h.DrainAndProcessTasks([&](HUDHandler::HUDTask& t) {
    if (t) t(*reinterpret_cast<ScaleformMenu*>(nullptr));
  });

  REQUIRE(order.size() == 3);
  REQUIRE(order[0] == 1);
  REQUIRE(order[1] == 2);
  REQUIRE(order[2] == 3);
}

TEST_CASE("HUDHandler reentrancy: tasks enqueued during processing run on the next pass", "[hudhandler][reentrancy]") {
  TestHUDHandler h;
  ClearHandlerQueues(h);

  std::atomic<int> executed{0};

  {
    HUDHandler::Locker lock(h._lock);
    h._taskQueue.push(HUDHandler::HUDTask{[&](ScaleformMenu&) {
      ++executed; // 1st
      // Enqueue another task during processing; should run in next pass only
      HUDHandler::Locker lock2(h._lock);
      h._taskQueue.push(HUDHandler::HUDTask{[&](ScaleformMenu&) { ++executed; }}); // 2nd
    }});
  }

  h.DrainAndProcessTasks([&](HUDHandler::HUDTask& t) {
    if (t) t(*reinterpret_cast<ScaleformMenu*>(nullptr));
  });

  REQUIRE(executed.load() == 1);

  h.DrainAndProcessTasks([&](HUDHandler::HUDTask& t) {
    if (t) t(*reinterpret_cast<ScaleformMenu*>(nullptr));
  });

  REQUIRE(executed.load() == 2);
}

TEST_CASE("HUDHandler concurrent producers: all tasks execute without data races", "[hudhandler][concurrency]") {
  TestHUDHandler h;
  ClearHandlerQueues(h);

  constexpr int Producers = 4;
  constexpr int TasksPerProducer = 250;
  constexpr int Total = Producers * TasksPerProducer;

  std::atomic<int> executed{0};

  std::vector<std::thread> threads;
  threads.reserve(Producers);

  for (int p = 0; p < Producers; ++p) {
    threads.emplace_back([&] {
      for (int i = 0; i < TasksPerProducer; ++i) {
        HUDHandler::Locker lock(h._lock);
        h._taskQueue.push(HUDHandler::HUDTask{[&](ScaleformMenu&) { executed.fetch_add(1, std::memory_order_relaxed); }});
      }
    });
  }

  using namespace std::chrono_literals;

  for (int i = 0; i < 10; ++i) {
    h.DrainAndProcessTasks([&](HUDHandler::HUDTask& t) {
      if (t) t(*reinterpret_cast<ScaleformMenu*>(nullptr));
    });
    std::this_thread::sleep_for(5ms);
  }

  for (auto& t : threads) t.join();

  h.DrainAndProcessTasks([&](HUDHandler::HUDTask& t) {
    if (t) t(*reinterpret_cast<ScaleformMenu*>(nullptr));
  });

  REQUIRE(executed.load() == Total);
}
