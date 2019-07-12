#include <iostream>
#include "DriveStats.h"

using namespace std;

DriveStats::DriveStats(double circle_length, double dist_weight, double error_weight, double speed_weight) 
    : _circle_length (circle_length)
    , _dist_weight   (dist_weight)
    , _error_weight  (error_weight)
    , _speed_weight  (speed_weight) {
    // empty
}

DriveStats::~DriveStats() {
    // empty
}

// update stats
void DriveStats::update(int num_steps, double total_error, double total_dist) {
    _num_steps = num_steps;
    _total_error = total_error;
    _total_dist = total_dist;
    _mean_speed = _total_dist / (_num_steps + 1e-8);
}

 // reward(metric) of drie stats
double DriveStats::reward() const {
    // distance
    double dist_reward = (_total_dist > _circle_length)? _circle_length : _total_dist;
    double reward = dist_reward * _dist_weight - _total_error * _error_weight + _mean_speed * _speed_weight;
    return reward;
}

// // -1: worse, 0: equal, 1: better
int DriveStats::compare(const DriveStats& ds) const {
    double reward1 = reward();
    double reward2 = ds.reward();

    if (reward1 > reward2) {
        return 1;
    }

    if (reward1 < reward2) {
        return -1;
    }

    return 0;
}

// show stats
void DriveStats::print() const {
    cout << "num steps: " << _num_steps << " distance: " << _total_dist \
        << " error: " << _total_error << " speed: " << _mean_speed << endl;
}
