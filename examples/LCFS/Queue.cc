#include <omnetpp.h>

using namespace omnetpp;


class Queue : public cSimpleModule
{
protected:
    cMessage *msgInServer;
    cMessage *endOfServiceMsg;

    cQueue queue;

    simsignal_t qlenSignal;
    simsignal_t busySignal;
    simsignal_t queueingTimeSignal;
    simsignal_t responseTimeSignal;

    double avgServiceTime;

    bool serverBusy;

    bool LCFS;

public:
    Queue();
    virtual ~Queue();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void startPacketService(cMessage *msg);
    void putPacketInQueue(cMessage *msg);
};

Define_Module(Queue);


Queue::Queue()
{
    msgInServer = endOfServiceMsg = nullptr;
}

Queue::~Queue()
{
    delete msgInServer;
    cancelAndDelete(endOfServiceMsg);
}

void Queue::initialize()
{

    if (par("LCFS")){
        EV << "LCFS mode\n";
    } else {
        EV << "FCFS mode\n";
    }
    LCFS = par("LCFS");

    endOfServiceMsg = new cMessage("end-service");
    queue.setName("queue");
    serverBusy = false;

    //signal registering
    qlenSignal = registerSignal("qlen");
    busySignal = registerSignal("busy");
    queueingTimeSignal = registerSignal("queueingTime");
    responseTimeSignal = registerSignal("responseTime");

    //initial messages
    emit(qlenSignal, queue.getLength());
    emit(busySignal, serverBusy);

    //get avgServiceTime parameter
    avgServiceTime = par("avgServiceTime").doubleValue();
}

void Queue::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) { //Packet in server has been processed

        //log service completion
        EV << "Completed service of " << msgInServer->getName() << endl;

        //Send processed packet to sink
        send(msgInServer, "out");

        //emit response time signal
        emit(responseTimeSignal, simTime() - msgInServer->getTimestamp());

        //start next packet processing if queue not empty
        if (!queue.isEmpty()) {
            //Put the next message from the queue inside the server, LCFS aware
            if(LCFS){
                msgInServer = (cMessage *)queue.back();
                queue.remove(queue.back());
            } else {
                msgInServer = (cMessage *)queue.pop();
            }


            //Emit queue len and queuing time for this packet
            emit(qlenSignal, queue.getLength());
            emit(queueingTimeSignal, simTime() - msgInServer->getTimestamp());

            //start service
            startPacketService(msg);
        } else {
            //server is not busy anymore
            msgInServer = nullptr;
            serverBusy = false;
            emit(busySignal, serverBusy);

            //log idle server
            EV << "Empty queue, server goes IDLE" <<endl;
        }

    }
    else { //packet from source has arrived

        //Setting arrival timestamp as msg field
        msg->setTimestamp();

        if (serverBusy) {
            putPacketInQueue(msg);
        }
        else { //server idle, start service right away
            //Put the message in server and start service
            msgInServer = msg;
            startPacketService(msg);

            //server is now busy
            serverBusy=true;
            emit(busySignal, serverBusy);

            //queueing time was ZERO
            emit(queueingTimeSignal, SIMTIME_ZERO);
        }
    }
}

void Queue::startPacketService(cMessage *msq)
{

    //generate service time and schedule completion accordingly
    simtime_t serviceTime = exponential(avgServiceTime);
    scheduleAt(simTime()+serviceTime, endOfServiceMsg);

    //log service start
    EV << "Starting service of " << msgInServer->getName() << endl;

}

void Queue::putPacketInQueue(cMessage *msg)
{
    queue.insert(msg);
    emit(qlenSignal, queue.getLength());

    //log new message in queue
    EV << msg->getName() << " enters queue"<< endl;
}


