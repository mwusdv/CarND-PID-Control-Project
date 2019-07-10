#include "twiddle.h"

Twiddle::Twiddle()
    : _params      ()
    , _steps       ()
    , _trials      ()
    , _param_index (0)
    , _best_reward (0)
    , _tolerance   (0)
    , _expansion   (0)
    , _shrink      (0) {

   // empty
}

Twiddle::~Twiddle() {}

// start parameter optimization
void Twiddle::init(const vector<double>& init_params, const vector<double>& init_steps, 
              double tolerance, double expansion, double shrink) {
    _params = init_params;
    _steps = init_steps;
    _param_index = 0;
    _tolerance = tolerance;
    _expansion = expansion;
    _shrink = shrink;

     int num_params = init_params.size();
    _trials.resize(num_params, 0);

    // first parameter to try
    _params[0] += _steps[0];
}

// update parameters according to current reward
void Twiddle::update(double reward) {
    // result is improved
    if (reward > _best_reward) {
        // update best reward
        _best_reward = reward;

        // enlarge the exploration step
        _steps[_param_index] *= _expansion;

        // clear the trail counter
        _trials[_param_index] = 0;
    }
    // result is not improved
    else if (_trials[_param_index] == 0){
        // try the opposite direction
        _params[_param_index] -= _steps[_param_index] * 2;
        _trials[_param_index] = 1;
    }
    // if both directions have been tried, shrink the 
    // exploration step   
    else {
        _trials[_param_index] = 0;
        _params[_param_index] += _steps[_param_index];
        _steps[_param_index] *= _shrink;
    }
    
    if (_trials[_param_index] == 0) {
        // move to the next parameter
        if ((++_param_index) == _params.size()) {
            _param_index = 0;
        }
        _params[_param_index] += _steps[_param_index];
    }
}

// get parameters
const vector<double>& Twiddle::getParams() const {
    return _params;
}
    