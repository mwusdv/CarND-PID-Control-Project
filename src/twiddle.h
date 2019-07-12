#ifndef TWIDDLE_H_
#define TWIDDLE_H_

#include <vector>
#include "DriveStats.h"

using std::vector;

class Twiddle {
public:
    Twiddle(double circle_dist);
    ~Twiddle();

    // start optimal parameters
    void init(const vector<double>& init_params, const vector<double>& init_steps, 
              double tolerance=0.2, double expansion=1.1, double shrink=0.9);

    // update parameters according to current reward
    void update(int num_steps, double total_error, double total_dist);

    // get parameters
    const vector<double>& getParams() const;

    // done with parameter optimization?
    bool done();

    // show parameters
    void showParams() const;

    // show stats
    void showStats() const;
    void showBestStats() const;
    
protected:
    vector<double> _params;
    vector<double> _steps;

    // record best parameters (for early stop)
    vector<double> _best_params;
    vector<double> _best_steps;

    vector<int> _trials;
    size_t _param_index;

    DriveStats _best_stats;
    DriveStats _curr_stats;

    double _tolerance;
    double _expansion;
    double _shrink;
};

#endif