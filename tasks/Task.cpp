/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"
#include <base/logging.h>

using namespace alt_imagenex;

Task::Task(std::string const& name)
    : TaskBase(name),mDriver(0)
{
  
}

Task::Task(std::string const& name, RTT::ExecutionEngine* engine)
    : TaskBase(name, engine),mDriver(0)
{
  
}

Task::~Task()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Task.hpp for more detailed
// documentation about them.

bool Task::configureHook()
{
    try{delete mDriver;
      mDriver = new alt_imagenex::Driver;
      if (!_io_port.get().empty())
      {
	  mDriver->open(_io_port.get());
      }
  
      setDriver(mDriver);
  
      if (! TaskBase::configureHook())
	  return false;
      return true;
    } catch(std::runtime_error &e){
      LOG_DEBUG("exception %s",e.what());
      error(COMM_ERROR);      
      return false;
    } 
}


bool Task::startHook()
{
    try{
      if (! TaskBase::startHook())
	  return false;
      return true;
    } catch(std::runtime_error &e){
      LOG_DEBUG("exception %s",e.what());
      error(COMM_ERROR);      
      return false;
    }
}

void Task::updateHook()
{
    try{
      int val;
      if (_gain.read(val) == RTT::NewData) {
	mDriver->setGain(val);
      }      
      if (_range.read(val) == RTT::NewData) {
	mDriver->setRange(val);
      }
      mDriver->collectData();
      _alt_samples.write(mDriver->getData());
      _alt_status.write(mDriver->getStatus());
      TaskBase::updateHook();
    } catch(std::runtime_error &e){
      LOG_DEBUG("exception %s",e.what());
      error(COMM_ERROR);      
    }
}

void Task::errorHook()
{
    TaskBase::errorHook();
    recover();
}

void Task::stopHook()
{
    TaskBase::stopHook();
}

void Task::cleanupHook()
{
  if(mDriver){
    mDriver->close();
  }  
  TaskBase::cleanupHook();
}

