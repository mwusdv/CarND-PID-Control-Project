#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"

// for convenience
using nlohmann::json;
using std::string;

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

int best_num_steps = 0;
double best_total_error = 0;

int num_steps = 0;
double total_error = 0;
double dp[3] = {0.1, 0.1, 0.1};
double params[3] = {0, 0, 0};
int trial[3] = {0, 0, 0};
int pidx = 0;
PID pid;


int main() {
  uWS::Hub h;
  

 
  /**
   * TODO: Initialize the pid variable.
   */
  
 

  h.onMessage([&pid](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, 
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

          pid.UpdateError(cte);
          double steer_value = pid.ControlValue();
          ++num_steps;
          total_error = pid.TotalError();

          /**
           * TODO: Calculate steering value here, remember the steering value is
           *   [-1, 1].
           * NOTE: Feel free to play around with the throttle and speed.
           *   Maybe use another PID controller to control the speed!
           */
          
          // DEBUG
          //std::cout << "CTE: " << cte << " Steering Value: " << steer_value 
          //          << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = 0.3;
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
    std::cout << "num_steps: " << num_steps << " best_num_steps: " << best_num_steps << std::endl;

    if(num_steps > 0) {
      if (num_steps > best_num_steps || (num_steps == best_num_steps && total_error < best_total_error)) {
        best_num_steps = num_steps;
        best_total_error = total_error;

        dp[pidx] *= 1.1;
        trial[pidx] = 0;
      }
      else if (num_steps < best_num_steps || (num_steps == best_num_steps && total_error > best_total_error)) {
        params[pidx] -= dp[pidx];
        dp[pidx] = -dp[pidx];
        trial[pidx] += 1;
        if (trial[pidx] == 2) {
          dp[pidx] *= 0.9;
          trial[pidx] = 0;
        }
      }
      ++pidx;
      if (pidx == 3) {
        pidx = 0;
      }
    }


    num_steps = 0;
    total_error = 0;
    params[pidx] += dp[pidx];
    std::cout << "param: " << params[0] << " " << params[1] << " " << params[2] << std::endl;
    pid.Init(params[0], params[1], params[2]);

   
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