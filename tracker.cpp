#include "headerfiles_global_vars.h"
#include "helper1_tracker.h"
#include "helper2_tracker.h"
using namespace std;



int main(int argc,char** argv)
{
	if(argc<3)
	{
		std::cout<<"please enter File \n";
		exit(1);
	}
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    

	fp = fopen(argv[1],"r");
	if(fp==NULL)
	{
		std::cout<<"File doesn't exist\n";
		return 1;
	}
    /*
    int count=0;
    while ((read = getline(&line, &len, fp)) != -1) {
        if(count==0) strcpy(ip_addr1,line);
        else if(count==1) portNo=atoi(line);
        count++;

    }
    */
   // reasding first tracker inpormation 
    read = getline(&line, &len, fp);
	strcpy(ip_addr1,line);
	read = getline(&line, &len, fp);
	portTracker1=atoi(line);
    //cout<<"ip_addr1 "<<ip_addr1<<endl;
    // reading second tracker information 
	read = getline(&line, &len, fp);
	strcpy(ip_addr2,line);
	read = getline(&line, &len, fp);
	portTracker2=atoi(line);
	


    ip_addr1[strlen(ip_addr1)-1]='\0';
    ip_addr2[strlen(ip_addr2)-1]='\0';

	cout<<"Tracker 1 : "<<ip_addr1<<" "<<portTracker1<<endl;
	//cout<<"Tracker 2 : "<<ip_addr2<<" "<<portTracker2<<endl;

    int no_trackers=atoi(argv[2]);
    tracker_attrs track;

    switch(no_trackers){
        case 1:
            strcpy(track.ipAddress,ip_addr1);
            track.port=portTracker1;
            break;
        case 2:
        strcpy(track.ipAddress,ip_addr1);
            track.port=portTracker1;
            break;
        default:
            cout<<"Please enter one or two in tracker number filed not more than that\n";
            break;

    }

    int NTHREADS=2;

    pthread_t thread_id1,thread_id2;

    
    int id2=pthread_create(&thread_id2, NULL, listener,&track);
    //std::cout<<id2<<endl;

    //*****************************Server thread creation for listening requests from peers*********************************//
    if(id2){
        std::cout<<"Server Listener thread creation failed :: Error:: "<<id2<<endl;
        exit(EXIT_FAILURE);
    }

    //*****************************Thread to implement the quit function *********************************//

    int id1=pthread_create(&thread_id1, NULL, quitting,NULL);
    if(id1){
        std::cout<<"Client thread creation failed  :: Error:: "<<id1<<endl;
        exit(EXIT_FAILURE);
    }


    pthread_join(thread_id2,NULL);
    return 0;
}
