#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"
#include "twiddle.h"

// for convenience
using nlohmann::json;

using namespace std;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
string hasData(string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != string::npos) {
    return "";
  }
  else if (b1 != string::npos && b2 != string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

// initialize pid
PID steer_pid;
PID throttle_pid;

const double circle_dist = 50000;

// initialize twiddle for parameter optimization
Twiddle twiddle(circle_dist);
int num_steps = 0;
double total_error = 0;
double total_dist = 0;
double base_throttle = 0.8;


void showStatus(const Twiddle& twiddle) {
  cout << "Parameters: " << endl;
  twiddle.showParams();

  cout << "Best stats: " << endl;
  twiddle.showBestStats();

  cout << "Current stats: " << endl;
  twiddle.showStats();

  cout << endl;
}

void reset(const Twiddle& twiddle, PID& steer_pid, PID& throttle_pid) {
  num_steps = 0;
  total_error = 0;
  total_dist = 0;

  showStatus(twiddle);  

  // set PID parameters
  const vector<double>& params = twiddle.getParams();
  steer_pid.Init(params[0], params[1], params[2]);
  throttle_pid.Init(params[3], 0, params[4]);
}

int main() {
  uWS::Hub h;

  h.onMessage([&steer_pid, &throttle_pid](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, 
                     uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {
      auto s = hasData(string(data).substr(0, length));

      if (s != "") {
        auto j = json::parse(s);

        string event = j[0].get<string>();

        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<string>());
          double speed = std::stod(j[1]["speed"].get<string>());
          double angle = std::stod(j[1]["steering_angle"].get<string>());
          
          /**
           * TODO: Calculate steering value here, remember the steering value is
           *   [-1, 1].
           * NOTE: Feel free to play around with the throttle and speed.
           *   Maybe use another PID controller to control the speed!
           */

          // update PIDs
          steer_pid.UpdateError(cte);
          throttle_pid.UpdateError(fabs(cte));

          // control values
          double steer_value = max(-1.0, min(1.0, steer_pid.ControlValue()));
          double throttle = max(0.1, min(1.0, base_throttle + throttle_pid.ControlValue()));

          // update status variables
          ++num_steps;
          total_error += fabs(cte);
          total_dist += speed;
        
          // remind to restart the driving process manually
          if (fabs(cte) >= 2.3) {
            std::cout << "CTE: " << cte << std::endl;
          }

          // finish one circle, restart the counting process
          if (total_dist >= circle_dist) {
            twiddle.update(num_steps, total_error, total_dist);

            // reset
            reset(twiddle, steer_pid, throttle_pid);    
          }

          if (twiddle.done()) {
            cout << "Done!" << endl;
            showStatus(twiddle);
            exit(0);
          }

          // DEBUG
          //cout << "CTE: " << cte << " Steering Value: " << steer_value << " throttle: " << throttle
          //     << " Speed: " << speed << endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = throttle;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          //std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }  // end "telemetry" if
      } else {
        // Manual driving
        string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }  // end websocket message if
  }); // end h.onMessage

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;

    if(num_steps > 0) {
      twiddle.update(num_steps, total_error, total_dist);
    }
    else {
      double tolerance = 0.2;
      twiddle.init(vector<double>(5, 0.0), vector<double>(5, 0.2), tolerance);
    }

    // reset
    reset(twiddle, steer_pid, throttle_pid);
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, 
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  
  h.run();
}

