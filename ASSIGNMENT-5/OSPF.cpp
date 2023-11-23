// NAME:R.Neel Prabhanjan
// Roll Number:CS20B056
// Course: CS3205 Jan. 2023 semester
// Lab number: 5
// Date of submission: 28-4-23
// I confirm that the source file is entirely written by me without
// resorting to any dishonest means.
// Website(s) that I used for basic socket programming code are:
// URL(s): GFG

#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

int main(int argc , char* argv[])
{
    int N;
    int i,j,k;
    string input_file;

    //Input file.
    for(i=0 ; i<argc ; i++)
    {
        string s=argv[i];

        if(s=="-f")
        {
            input_file=argv[i+1];
        }
    }

    //Reading the number of routers.
    ifstream in_file(input_file);

    in_file >> N;

    //Forking and creating N child processes i.e. the routers.
    for(int i=0 ; i<N ; i++)
    {
        pid_t pid=fork();

        if(pid<0)
        {
            cerr << "Error creating child process" << endl;
            return 1;
        }
        else if(pid==0)
        {
            //Generating command line arguments for the routers.
            char* args[argc+3];
            args[0]=(char*)("./ROUTER");
            args[1]=(char*)("-i");
            string s=to_string(i);
            char* charPtr=new char[s.length()+1];
            strcpy(charPtr,s.c_str());
            args[2]=charPtr;
            args[argc+2]=nullptr;

            for(j=1 ; j<argc ; j++)
            {
                args[j+2]=argv[j];
            }

            execv(args[0],args);

            cerr << "Error executing other_process" << endl;
            return 1;
        }
    }

    //Wait for routers to complete their execution.
    int status;
    pid_t childPid;
    while((childPid=wait(&status))>0)
    {
        if(WIFEXITED(status))
        {
            //cout << "Child process " << childPid << " exited with status " << WEXITSTATUS(status) << endl;
        }
        else
        {
            //cout << "Child process " << childPid << " terminated abnormally" << endl;
        }
    }

    cout << "All Routers completed. Exiting ..." << endl;
    return 0;
}
