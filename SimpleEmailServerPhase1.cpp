#include<bits/stdc++.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#define BACKLOG 2
using namespace std;
int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr,"Error: 2 arguments after the executable name are required (portNum and passwdfile) but given %d\n",argc-1);
        exit(1);
    }
    int myport;
    sscanf(argv[1],"%d",&myport);
    int sockfd;
    sockfd=socket(PF_INET,SOCK_STREAM,0);
    sockaddr_in my_addr;
    my_addr.sin_family=AF_INET;
    my_addr.sin_port=htons(myport);
    my_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    // cout<<my_addr.sin_addr.s_addr<<endl;
    memset(&(my_addr.sin_zero),'\0',8);
    int succ=bind(sockfd,(struct sockaddr*) &my_addr,sizeof(struct sockaddr));
    if(succ==0){
        printf("BindDone: %d\n",myport);
    }
    else{
        fprintf(stderr,"Error: in binding to the port\n");
        exit(2);
    }
    // string passwd_file_addr_str(passwd_file_addr);
    // printf("hello %s",argv[2]);
    // printf("here\n");
    ifstream passfile(argv[2]);
    // printf("here\n");
    if(!passfile.good()){
        fprintf(stderr,"Error: Either the input file does not exist or is not readable\n");
        exit(3);
    }
    // else{
    //     // cout<<"file is good"<<endl;
    // }
    int listen_success=listen(sockfd, BACKLOG);
    
    if(listen_success != -1){
        printf("ListenDone: %d\n",myport);
    }
    sockaddr_in their_addr;
    // sockaddr_in* cpointer=&client_addr;
    socklen_t sin_size=sizeof(struct sockaddr_in);
    int client_sockfd=accept(sockfd,(struct sockaddr*) &their_addr,&sin_size);
    if(client_sockfd!=-1){
        cout<<"Client: "<<inet_ntoa(their_addr.sin_addr)<<":"<<ntohs(their_addr.sin_port)<<endl;

    }
    string user,passwd;
    map<string,string> user_password_map;
    while(passfile>>user>>passwd){
        user_password_map[user]=passwd;
    }
    // cout<<user_password_map.size()<<endl;
    char read_message[100000];
    //??What should be length of buffer read.
    int length_read=recv(client_sockfd,read_message,100000,0);
    // cout<<length_read<<endl;
    if(length_read==0){
        cout<<"Sender has closed connection"<<endl;
    }
    else if(length_read==-1){
        cout<<"Error in reading"<<endl;
    }
    // read_message="User: user1 Pass: Mh:21drrz";
    regex user_pass_format("User: (.*) Pass: (.*)\0");
    smatch match;
    string read_message_str(read_message);
    read_message_str=read_message_str.substr(0,length_read);
    // cout<<"85: "<<read_message_str<<endl;
    // printf("86: %s, %d",read_message,length_read);
    if (regex_search(read_message_str,match,user_pass_format)==true){
        const char* uname=match.str(1).c_str();
        const char* pwd=match.str(2).c_str();
        string uname_str=match.str(1);
        string pwd_str=match.str(2);
        if(user_password_map.find(uname_str)!=user_password_map.end()){
            if(user_password_map[uname_str]==pwd_str){
                printf("Welcome %s\n",uname);
                // printf("Welcome %s\n",uname);
                string message="Welcome "+uname_str+"\n"+"\0";
                // printf("--%s--\n",uname);
                const char* message_sent=message.c_str();
                // cout<<"here"<<endl;
                // sprintf(message_sent,"Welcome %s\n",uname);
                // cout<<message.size()<<endl;
                send(client_sockfd,message_sent,strlen(message_sent),0);
                // cout<<"on 105"<<endl;
                length_read=recv(client_sockfd,read_message,500,0);
                if(length_read==0){
                    cout<<"Sender has closed connection"<<endl;
                }
                else if(length_read==-1){
                    cout<<"Error in reading"<<endl;
                }
                // read_message=string("quit").c_str();
                string quit_command(read_message);
                quit_command=quit_command.substr(0,length_read);
                if(quit_command=="quit"){
                    cout<<"Bye "+uname_str+"\n";
                    close(sockfd);
                }
                else{
                    cout<<"Unknown Command\n";
                    close(sockfd);
                }
            }
            else{
                cout<<"Wrong Passwd\n";
                close(sockfd);
            }
        }
        else{
            cout<<"Invalid User\n";
            close(sockfd);
        }
        // printf("User is %s, and Password is %s\n",uname,pwd);

    }
    else{
        cout<<"Unknown Command\n";
        close(sockfd);
    }

    return 0;

}