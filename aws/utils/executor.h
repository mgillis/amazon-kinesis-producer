// Copyright 2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
//
// Licensed under the Amazon Software License (the "License").
// You may not use this file except in compliance with the License.
// A copy of the License is located at
//
//  http://aws.amazon.com/asl
//
// or in the "license" file accompanying this file. This file is distributed
// on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied. See the License for the specific language governing
// permissions and limitations under the License.

#ifndef AWS_UTILS_EXECUTOR_H_
#define AWS_UTILS_EXECUTOR_H_

#include <chrono>
#include <functional>

namespace aws {
namespace utils {

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

class ScheduledCallback {
 public:
  virtual ~ScheduledCallback() = default;

  virtual void cancel() = 0;

  virtual bool completed() = 0;

  virtual void reschedule(TimePoint at) = 0;

  virtual TimePoint expiration() = 0;

  virtual void reschedule(std::chrono::milliseconds from_now) {
    reschedule(Clock::now() + from_now);
  }

  virtual std::chrono::milliseconds time_left() {
    auto dur =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            expiration() - Clock::now());
    if (dur.count() > 0) {
      return dur;
    } else {
      return std::chrono::milliseconds(0);
    }
  }
};

class Executor {
 public:
  using Func = std::function<void () noexcept>;

  virtual void submit(Func f) = 0;

  virtual void schedule(Func f,
                        TimePoint at,
                        std::shared_ptr<ScheduledCallback>* container) = 0;

  virtual void schedule(Func f,
                        std::chrono::milliseconds from_now,
                        std::shared_ptr<ScheduledCallback>* container) {
    schedule(std::move(f), Clock::now() + from_now, container);
  }

  virtual void schedule(Func f, TimePoint at) {
    schedule(std::move(f), at, nullptr);
  }

  virtual void schedule(Func f, std::chrono::milliseconds from_now) {
    schedule(std::move(f), from_now, nullptr);
  }

  virtual size_t num_threads() const noexcept = 0;

  virtual void join() = 0;
};

} //namespace utils
} //namespace aws

#endif //AWS_UTILS_EXECUTOR_H_
