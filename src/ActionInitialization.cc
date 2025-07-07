#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

ActionInitialization::ActionInitialization()
 : G4VUserActionInitialization()
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
  SetUserAction(new RunAction());
}

void ActionInitialization::Build() const
{
  // Set primary generator
  SetUserAction(new PrimaryGeneratorAction());
  
  // Create and set RunAction
  RunAction* runAction = new RunAction();
  SetUserAction(runAction);
  
  // Create and set EventAction
  EventAction* eventAction = new EventAction();
  SetUserAction(eventAction);
  
  // Create and set SteppingAction
  SteppingAction* steppingAction = new SteppingAction(eventAction);
  SetUserAction(steppingAction);
  
  // Connect stepping action to event action
  //eventAction->SetSteppingAction(steppingAction);
}