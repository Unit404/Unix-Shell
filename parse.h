#ifndef PARSE_H
#define PARSE_H
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
#include <regex>
#include <chrono>
using namespace std;

string strip(string in) //remove all leading and trailing whitespace from a string
{
    while(isspace(in.front()))
    {
        in.erase(in.begin());
    }

    while(isspace(in.back()))
    {
        in.pop_back();
    }

    return in;
}
char** vec2arr(const vector<string> &commands, char** args)
{

    for(int i = 0;i<commands.size();i++)
    {
        args[i]=(char*)commands[i].c_str();
    }
    for(int i = commands.size(); i<256; i++)
    {
        args[i]=NULL;
    }
    return args;
}
vector<string> split(string _input, char del) //split the given string by the given character
{
    vector<string> vec;
    string arg="";
    char c;
    bool inDQuote=false;
    bool inSQuote=false;
    for(int i = 0; i<_input.size();i++)
    {
        c=_input[i];
        if(c=='\"'&&!inSQuote) // if we have found a double quote and are not inside of a single quote
            inDQuote=!inDQuote;
        
        if(c=='\'' && !inDQuote) // if we have found a single quote and are not inside of a double quote
            inSQuote=!inSQuote;


        if(c==del &&!inDQuote&&!inSQuote) //if we are at the character we want to split, AND we are not in either quote
        {
            vec.push_back(strip(arg));
            arg="";
        }
        else
        {
            arg.push_back(c);
        }
    }
    vec.push_back(strip(arg));
    return vec;    
}

bool parseCD(string& command, string& _history)
{
    vector<string> dirCom=split(command, ' ');
    string newDir=strip(dirCom[1]);
    char dir[256];
    string cwd(getcwd(dir,256));
    string oldHist=_history;
    if(newDir=="-")
    { 
        newDir=_history;
        _history=cwd;
        cerr<<newDir<<endl;
    }
    else
    {
        _history=cwd;
    }
    
    int succ = chdir(newDir.c_str());
    if(succ!=0)
    {
        _history=oldHist;
        return false;
    }
    else
    {
        return true;
    }
}


bool IOredirect(string& command)
{
    vector<string> rdrct = split(command, '>');

    if(rdrct.size()>1)
    {
        string args=strip(rdrct[0]);
        string file=strip(rdrct[1]);
        command=args;
        int fd = open(file.c_str(),O_WRONLY|O_CREAT|O_RDONLY, S_IWUSR|S_IRUSR);
        dup2(fd,1);
        close(fd);
    }

    rdrct = split(command, '<');
    if(rdrct.size()>1)
    {
        string args=strip(rdrct[0]);
        string file=strip(rdrct[1]);
        command=args;
        int fd = open(file.c_str(), O_RDONLY, S_IWUSR|S_IRUSR);
        dup2(fd,0);
        close(fd);
    }
    return true;
}

bool stripQs(vector<string>& commands)
{
    for(int Q = 0; Q <commands.size(); Q++)
    {
        if(commands[Q].front()=='\"' ||commands[Q].front()=='\'')
        {
            commands[Q].erase(commands[Q].begin());

        }
        if(commands[Q].back()=='\"' ||commands[Q].back()=='\'')
        {
            commands[Q].pop_back();
        }
    }
    return true;
}








#endif