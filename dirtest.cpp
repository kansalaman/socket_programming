#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<bits/stdc++.h>
#include<sys/stat.h>
using namespace std;
int main(){
    string fname="test1";
    DIR* udb;
    udb=opendir("test1");
    struct dirent* ep;
    while(ep=readdir(udb)) cout<<ep->d_name<<endl;

    return 0;

}