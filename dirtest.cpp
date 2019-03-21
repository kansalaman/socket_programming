#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<bits/stdc++.h>
using namespace std;
int main(){
    DIR *dp;
    int i=0;
    struct dirent *ep;
    string dirname="user-database/";
    dp=opendir(dirname.c_str());
    if (dp!=NULL){
        while(ep=readdir(dp)) i++;
        closedir(dp);

    }
    else cout<<"couldn't open directory"<<endl;
    cout<<i<<endl;
    return 0;

}