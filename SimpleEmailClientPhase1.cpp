#include<bits/stdc++.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#define BACKLOG 2
using namespace std;
int main(int argc, char *argv[]){
    if(argc != 4){
        fprintf(stderr,"Error: 3 arguments after the executable name are required (serverIPAddr:port, user-name, passwd) but given %d\n",argc-1);
        exit(1);
    }
    sockaddr_in dest_addr;
    regex ip_port("(.*):([^:]*)");
    smatch match;
    string ip_port_str(argv[1]);
    int sockfd;
    sockfd=socket(PF_INET,SOCK_STREAM,0);
    if(regex_search(ip_port_str,match,ip_port)==true){
        dest_addr.sin_family=AF_INET;
        stringstream port(match.str(2));
        int dest_port_no;
        port>>dest_port_no;
        dest_addr.sin_port=htons(dest_port_no);
        dest_addr.sin_addr.s_addr=inet_addr(match.str(2).c_str());
        memset(&(dest_addr.sin_zero),'\0',8);
        int connect_success=connect(sockfd,(struct sockaddr *)&dest_addr,sizeof(struct sockaddr));
        if(connect_success==-1){
            cout<<"Error: Connection Failed"<<endl;
            exit(2);
        }
        else{
            printf("ConnectionDone: %s\n",argv[1]);
        }
        string uname(argv[2]);
        string passwd(argv[3]);
        string message_to_send="User: "+uname+" Pass: "+passwd+"\0";
        send(sockfd,message_to_send.c_str(),message_to_send.size(),0);
        char* read_message;
    //??What should be length of buffer read.
        int length_read=recv(sockfd,read_message,500,0);
        printf("%s",read_message);
        message_to_send="quit\0";
        send(sockfd,message_to_send.c_str(),message_to_send.size(),0);
        exit(0);


    }
    return 0;
}