#include<bits/stdc++.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<dirent.h>
#define BACKLOG 10
using namespace std;
int main(int argc, char *argv[]){
    if(argc != 4){
        fprintf(stderr,"Error: 3 arguments after the executable name are required (portNum,passwdfile,user-database) but given %d\n",argc-1);
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
    DIR *dbdir;
    dbdir=opendir(argv[3]);
    if(dbdir==NULL){
        fprintf(stderr,"Error: Directory does not exist or is inaccessible\n");
        exit(4);
    }
    int listen_success=listen(sockfd, BACKLOG);
    
    if(listen_success != -1){
        printf("ListenDone: %d\n",myport);
    }
    sockaddr_in their_addr;
    // sockaddr_in* cpointer=&client_addr;
    socklen_t sin_size=sizeof(struct sockaddr_in);
    int client_sockfd;
    string user,passwd;
    map<string,string> user_password_map;
    while(passfile>>user>>passwd){
        user_password_map[user]=passwd;
    }
    while(true){
        client_sockfd=accept(sockfd,(struct sockaddr*) &their_addr,&sin_size);
        cout<<"waiting"<<endl;
        if(client_sockfd!=-1){
            cout<<"Client: "<<inet_ntoa(their_addr.sin_addr)<<":"<<ntohs(their_addr.sin_port)<<endl;

        }
        
        char read_message[100000];
        int length_read=recv(client_sockfd,read_message,100000,0);
        if(length_read==0){
            cout<<"Sender has closed connection"<<endl;
        }
        else if(length_read==-1){
            cout<<"Error in reading"<<endl;
        }
        regex user_pass_format("User: (.*) Pass: (.*)\0");
        smatch match;
        string read_message_str(read_message);
        read_message_str=read_message_str.substr(0,length_read);

        if (regex_search(read_message_str,match,user_pass_format)==true){

            const char* uname=match.str(1).c_str();
            const char* pwd=match.str(2).c_str();
            string uname_str=match.str(1);
            string pwd_str=match.str(2);

            if(user_password_map.find(uname_str)!=user_password_map.end()){
                if(user_password_map[uname_str]==pwd_str){
                    printf("Welcome %s\n",uname);
                    string message="Welcome "+uname_str+"\n"+"\0";
                    const char* message_sent=message.c_str();
                    send(client_sockfd,message_sent,strlen(message_sent),0);
                    
                    while(true){
                        regex retrv_format("RETRV ([0123456789]+)");
                        smatch retrv_match;

                        length_read=recv(client_sockfd,read_message,500,0);
                        if(length_read==0){
                            cout<<"Sender has closed connection"<<endl;
                        }
                        else if(length_read==-1){
                            cout<<"Error in reading"<<endl;
                        }

                        string list_command(read_message);
                        list_command=list_command.substr(0,length_read);

                        if(list_command=="LIST"){

                            string user_database(argv[3]);
                            user_database=user_database+"/"+uname_str+"/";
                            dbdir=opendir(user_database.c_str());
                            int no_files=0;
                            struct dirent *ep;

                            if(dbdir!=NULL){
                                while(ep=readdir(dbdir)) no_files++;
                                no_files=no_files-2;
                                closedir(dbdir);
                                string no_files_str;
                                ostringstream temp1;
                                temp1<<no_files;
                                no_files_str=temp1.str();
                                message=uname_str+": No of messages "+no_files_str+"\n";
                                cout<<message;
                                message=message+"\0";
                                send(client_sockfd,message.c_str(),strlen(message.c_str()),0);
                                
                            }
                            else{

                                cout<<uname_str+": Folder Read Fail\n";
                                close(client_sockfd);
                            }
                            
                        }
                        else if(list_command=="quit"){
                            
                                    cout<<"Bye "+uname_str+"\n";
                                    close(client_sockfd);
                                    break;
                                }
                        else if(regex_search(list_command,retrv_match,retrv_format)==true){
                            string file_id(retrv_match.str(1));
                            DIR* user_folder;
                            string parent_dir_str(argv[3]);
                            string user_folder_str=parent_dir_str+"/"+uname_str;
                            user_folder=opendir(user_folder_str.c_str());
                            struct dirent* inner_file;
                            if(user_folder==NULL){
                                cout<<"Message Read Fail\n";
                                close(client_sockfd);
                                break;

                            }
                            else{
                                bool file_found=0;
                                string file_to_transfer_str;
                                string file_name_format=file_id+"\."+".*";
                                regex file_name_format_reg(file_name_format);
                                try{
                                while(inner_file=readdir(user_folder)){
                                    string temp_file_name(inner_file->d_name);
                                    smatch tempmatch;
                                    if(regex_search(temp_file_name,tempmatch,file_name_format_reg)==true){
                                        file_found=1;
                                        file_to_transfer_str=temp_file_name;
                                    }
                                }}
                                catch(...){
                                    cout<<"Message Read Fail\n";
                                    close(client_sockfd);
                                    break;
                                }
                                if(!file_found){
                                    cout<<"Message Read Fail\n";
                                    close(client_sockfd);
                                    break;
                                }
                                else{
                                    cout<<uname_str+": Transferring Message "+file_id+"\n";
                                    // char file_buffer[100];
                                    FILE* temp_file;
                                    string file_path(user_folder_str+"/"+file_to_transfer_str);
                                    temp_file=fopen(file_path.c_str(),"rb");
                                    // fscanf(temp_file,"%s",file_buffer);
                                    send(client_sockfd,file_to_transfer_str.c_str(),strlen(file_to_transfer_str.c_str()),0);
                                    while(1){
                                        char buff[100000];
                                        int nread=fread(buff,1,1024,temp_file);
                                        if(nread>0)
                                        {   cout<<nread<<" "<<strlen(buff)<<endl;
                                           int amt_send=send(client_sockfd,buff,nread,0);
                                           cout<<"Line 198 amt_send "<<amt_send<<" "<<strlen(buff)<<endl;
                                        }
                                        if(nread<1024){
                                            break;
                                        }
                                        cout<<"reading"<<endl;
                                    }


                                }

                            }

                        }
                        
                        else{
                            cout<<"Unknown Command\n";
                            close(client_sockfd);
                            break;
                        }


                    
                }}
                else{
                    cout<<"Wrong Passwd\n";
                    close(client_sockfd);
                }
            }
            else{
                cout<<uname_str+" is invalid"<<endl;
                cout<<"Invalid User\n";
                close(client_sockfd);
            }
            // printf("User is %s, and Password is %s\n",uname,pwd);

        }
        else{
            cout<<"Unknown Command\n";
            close(client_sockfd);
        }
    }
    close(sockfd);

    return 0;

}