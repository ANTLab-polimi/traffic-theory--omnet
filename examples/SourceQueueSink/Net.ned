network Net
{
    @display("bgb=818,273");
    submodules:
        source1: Source {
            parameters:
                @display("p=89,100");
        }
        queue1: Queue {
            parameters:
                @display("p=329,100");
        }
        sink1: Sink {
            parameters:
                @display("p=569,100");
        }
    connections:
        source1.out --> queue1.in;
        queue1.out --> sink1.in;
}
