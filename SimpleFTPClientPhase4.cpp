#include<bits/stdc++.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<dirent.h>
#define BACKLOG 2
using namespace std;
int main(int argc, char *argv[]){
    if(argc != 6){
        fprintf(stderr,"Error: 5 arguments after the executable name are required (serverIPAddr:port, user-name, passwd, list-of-messages, local-folder) but given %d\n",argc-1);
        exit(1);
    }
    sockaddr_in dest_addr;
    regex ip_port("(.*):([^:]*)");
    smatch match;
    string ip_port_str(argv[1]);
    int sockfd;
    sockfd=socket(PF_INET,SOCK_STREAM,0);
    // printf("%s,%s,%s\n",argv[1],argv[2],argv[3]);
    // cout<<"here"<<endl;
    if(regex_search(ip_port_str,match,ip_port)==true){
        dest_addr.sin_family=AF_INET;
        stringstream port(match.str(2));
        int dest_port_no;
        port>>dest_port_no;
        // cout<<dest_port_no<<" "<<match.str(2)<<endl;
        dest_addr.sin_port=htons(dest_port_no);
        dest_addr.sin_addr.s_addr=inet_addr(match.str(1).c_str());
        memset(&(dest_addr.sin_zero),'\0',8);
        int connect_success=connect(sockfd,(struct sockaddr *)&dest_addr,sizeof(struct sockaddr));
        if(connect_success==-1){
            cout<<"Error: Connection Failed"<<endl;
            exit(2);
        }
        else{
            printf("ConnectionDone: %s\n",argv[1]);
        }
        string num_string(argv[4]);
        string local_folder_name(argv[5]);
        stringstream ss(num_string);
        vector<int> file_no_vec;
        int file_no;
        try
        {
         while(ss>>file_no){
             file_no_vec.push_back(file_no);
             if(ss.peek()==',') ss.ignore();

         }   
        }
        catch(int x1)
        {
            cerr<<"Error: 5 arguments after the executable name are required (serverIPAddr:port, user-name, passwd, list-of-messages(--> must be comma separated integers), local-folder)\n";
            exit(3);
        }
        string fname(argv[5]);
        string delete_command="rm -r "+fname;

        DIR* save_dir;
        save_dir=opendir(fname.c_str());
        if(save_dir==NULL){
            int directory_not_created=mkdir(fname.c_str(),0777);
            if(directory_not_created==-1){
                cerr<<"Problem with directory management\n";
                exit(4);
            }
        }
        else{
            system(delete_command.c_str());
            int directory_not_created=mkdir(fname.c_str(),0777);
            if(directory_not_created==-1){
                cerr<<"Problem with directory management\n";
                exit(4);
            }

        }
        string uname(argv[2]);
        string passwd(argv[3]);
        string message_to_send="User: "+uname+" Pass: "+passwd+"\0";
        send(sockfd,message_to_send.c_str(),strlen(message_to_send.c_str()),0);
        char read_message[100000];
    //??What should be length of buffer read.
        int length_read=recv(sockfd,read_message,100000,0);
        printf("%s",read_message);

        sleep(10);
            
        for(int i=0;i<file_no_vec.size();i++){
            int file_no_to_download=file_no_vec[i];
            ostringstream temp1;
            temp1<<file_no_to_download;
            string file_no_str(temp1.str());
            message_to_send="RETRV "+file_no_str+"\0";
            send(sockfd,message_to_send.c_str(),strlen(message_to_send.c_str()),0);
            char file_name[100000];
            read(sockfd,file_name,100000);
            string downloaded_file_name(file_name);
            string save_address=fname+"/"+downloaded_file_name;
            FILE* dwfl;
            // cout<<save_address<<endl;
            dwfl=fopen(save_address.c_str(),"ab");
            char file_temp_buff[1024];
            int brecv=0;
            long double cize=1;
            brecv=recv(sockfd,file_temp_buff,1024,0);
            if(brecv!=0){
            while(true){
                cize++;
                fflush(stdout);
                fwrite(file_temp_buff,1,brecv,dwfl);
                if(brecv<1024) break;
                brecv=recv(sockfd,file_temp_buff,1024,0);
                // if(brecv<=0) break;
            }
            cout<<"Downloaded Message "+file_no_str+"\n";}
            fclose(dwfl);
        }

        message_to_send="quit\0";
        send(sockfd,message_to_send.c_str(),strlen(message_to_send.c_str()),0);
        exit(0);


    }
    return 0;
}