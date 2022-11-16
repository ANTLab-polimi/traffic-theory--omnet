simple Buffer
{
    parameters:
        bool LCFS;
        int queueLimit;
        int n_servers = default(2);
        @display("i=block/queue;q=queue");
        
        @signal[qlen](type="long");
        @signal[queueingTime](type="simtime_t");
        @signal[pDropped](type="int");
        
        @statistic[qlen](title="queue length";record=vector,timeavg,max;interpolationmode=sample-hold);
        @statistic[queueingTime](title="queueing time at dequeue";unit=s;record=vector,mean,max;interpolationmode=none);
    	@statistic[pDropped](title="packet drops vector"; record=vector,stats;);
    	
    gates:
        input in;
        inout out[];
}