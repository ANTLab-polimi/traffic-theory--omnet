simple Source
{
    parameters:
        volatile double interArrivalTime @unit(s);
        @display("i=block/source");
    gates:
        output out;
}