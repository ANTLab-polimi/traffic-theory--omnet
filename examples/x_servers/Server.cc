#include <omnetpp.h>
#include "serverStatusUpdate_m.h"
#include <assert.h>

using namespace omnetpp;


class Server : public cSimpleModule
{
protected:
    cMessage *msgInServer;
    cMessage *endOfServiceMsg;

    simsignal_t busySignal;
    simsignal_t responseTimeSignal;

    double avgServiceTime;
    bool serverBusy;

    ServerStatusUpdate busy_su;
    ServerStatusUpdate idle_su;

public:
    Server();
    virtual ~Server();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void startPacketService(cMessage *msg);
    void putPacketInQueue(cMessage *msg);
};

Define_Module(Server);


Server::Server()
{
    msgInServer = endOfServiceMsg = nullptr;
}

Server::~Server()
{
    delete msgInServer;
    cancelAndDelete(endOfServiceMsg);
}

void Server::initialize()
{
    endOfServiceMsg = new cMessage("end-service");
    serverBusy = false;

    //signal registering
    busySignal = registerSignal("busy");
    responseTimeSignal = registerSignal("responseTime");

    //initial messages
    emit(busySignal, serverBusy);

    //get avgServiceTime parameter
    avgServiceTime = par("avgServiceTime").doubleValue();

    // init status update messages
    busy_su.setServerId(this->getIndex());
    busy_su.setBusy(true);
    idle_su.setServerId(this->getIndex());
    idle_su.setBusy(false);
}

void Server::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) { //Packet in server has been processed

        //log service completion
        EV << "Completed service of " << msgInServer->getName() << endl;

        //Send processed packet to sink
        send(msgInServer, "out");

        //emit response time signal
        emit(responseTimeSignal, simTime() - msgInServer->getTimestamp());

        // send status update to queue
        serverBusy = false;

        //server is not busy anymore
        msgInServer = nullptr;
        serverBusy = false;
        emit(busySignal, serverBusy);

        //log idle server
        EV << "Empty queue, server goes IDLE" <<endl;

        // send status update
        send(idle_su.dup(), "in$o");
    }
    else { // packet from buffer
        assert(!serverBusy);

        //Setting arrival timestamp as msg field
        msg->setTimestamp();
        //Put the message in server and start service
        msgInServer = msg;
        startPacketService(msg);

        //server is now busy
        serverBusy=true;
        emit(busySignal, serverBusy);
        send(busy_su.dup(), "in$o");
    }
}

void Server::startPacketService(cMessage *msq)
{

    //generate service time and schedule completion accordingly
    simtime_t serviceTime = exponential(avgServiceTime);
    scheduleAt(simTime()+serviceTime, endOfServiceMsg);

    //log service start
    EV << "Starting service of " << msgInServer->getName() << endl;

}



