network Net
{
    submodules:
        gen: Source {
            parameters:
                @display("p=89,100");
        }
        sink: Sink {
            parameters:
                @display("p=329,100");
        }
    connections:
        gen.out --> {delay=300ms;} --> sink.in;
}
