#include <omnetpp.h>
#include "serverStatusUpdate_m.h"

using namespace omnetpp;


class Buffer : public cSimpleModule
{
protected:

    cQueue queue;

    simsignal_t qlenSignal;
    simsignal_t queueingTimeSignal;
    simsignal_t pDroppedSignal;

    bool LCFS;
    int queueLimit;

    int n_servers;
    std::vector<bool> serverBusy;

public:
    Buffer();
    virtual ~Buffer();

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    //void startPacketService(cMessage *msg);
    void putPacketInQueue(cMessage *msg);
    int getIdleServer();
    void sendPacketToServer(cMessage* msg, int serverId);
    cMessage* getPacketFromQueue();
};

Define_Module(Buffer);


Buffer::Buffer()
{

}

Buffer::~Buffer()
{
}

void Buffer::initialize()
{

    if (par("LCFS")){
        EV << "LCFS mode\n";
    } else {
        EV << "FCFS mode\n";
    }
    LCFS = par("LCFS");
    queueLimit = par("queueLimit");
    n_servers = par("n_servers");

    // init server status with not busy
    for(int s=0; s<n_servers; s++){
        serverBusy.push_back(false);
    }

    queue.setName("queue");

    //signal registering
    qlenSignal = registerSignal("qlen");
    queueingTimeSignal = registerSignal("queueingTime");
    pDroppedSignal = registerSignal("pDropped");

    //initial messages
    emit(qlenSignal, queue.getLength());

}

void Buffer::handleMessage(cMessage *msg)
/*
 * Buffers can either receive messages from the in port (packets) or messages from
 * the out port (server status updates). No self messages are expected
 */
{
    if (msg->isSelfMessage()) {
        // we keep this for future expansions
        return;
    }
    else { //packet from source has arrived

        if(strcmp(msg->getArrivalGate()->getName(),"in")==0){
            // this is a packet to be put in queue
            // Setting arrival timestamp as msg field
            msg->setTimestamp();

            // check if any server is idle
            int idleServerId = getIdleServer();
            if (idleServerId == -1){
                // servers are all busy, put in queue
                putPacketInQueue(msg);
            } else {
                // send packet to idle server
                sendPacketToServer(msg, idleServerId);
                // set server as busy
                serverBusy[idleServerId]=true;
            }
        }
        else if(strcmp(msg->getArrivalGate()->getName(),"out$i")==0){
            // this is a server status update
            ServerStatusUpdate* ssu = (ServerStatusUpdate*) msg;
            // first update server status, then if not empty and if not busy send packet
            serverBusy[ssu->getServerId()] = ssu->getBusy();
            if(!queue.isEmpty() && !ssu->getBusy()){
                // send a packet to the newly idle server
                sendPacketToServer(getPacketFromQueue(), ssu->getServerId());
                serverBusy[ssu->getServerId()] = true;
            }

            // no one needs this message anymore, let's destroy it
            delete ssu;
        }

    }
}

void Buffer::putPacketInQueue(cMessage *msg)
{
    if(queue.getLength() >= queueLimit){
        // drop packet and end event
        emit(pDroppedSignal, 1);
        EV << "Packet dropped due to queue overflow\n";
        delete(msg);
    } else {
        queue.insert(msg);
        emit(qlenSignal, queue.getLength());

        //log new message in queue
        EV << msg->getName() << " enters queue"<< endl;
    }
}

cMessage* Buffer::getPacketFromQueue(){
    cMessage* ret;
    if(LCFS){
        ret = (cMessage *)queue.back();
        queue.remove(queue.back());
    } else {
        ret = (cMessage *)queue.pop();
    }


    //Emit queue len and queuing time for this packet
    emit(qlenSignal, queue.getLength());
    emit(queueingTimeSignal, simTime() - ret->getTimestamp());

    return ret;
}

int Buffer::getIdleServer(){
    for(int s=0; s<n_servers; s++){
        if(!serverBusy[s]){
            return s;
        }
    }
    return -1;
}

void Buffer::sendPacketToServer(cMessage* msg, int serverId){
    send(msg, "out$o",serverId);
}


