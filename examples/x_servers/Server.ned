simple Server
{
    parameters:
        volatile double avgServiceTime @unit(s);
        
        @signal[busy](type="bool");
        @signal[responseTime](type="simtime_t");
        
        @statistic[busy](title="server busy state";record=vector?,timeavg;interpolationmode=sample-hold);
        @statistic[responseTime](title="response time at end of service";unit=s;record=vector,mean,max;interpolationmode=none);
    	
    gates:
        inout in;
        output out;
}