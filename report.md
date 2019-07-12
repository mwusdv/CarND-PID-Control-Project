# CarND-Controls-PID
Self-Driving Car Engineer Nanodegree Program

---

## Overall Design

* Two PID controllers
    
    * `steer_pid`: controls the steering angle
    * `throttle_pid`: controls throttle value, and thus the speed. The basic idea is that we should reduce the speed when the error is large. To this end, the absolute value of cte is used, insead of the original value of cte that is used in the `steer_pid`.

* Twiddle for parameter optimization: following the course material, I implemented the twiddle algorithm by implmenting a `Twiddle` class. This Twiddle class is responsible for optimizing all the 6 parameters for the two PID controllers mentioned above.


## Semi-automatic Parameter Optimization

In the toy example of the PID course, we can run the twiddle algorithm fully automatically. Because we can fully control the car to run and re-run the whole process with any given parameters. However, in this project, I didn't figure out how to automactially start/restart or end the process of the simulation. Therefore some manual tunning is necessary. 

However, even with manual tunning, I would still follow the twiddle algorithm. To this end, I built a semi-auotmaic twiddle process where minimal manual efforts are needed.

Essentially the process is as the following: given a set of control parameters, if the car runs out of the road, human can hit the "ESC" key to stop the simulation process. This pauses the tunning process, which will be listing to the port 4567 to wait for more inputs. And then human can click the simulator botton to restart the process. At this time, the OnConnection message is received by the tunning process. It will then try the next set of parameters chose by the twiddle algorithm. This way, I just need to click the "ESC" key to pause and clicke the simulation button to restart the parameter optimization process. All the remaining details are handled by the code.

On the other hand, if the car runns within the road, or does not run far away from the road, then we can let the code keep running. The code cacluates the "distance" that the car has covered by accumulating the speed values. Once the distance value reaches 45000, which is approximatley a full lap, the tunning process will restart with the next set of parameters chosen by the twiddle algorithm. Therefore, if the car keeps running within the road, we can let the code keep runng by itself. It will run the twiddle algorithm to optimize the parameters.

Evaluation of the parameters: in order to run the twiddle algorithm, we need to be able to evaluate a given set of parameters, so that we can select the best set of parameters. At the end of each iteration (either triggered by human's hitting the "ESC" key, or the car has covered distance value of 45000), the current set of parameters is evaluated based on the distance the car has covered, the total error and the average speed in the iteration. Specifically,the evaluation score is computed as:

`score = 
min(total_distance, 45000)*W_d - total_error*W_e + mean_speed*W_s`

Where `W_d=10  W_e=2  W_s=1` are the weights of ditance, total error and average speed respectively. The idea is that we expect to run a whole lap, hence initially the ditance is the most important value. Once we can cover a whole lap, we hope the total error can be small, and then we hope the car can run fast.

In the submtted code, I directly set the parameters I have found. The orginal initial values (all zeros) and steps are commented as reference.

## Reflection on the P, I, D components

* **P compoent**. This components is for reducing the instant error. For example, the P component of the `steer_pid` is useful to prevent the car from going outside the road. A too small value of this parameter will cause the car go outside the road at sharp turns. However, a too big value of this parameter can cause overshoots and oscilliations.  The P component of the `throttle_pid` is to reduce the speed when currently having a large error.

* **D component**. This component is helpful to reduce the overshoots and oscilliations caused by the P components. However, a too large value of this parameter can also cause oscilliations the opposite way to the P compoment. And only tunning P and D alone is not enough or at leaset not easy to come up with a set of statisfactory parameters.

* **I compoment**. Comparing with the P compoment which handles the instant error, this componet deals with the error within a period of time. In the course, the error were added across the whole trajectory. However, in this PID control project, the route is much longer. And it may not make too much sense to decide the control parameter using the error many time steps back. So I set a `window_size` parameter and use a queue to record the recent errors. Once the number of elements in the queue reaches `window_size`, the head of the queue will be popped. The `Ki` parameter is computed based on the average error values contained this queue. For `throttle_pid`, this component is helpful to reduce the speed of the car if recently there has been large errors even when the current error is small.

## Result
With the above desribed parameter optimization process, and the implementation of PID controlers, I came up with a set of satisfactory parameters pretty easily: `0.15, 0.009, 1.0, 1.0, 1.0, 1.0`. The first three are for `steer_pid` and the last three are for `throttle_pid`. It can be seen that these parameters are obtained very early during the optimization process without much twiddle search. With this set of parameters, the car can drive the whole lap smoothly, and with an average speed about 29--30 MPH. And I found that it is quite helpful to tune the throttle controler together with the steering controler. Since throttle controler can reduce the speed when the error is large. This can reduce the pressure of finding proper steering values at high speeds.

However, I did try to let the code run by itself longer, trying to get better parameters. The problem I found is that the parameters obtained with more twiddle search this way are not consistent with the true results. For example, I got another set of parameters with much smaller total error indicated by the code. But when I restarted the simulation with this set of parameters, the running was not smooth with many zig-zags. The reason for this is probably that I ketp the code running for twiddle search. And the situation where this set of parameters were obtained was not the same as when I run the simulation from the very beginning: the initial speed and position of the car were probably quite different.





