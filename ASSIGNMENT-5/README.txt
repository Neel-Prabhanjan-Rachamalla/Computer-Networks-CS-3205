                                ASSIGNMENT-5
                                ============

The purpose of this lab is to implement a simplified version of the Open Shortest Path First (OSPF) routing
protocol.

Given a set of N routers, the goal is for EACH router to: (a) exchange HELLO packets with neighbours,
(b) create Link State Advertisement (LSA) packets based on neighboring nodes’ info, (c) broadcast the
LSA packets to all other routers in the network, (d) construct the network topology based on the LSA
packets received from other routers, and (e) determining the routing table entries based on this topology,
by using Dijkstra’ algorithm (single source – all nodes shortest paths). If multiple equal-cost paths exist,
any one of them can be reported.

The HELLO packets will be exchanged every x seconds; the LSA updates will be sent every y seconds;
the routing table computation will be done every z seconds.

1. Contents of the folder:- Makefile, Input file, Output file, Comments file, Report, OSPF.cpp, ROUTER.cpp

2. Running the code:-
    Firstly, use the makefile to compile the two source codes by command "make all".
    For removing the executable files use the command "make clean".
    Run the executable on terminal as :-
    ./OSPF -f input2.txt -o output.txt -h 5 -a 10 -s 20

Note:- Here the meaning of each corresponding flags are given in the assignment pdf.

Please contact for any queries :- cs20b056@smail.iitm.ac.in
