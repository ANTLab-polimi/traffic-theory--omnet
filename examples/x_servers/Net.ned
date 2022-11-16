network Net
{
        @display("bgb=818,273");
        int n_servers = default(2);
    submodules:
        source1: Source {
            parameters:
                @display("p=89,100");
        }
        server[n_servers]: Server {
            parameters:
                @display("p=436.6875,99.667496,c,80");
        }
        buffer1: Buffer {
            parameters:
                n_servers = n_servers;
                @display("p=329,100");
        }
        sink1: Sink {
            parameters:
                @display("p=569,100");
        }
    connections:
        source1.out --> buffer1.in;
        // connect buffer out to all servers and server out ports to sink 
        for i=0..n_servers-1 {
                buffer1.out++ <--> server[i].in;
                server[i].out --> sink1.in++;
        }
}
