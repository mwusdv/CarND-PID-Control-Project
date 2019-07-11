#ifndef _TWIDDLE_H_
#define TWICDLE_H_

#include <vector>

using std::vector;

class Twiddle {
public:
    Twiddle();
    ~Twiddle();

    // start optimal parameters
    void init(const vector<double>& init_params, const vector<double>& init_steps, 
              double tolerance=0.2, double expansion=1.1, double shrink=0.9);

    // update parameters according to current reward
    void update(double reward);

    // get parameters
    const vector<double>& getParams() const;

    // done with parameter optimization?
    bool done();

    // show parameters
    void showParams() const;
    
protected:
    double _tolerance;
    vector<double> _params;
    vector<double> _steps;
    vector<int> _trials;
    int _param_index;

    double _best_reward;

    double _expansion;
    double _shrink;
};

#endif