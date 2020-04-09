#pragma once

#include <mutex>
#include <unordered_map>

#include <engine/deadline.hpp>
#include <engine/io/common.hpp>
#include <engine/mpsc_queue.hpp>

#include <engine/ev/watcher.hpp>

namespace engine::io {

// Not thread-safe
// Reports HUP as readiness (libev limitation)
class Poller {
 public:
  using WatcherPtr = std::shared_ptr<ev::Watcher<ev_io>>;
  using WatcherWeakPtr = std::weak_ptr<ev::Watcher<ev_io>>;
  using WatchersMap = std::unordered_map<int, WatcherWeakPtr>;

  struct Event {
    int fd{kInvalidFd};
    enum { kError = -1, kRead, kWrite } type{kError};
  };

  Poller();
  ~Poller();

  Poller(const Poller&) = delete;
  Poller(Poller&&) = delete;

  void Reset();

  [[nodiscard]] WatcherPtr AddRead(int fd);
  [[nodiscard]] WatcherPtr AddWrite(int fd);

  bool NextEvent(Event&, Deadline);
  bool NextEventNoblock(Event&);

 private:
  explicit Poller(const std::shared_ptr<MpscQueue<Event>>&);

  void StopRead(int fd);
  void StopWrite(int fd);
  void ResetWatchers();

  static void IoEventCb(struct ev_loop*, ev_io*, int) noexcept;

  MpscQueue<Event>::Consumer event_consumer_;
  MpscQueue<Event>::Producer event_producer_;
  std::mutex read_watchers_mutex_;
  WatchersMap read_watchers_;
  std::mutex write_watchers_mutex_;
  WatchersMap write_watchers_;
};

}  // namespace engine::io
