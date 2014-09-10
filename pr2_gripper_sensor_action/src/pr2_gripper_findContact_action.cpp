/*
 * Copyright (c) 2009, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \author Joe Romano
 */

// @author: Joe Romano
// @email: joeromano@gmail.com 
// @brief: pr2_gripper_findContact_action.cpp - action server for sending
//         findContact commands to the roobt

#include "ros/ros.h"

#include <actionlib/server/action_server.h>
#include <pr2_gripper_sensor_msgs/PR2GripperFindContactAction.h>
#include <pr2_gripper_sensor_msgs/PR2GripperFindContactData.h>
#include <pr2_gripper_sensor_msgs/PR2GripperFindContactCommand.h>
#include <std_srvs/Empty.h>

class Pr2GripperFindContact
{
private:
  typedef actionlib::ActionServer<pr2_gripper_sensor_msgs::PR2GripperFindContactAction> GAS;
  typedef GAS::GoalHandle GoalHandle;
public:
  Pr2GripperFindContact(ros::NodeHandle &n) :
    node_(n),
    action_server_(node_, "find_contact",
                   boost::bind(&Pr2GripperFindContact::goalCB, this, _1),
                   boost::bind(&Pr2GripperFindContact::cancelCB, this, _1)),
    has_active_goal_(false)
  {
    ros::NodeHandle pn("~");

    // we will listen for messages on "state" and send messages on "find_contact"
    pub_controller_command_ =
      node_.advertise<pr2_gripper_sensor_msgs::PR2GripperFindContactCommand>("find_contact", 1);
    sub_controller_state_ =
      node_.subscribe("contact_state", 1, &Pr2GripperFindContact::controllerStateCB, this);

    watchdog_timer_ = node_.createTimer(ros::Duration(1.0), &Pr2GripperFindContact::watchdog, this);

  }

  ~Pr2GripperFindContact()
  {
    pub_controller_command_.shutdown();
    sub_controller_state_.shutdown();
    watchdog_timer_.stop();
  }

private:

  ros::NodeHandle node_;
  GAS action_server_;
  ros::Publisher pub_controller_command_;
  ros::Subscriber sub_controller_state_;
  ros::Timer watchdog_timer_;

  bool has_active_goal_;
  GoalHandle active_goal_;
  ros::Time goal_received_;

  double stall_timeout_;
  ros::Time last_movement_time_;
  ros::Time action_start_time;


  void watchdog(const ros::TimerEvent &e)
  {
    ros::Time now = ros::Time::now();

    // Aborts the active goal if the controller does not appear to be active.
    if (has_active_goal_)
    {
      bool should_abort = false;
      if (!last_controller_state_)
      {
        should_abort = true;
        ROS_WARN("Aborting goal because we have never heard a controller state message.");
      }
      else if ((now - last_controller_state_->stamp) > ros::Duration(5.0))
      {
        should_abort = true;
        ROS_WARN("Aborting goal because we haven't heard from the controller in %.3lf seconds",
                 (now - last_controller_state_->stamp).toSec());
      }

      if (should_abort)
      {
        // Marks the current goal as aborted.
        active_goal_.setAborted();
        has_active_goal_ = false;
      }
    }
  }

  void goalCB(GoalHandle gh)
  {
    // Cancels the currently active goal.
    if (has_active_goal_)
    {
      // Marks the current goal as canceled.
      active_goal_.setCanceled();
      has_active_goal_ = false;
    }

    gh.setAccepted();
    active_goal_ = gh;
    has_active_goal_ = true;
    goal_received_ = ros::Time::now();

    // update our zero values for 0.25 seconds
    if(active_goal_.getGoal()->command.zero_fingertip_sensors)
    {
      std_srvs::Empty::Request req;
      std_srvs::Empty::Response resp;
      if(ros::service::exists("zero_fingertip_sensors",true))
      {
        ROS_INFO("updating zeros!");
        ros::service::call("zero_fingertip_sensors",req,resp);
      }
    }
    
    // Sends the command along to the controller.
    pub_controller_command_.publish(active_goal_.getGoal()->command);
    
    last_movement_time_ = ros::Time::now();
    action_start_time = ros::Time::now();
  }

  void cancelCB(GoalHandle gh)
  {
    
    if (active_goal_ == gh)
    {
      // stop the real-time motor control
      std_srvs::Empty::Request req;
      std_srvs::Empty::Response resp;
      if(ros::service::exists("stop_motor_output",true))
      {
        ROS_INFO("Stopping Motor Output");
        ros::service::call("stop_motor_output",req,resp);
      }

      // Marks the current goal as canceled.
      active_goal_.setCanceled();
      has_active_goal_ = false;
    }
  }


  pr2_gripper_sensor_msgs::PR2GripperFindContactDataConstPtr last_controller_state_;
  void controllerStateCB(const pr2_gripper_sensor_msgs::PR2GripperFindContactDataConstPtr &msg)
  {
    last_controller_state_ = msg;
    ros::Time now = ros::Time::now();

    if (!has_active_goal_)
      return;

    pr2_gripper_sensor_msgs::PR2GripperFindContactGoal goal;
    goal.command = active_goal_.getGoal()->command;

    pr2_gripper_sensor_msgs::PR2GripperFindContactFeedback feedback;
    feedback.data = *msg;

    pr2_gripper_sensor_msgs::PR2GripperFindContactResult result;
    result.data = *msg;

    // do not check until some dT has expired from message start
    double dT = 0.2;
    if(feedback.data.stamp.toSec() < action_start_time.toSec()+dT ){}


    // if we are actually in a find_contact control state or positoin control state
    else if(feedback.data.rtstate.realtime_controller_state == 5 || feedback.data.rtstate.realtime_controller_state == 3)
    {
      if(feedback.data.contact_conditions_met)
      {
        active_goal_.setSucceeded(result);
        has_active_goal_ = false;
      }
    }

    else
      has_active_goal_ = false;

    active_goal_.publishFeedback(feedback);
  }
};


int main(int argc, char** argv)
{
  ros::init(argc, argv, "find_contact_node");
  ros::NodeHandle node;
  Pr2GripperFindContact jte(node);

  ros::spin();

  return 0;
}
