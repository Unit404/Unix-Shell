#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>
#include <signal.h>
#include <sys/time.h>
#include <cassert>
#include <assert.h>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <list>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <sstream>
#include <fcntl.h>
#include <stdlib.h>
#include "parse.h"
#include <regex>
using namespace std;

bool execute(string command, bool exp, int extp[2], vector<int>& children, string& history)
{
    
    bool background=false;
    command=strip(command);

    for(int c = 0; c< children.size(); c++)
    {
        if(waitpid(children[c],0,WNOHANG)==-1)
        {
            children.erase(children.begin()+c);
        }
    }
    vector<string> pipes=split(command,'|');

    for(int i =0;i<pipes.size();i++)
    {
        int fds[2];
        command=strip(pipes[i]);
        vector<string> backps = split(command, '&');
        if(backps.size()>1)
        {
            background=true;
            command=backps[0];
        }
        pipe(fds);
        if(command.find("cd")==0)
        {
            parseCD(command, history);
            return true;
        }
        int a = fork();
        children.push_back(a);
        if(a==0)
        {
          
            IOredirect(command);
            if(i!=pipes.size()-1) //if we are going to pipe into another command
            {
                dup2(fds[1], 1);
            }
            vector<string> data=split(command, ' ');
            int argc=data.size();
            stripQs(data);
            char* argv[256];
            vec2arr(data, argv);
            close(fds[1]);
            execvp(argv[0],argv);
            return false;
        }
        else
        {
            if(background && !exp)
            {
                children.push_back(a);
            }
            else
            {
                if(i==pipes.size()-1)
                    waitpid(a,0,0);
                else
                    children.push_back(a);
            }

            dup2(fds[0],0);
            close(fds[1]);
        }
    }
    return true;
}
int main()
{
    string input;
    int in = dup(0);
    char dir[256];
    vector<int> bgProcs;
    string history;
    int temp[2];
    pipe(temp);
    while(true)
    {
        dup2(in,0);
        time_t t=time(nullptr);
        char *name = getenv("USER");
        cout<<put_time(localtime(&t), "%F %T")<<" USER:"<<name<<" "<<getcwd(dir,256)<<"$ ";
 
        getline(cin, input);
        if(input=="exit")
        {
            cout<<"Terminating Shell"<<endl;
            break;
        }
        if(!execute(input, false, temp, bgProcs, history))
        {
            break; //if the command failed, exit
        }
    }
}
