﻿#pragma once

#include "physics/equipotential.hpp"

#include <functional>

#include "geometry/grassmann.hpp"
#include "geometry/named_quantities.hpp"
#include "numerics/double_precision.hpp"
#include "quantities/elementary_functions.hpp"

namespace principia {
namespace physics {
namespace internal_equipotential {

using geometry::Normalize;
using geometry::Vector;
using geometry::Velocity;
using integrators::IntegrationProblem;
using numerics::DoublePrecision;
using quantities::Abs;
using quantities::Frequency;
using quantities::Pow;
using quantities::Time;
using ::std::placeholders::_1;
using ::std::placeholders::_2;
using ::std::placeholders::_3;

template<typename ODE>
ODEAdaptiveStepParameters<ODE>::ODEAdaptiveStepParameters(
    AdaptiveStepSizeIntegrator<ODE> const& integrator,
    std::int64_t const max_steps,
    Length const& length_integration_tolerance)
    : integrator_(&integrator),
      max_steps_(max_steps),
      length_integration_tolerance_(length_integration_tolerance) {}

template<typename ODE>
AdaptiveStepSizeIntegrator<ODE> const&
ODEAdaptiveStepParameters<ODE>::integrator() const {
  return *integrator_;
}

template<typename ODE>
std::int64_t ODEAdaptiveStepParameters<ODE>::max_steps() const {
  return max_steps_;
}

template<typename ODE>
Length ODEAdaptiveStepParameters<ODE>::length_integration_tolerance() const {
  return length_integration_tolerance_;
}

template<typename Frame>
Equipotential<Frame>::Equipotential(
    AdaptiveParameters const& adaptive_parameters,
    Ephemeris<Frame> const& ephemeris)
    : adaptive_parameters_(adaptive_parameters),
      ephemeris_(&ephemeris) {}

template<typename Frame>
auto Equipotential<Frame>::ComputeLine(
    Bivector<double, Frame> const& plane,
    Position<Frame> const& position,
    Instant const& t) -> State {
  ODE equation{
      .compute_derivative = std::bind(
          &Equipotential::RightHandSide, this, plane, position, t, _1, _2, _3)};
  SystemState initial_state({{position}, {0}}, s_initial_);
  IntegrationProblem<ODE> const problem{
      .equation = std::move(equation),
      .initial_state = std::move(initial_state)};

  typename AdaptiveStepSizeIntegrator<ODE>::Parameters const
      integrator_parameters(
          /*first_time_step=*/initial_s_step_,
          /*safety_factor=*/0.9,
          /*max_steps=*/adaptive_parameters_.max_steps(),
          /*last_step_is_exact=*/true);

  State equipotential;
  typename AdaptiveStepSizeIntegrator<ODE>::AppendState const append_state =
      [&equipotential](SystemState const& system_state) {
        std::get<0>(equipotential)
            .push_back(std::get<0>(system_state.y).front().value);
        std::get<1>(equipotential)
            .push_back(std::get<1>(system_state.y).front().value);
      };

  auto const tolerance_to_error_ratio =
      std::bind(&Equipotential::ToleranceToErrorRatio, this, _1, _2);

  auto const instance = adaptive_parameters_.integrator().NewInstance(
      problem, append_state, tolerance_to_error_ratio, integrator_parameters);
  auto status = instance->Solve(s_final_);

  return equipotential;
}

template<typename Frame>
absl::Status Equipotential<Frame>::RightHandSide(
    Bivector<double, Frame> const& plane,
    Position<Frame> const& position,
    Instant const& t,
    IndependentVariable const& s,
    State const& state,
    StateVariation& state_variation) {
  // First state variable.
  auto const& γₛ = std::get<0>(state).front();
  auto const dVǀᵧ₍ₛ₎ =
      ephemeris_->ComputeGravitationalAccelerationOnMasslessBody(γₛ, t);
  Velocity<Frame> const γʹ = Normalize(plane * dVǀᵧ₍ₛ₎) * characteristic_speed_;

  // Second state variable.
  double const β = std::get<1>(state).front();
  auto const& γ₀ = position;
  Frequency const βʹ =
      s == s_initial_
          ? Frequency{}
          : Pow<2>(characteristic_speed_) * (s - s_initial_) / (γₛ - γ₀).Norm²();
  //LOG(ERROR)<<u8"βʹ="<<βʹ;

  std::get<0>(state_variation).front() = γʹ;
  std::get<1>(state_variation).front() = βʹ;

  return β > β_max_ ? absl::AbortedError("β reached max") : absl::OkStatus();
}

template<typename Frame>
double Equipotential<Frame>::ToleranceToErrorRatio(
    Difference<IndependentVariable> const& current_s_step,
    SystemStateError const& error) {
  if (current_s_step < initial_s_step_) {
    return 0.0;
  }
  Length const max_length_error = std::get<0>(error).front().Norm();
  double const max_braking_error = Abs(std::get<1>(error).front());
  return std::min(
      adaptive_parameters_.length_integration_tolerance() / max_length_error,
      1.0 / max_braking_error);
  //return adaptive_parameters_.length_integration_tolerance() / max_length_error;
}

}  // namespace internal_equipotential
}  // namespace physics
}  // namespace principia
