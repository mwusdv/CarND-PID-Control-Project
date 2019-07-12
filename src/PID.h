#ifndef PID_H
#define PID_H

#include <queue>

using std::queue;

class PID {
 public:
  /**
   * Constructor
   */
  PID();

  /**
   * Destructor.
   */
  virtual ~PID();

  /**
   * Initialize PID.
   * @param (Kp_, Ki_, Kd_) The initial PID coefficients
   */
  void Init(double Kp_, double Ki_, double Kd_, size_t window_size=100);

  /**
   * Update the PID error variables given cross track error.
   * @param cte The current cross track error
   */
  void UpdateError(double cte);

  double ControlValue();

 private:
  /**
   * PID Errors
   */
  double p_error;
  double i_error;
  double d_error;

  // recent errors to cimpute Ki
  size_t _window_size;
  queue<double> _recent_errors;

  /**
   * PID Coefficients
   */ 
  double Kp;
  double Ki;
  double Kd;
};

#endif  // PID_H