#include <omnetpp.h>

using namespace omnetpp;


class Sink : public cSimpleModule
{
  private:
    simsignal_t lifetimeSignal;
    simsignal_t arrivedMsgSignal;
    int nb_arrivedMsg;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Sink);


void Sink::initialize()
{
    lifetimeSignal = registerSignal("lifetime");
    arrivedMsgSignal = registerSignal("arrivedMsg");
    nb_arrivedMsg = 0;
}

void Sink::handleMessage(cMessage *msg)
{
    //compute packet lifetime
    simtime_t lifetime = simTime() - msg->getCreationTime();
    emit(lifetimeSignal, lifetime);

    //log lifetime
    EV << "Received " << msg->getName() << ", lifetime: " << lifetime << "s" << endl;


    nb_arrivedMsg ++;
    emit(arrivedMsgSignal, nb_arrivedMsg);

    delete msg;
}
