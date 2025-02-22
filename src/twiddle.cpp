#include <numeric>
#include <iostream>

#include "twiddle.h"

using namespace std;

Twiddle::Twiddle(double circle_dist)
    : _params      ()
    , _steps       ()
    , _best_params ()
    , _best_steps  ()
    , _trials      ()
    , _param_index (0)
    , _best_stats  (circle_dist)
    , _curr_stats  (circle_dist)
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
    _best_params = init_params;
    _best_steps = init_steps;
    
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
void Twiddle::update(int num_steps, double total_error, double total_dist) {
    _curr_stats.update(num_steps, total_error, total_dist);
    int result = _curr_stats.compare(_best_stats);

    // result is improved
    if (result == 1) {
        // update best stats
        _best_stats = _curr_stats;
        _best_params = _params;
        _best_steps = _steps;

        // enlarge the exploration step
        _steps[_param_index] *= _expansion;

        // clear the trail counter
        _trials[_param_index] = 0;
    }
    // result is not improved, try another direction, but ignore negative parameters
    else if (_trials[_param_index] == 0 && _params[_param_index] > _steps[_param_index]*2){
        // try the opposite direction
        _params[_param_index] -= _steps[_param_index] * 2;
        _trials[_param_index] = 1;
    }
    // if both directions have been tried, shrink the 
    // exploration step   
    else {
        double flag = _trials[_param_index] > 0? 1 : -1;
        _trials[_param_index] = 0;
        _params[_param_index] += _steps[_param_index]*flag;
        _steps[_param_index] *= _shrink;
    }
    
    if (_trials[_param_index] == 0) {
        // move to the next parameter
        if ((++_param_index) == _params.size()) {
            _param_index = 0;
        }
        //  parameter cannot be negative
        _params[_param_index] += _steps[_param_index];
    }
}

// get parameters
const vector<double>& Twiddle::getParams() const {
    return _params;
}

bool Twiddle::done() {
    double sum = accumulate(_steps.begin(), _steps.end(), 0.0);
    return (sum < _tolerance);
}

// show parameters
void Twiddle::showParams() const {
    cout << "params: " << endl;
    for (size_t i = 0; i < _params.size(); ++i) {
        cout << _params[i] << " ";
    }
    cout << endl;

    cout << "steps: " << endl;
    for (size_t i = 0; i < _steps.size(); ++i) {
        cout << _steps[i] << " ";
    }
    cout << endl;

    cout << "Best params: " << endl;
    for (size_t i = 0; i < _best_params.size(); ++i) {
        cout << _best_params[i] << " ";
    }
    cout << endl;

    cout << "Best steps: " << endl;
    for (size_t i = 0; i < _best_steps.size(); ++i) {
        cout << _best_steps[i] << " ";
    }
    cout << endl;
}

// show current stats
void Twiddle::showStats() const {
    _curr_stats.print();
}

// show the best stats
void Twiddle::showBestStats() const {
    _best_stats.print();
}