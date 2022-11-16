#include <omnetpp.h>

using namespace omnetpp;


class Source : public cSimpleModule
{
  private:
    cMessage *sendMessageEvent;
    int nbGenMessages;

    double avgInterArrivalTime;

  public:
    Source();
    virtual ~Source();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Source);

Source::Source()
{
    sendMessageEvent = nullptr;
}

Source::~Source()
{
    cancelAndDelete(sendMessageEvent);
}

void Source::initialize()
{
    sendMessageEvent = new cMessage("sendMessageEvent");
    nbGenMessages = 0;

    //get avg interarrival time
    avgInterArrivalTime = par("avgInterArrivalTime").doubleValue();

    //start sending packets
    scheduleAt(simTime(), sendMessageEvent);
}

void Source::handleMessage(cMessage *msg)
{
    //generate packet name
    char msgname[20];
    sprintf(msgname, "message-%d", ++nbGenMessages);

    //generate and send the packet out to the queue
    cMessage *message = new cMessage(msgname);
    send(message, "out");

    //schedule next packet
    scheduleAt(simTime()+exponential(avgInterArrivalTime), sendMessageEvent);
}
