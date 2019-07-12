#ifndef DRIVE_STATS_   
#define DRIVE_STATS_

class DriveStats {
public:
    DriveStats(double circle_dist, double dist_weight=10, double error_weight=2, double speed_weight=1);
    ~DriveStats();

    // update stats
    void update(int num_steps, double total_error, double total_dist);

    // -1: worse, 0: equal, 1: better
    int compare(const DriveStats& ds) const;

    // reward(metric) of drie stats
    double reward() const;

    // show parameters
    void print() const;

public:
    double _circle_length;

    int _num_steps;
    double _total_dist;
    double _total_error;
    double _mean_speed;

    // weights for computing the reward
    double _dist_weight;
    double _error_weight;
    double _speed_weight;
};

#endif