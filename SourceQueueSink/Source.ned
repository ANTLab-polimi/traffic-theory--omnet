simple Source
{
    parameters:
        volatile double avgInterArrivalTime @unit(s);
        @display("i=block/source");
    gates:
        output out;
}