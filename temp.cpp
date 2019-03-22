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

                            length_read=recv(client_sockfd,read_message,500,0);
                            if(length_read==0){
                                cout<<"Sender has closed connection"<<endl;
                            }
                            else if(length_read==-1){
                                cout<<"Error in reading"<<endl;
                            }
                            string quit_command(read_message);
                            quit_command=quit_command.substr(0,length_read);
                            if(quit_command=="quit"){
                                cout<<"Bye "+uname_str+"\n";
                                close(client_sockfd);
                            }
                            else{
                                cout<<"Unknown Command\n";
                                close(client_sockfd);
                            }
                        }
                        else{
                            cout<<uname_str+": Folder Read Fail\n";
                            close(client_sockfd);
                        }
                        
                    }
                    else{
                        cout<<"Unknown Command\n";
                        close(client_sockfd);
                    }