#pragma once

#include <vector>

#include "gmock/gmock.h"
#include "physics/continuous_trajectory.hpp"

namespace principia {
namespace physics {
namespace _mock_continuous_trajectory {
namespace internal {

template<typename Frame>
class MockContinuousTrajectory : public ContinuousTrajectory<Frame> {
 public:
  MockContinuousTrajectory() : ContinuousTrajectory<Frame>() {}

  MOCK_METHOD(Position<Frame>,
              EvaluatePosition,
              (Instant const& time),
              (const, override));
  MOCK_METHOD(DegreesOfFreedom<Frame>,
              EvaluateDegreesOfFreedom,
              (Instant const& time),
              (const, override));
};

}  // namespace internal

using internal::MockContinuousTrajectory;

}  // namespace _mock_continuous_trajectory
}  // namespace physics
}  // namespace principia

namespace principia::physics {
using namespace principia::physics::_mock_continuous_trajectory;
}  // namespace principia::physics
