#include "PID.h"

/**
 * TODO: Complete the PID class. You may add any additional desired functions.
 */

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp_, double Ki_, double Kd_, size_t window_size) {
  /**
   * TODO: Initialize PID coefficients (and errors, if needed)
   */
  Kp = Kp_;
  Ki = Ki_;
  Kd = Kd_;

  _window_size = window_size;
  _recent_errors = queue<double>();
}

void PID::UpdateError(double cte) {
  /**
   * TODO: Update PID errors based on cte.
   */
  d_error = cte - p_error;
  p_error = cte;

  // accumulate i_error
  i_error += cte;

  // if queue is full, drop the head
  _recent_errors.push(cte);
  if (_recent_errors.size() == _window_size) {
    double head = _recent_errors.front();
    _recent_errors.pop();

    // only consider the most recent errors
    i_error -= head;
  }
}


double PID::ControlValue() {
  return -Kp * p_error - Kd * d_error - Ki * i_error/(_recent_errors.size() + 1e-8);
}