/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"
#include <base/logging.h>
#include <base_schilling/Error.hpp>

using namespace alt_imagenex;
using namespace oro_marum;

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
    try{
      delete mDriver;
      mDriver = new alt_imagenex::Driver;
      mDriver->setEcho(_echo_on.get());
      mDriver->setSoundVelocity(_sound_velocity.get());
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
      _log_message.write(LogMessage(e));
      throw e;
    } 
}


bool Task::startHook()
{
  if (! TaskBase::startHook())
    return false;
  return true;
}

void Task::updateHook()
{	
  run();
}

void Task::errorHook()
{
  run();  
  TaskBase::errorHook();
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

void Task::processIO()
{
  int val;
  if (_gain.read(val) == RTT::NewData) {
    mDriver->setGain(val);
  }      
  if (_range.read(val) == RTT::NewData) {
    mDriver->setRange(val);
  }
  mDriver->collectData();
  _alt_samples.write(mDriver->getData());
  AltStatus status = mDriver->getStatus(); 
  _alt_status.write(status);
  if(_echo_on.get()){
    _alt_echo.write(mDriver->getEchoData());
  }
  if(!status.serialStatus){
    _log_message.write(LogMessage(Error,ALTSTR_SERIAL,ALTALARM_SERIAL));
    error(DEV_ERROR);
  }
  else{
    if(DEV_ERROR == state()){
      recover();
    }
  }
}

void Task::run()
{
    try{
      switch(state()){
	case RUNNING: {
	  processIO();
	  state(MONITORING);
	  break;
	}
	case MONITORING:{
	  processIO();
	  break;
	}
	default: break;
      }   
    } catch(std::runtime_error &e){
      LOG_DEBUG("exception %s",e.what());
      _log_message.write(LogMessage(e));
      exception(IO_TIMEOUT);
    }
}
