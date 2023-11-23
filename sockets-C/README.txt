                            CS-3205 ASSIGNMENT-1
                            ====================

CONTENTS :-
********
1. connectsock.c
2. passivesock.c
3. errexit.c
4. client files :- UDPecho.c (Q1) , UDPmath.c(Q2) , TCPfile.c(Q3)
5. server files :- UDPechod.c (Q1) , UDPmathd.c(Q2) , TCPfiled.c(Q3)
6. makefile

RUNNING MAKEFILE :-
****************
After extracting the tarred zip file , run on 2 terminals (client and server) by using commands :-

    1.   "make clean"   on any one terminal.
    2.   "make all"     on any one terminal.
    3.   i)   "./UDPechod 5666"  on server terminal.
        ii)   "./UDPecho localhost 5666"  on client terminal.
    Similarly the other questions.

Note :- Here , "5666" is the port number which can be replaced by any other port numbers.
        For ports such as "echo" , "time" use sudo in front of those commands to run.

INPUT FORMATS :-
*************
Q1. For first part , i.e. echo , input is a string (can contain spaces).
Q2. For second part , i.e. math , input is a string (op a b).
Q3. For third part , i.e. file , input is a string (filename N) , here filename and number of bytes are space separated.
    Example: example.txt 10  ==>  filename is "example.txt" and number of bytes is "10".

For all the three parts , terminals can be ended by inputting "exit".

Note :- For third part , only client terminal will end by inputting "exit" but server terminal will run which can be terminated by using "Ctrl C".

Everything else will work fine to the best of my knowledge.
For any other queries , please reach out to me :- cs20b056@smail.iitm.ac.in