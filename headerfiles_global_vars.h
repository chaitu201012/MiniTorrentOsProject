
#ifndef  HEADER_FILES
#define  HEADER_FILES

#include <bits/stdc++.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>
#include <map>
#include <vector>
#include <cstring>
#include <openssl/sha.h>

//******************************

#define vecs vector<string>
#define CHUNKSIZE 512
#define BUFFERSIZE 1024
#define DELIMITER "#"
#define DELIMITER2 "$"
#define DELI '#'
#define DELI2 '$'
#define len(a) a.size()





//******************************

using namespace std;




//******************************

char ip_addr1[32];
char ip_addr2[32];
int portTracker1,portTracker2;

//*******************
typedef struct clportthread{
    int clport;
    int accport;
    int group_id;
}threadport;


struct tracker_attrs{
    char ipAddress[32];
    int port;  
};
typedef struct tracker_attrs trackerAttributes;

//*************************************************

typedef struct peerSock{
    int sock;
    int port;

}peerSock;

typedef struct filePort{
    char fileName[200];
    int port;
    string chunkinfo;
    int clPort=0;
    char outfile[200];
}filePort;

//****************************************
struct flag_info{
    int flag=0;
    int port;
    char ip[32];
    int group_id;
};

//****************************************
typedef struct user_info
{
	char user_name[20];
	char pwd[20];
	char ip[20];
	int port;
}userInfo;
//******************************************

struct user_name_pwd{
    char user_name[20];
    char pwd[20];
};
//******************************************

typedef struct client_attrs{
    char ip[32];
    int port;
    char trackFile[50];
}clientInfo;

typedef struct fileswithtracker{
    char ip[32];
    int port;
    char fileName[20];
    int fileSize;
    string sha1;
    int noOfChunks;
    string chunkids;
    int group_id;

}fileWithTracker;

int user_number=0;
int group_number=0;
int group_id;


//************Maps needed to handle all functions and store user and file information*******************

map<string,user_info> map_user_info;
set<string> files;
set<string> login; //user name of the users 
set<int> logged_ports;  // contains user ids that are currently logged in and logs out when ever enterlog out
map<int,int> groups;//<port groupid>
map<int,int> join_requests;
set<int> groupids;
map<int,set<string>> files_group; // group_id, set of files in that group 
map<int,int>  owners; //<group_id,port> of peer whos is the owner orcreator of group 
vector<fileWithTracker> filesAvailable; ///useful to detect which peer(port) has which file.
map<pair<int,string>,set<int>> ports_with_chunks ;//some data structure to store the details which port has which file and which chunk 
//  port  fileName   set of chunks.
//pass a struct filename and port 

//**********************************************************************************************************


string prepareBuffer(char buf1[],char buf2[]){
    string s ="";
    s=string(buf1)+DELIMITER+string(buf2)+DELIMITER;
    return s;
}

short socketCreate(void) {
    short hSocket;
    printf("Create the socket\n");
    hSocket = socket(AF_INET, SOCK_STREAM, 0);
    return hSocket;
}

int socketConnect(int hSocket) {
    int iRetval=-1;
    int ServerPort = 12345;//exact same as that of server if does not matcch then it will not connect 
    struct sockaddr_in remote;
    remote.sin_addr.s_addr = inet_addr("127.0.0.1"); //Local Host
    remote.sin_family = AF_INET;
    remote.sin_port = htons(ServerPort);
    iRetval = connect(hSocket,(struct sockaddr *)&remote,sizeof(struct sockaddr_in));
    return iRetval;
}

int socketSend(int hSocket,char* Rqst,short lenRqst) {
    int shortRetval = -1;
    struct timeval tv;
    tv.tv_sec = 20;  /* 20 Secs Timeout */
    tv.tv_usec = 0;

    if(setsockopt(hSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&tv,sizeof(tv)) < 0) {
        printf("Time Out\n");
        return -1;
    }
    shortRetval = send(hSocket, Rqst, lenRqst, 0);
    return shortRetval;
}


int bindCreatedSocket(int hSocket) {
    int iRetval=-1;
    int clientPort = 12345;

   struct sockaddr_in  remote= {0};

   /* Internet address family */
   remote.sin_family = AF_INET;

   /* Any incoming interface */
   remote.sin_addr.s_addr = htonl(INADDR_ANY);
   remote.sin_port = htons(clientPort); /* Local port */

   iRetval = bind(hSocket,(struct sockaddr *)&remote,sizeof(remote));
   return iRetval;
}

string calculateSHA1(char *filename)
{
    if (!filename) {
        return NULL;
    }

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        return NULL;
    }

    unsigned char* sha1_digest =(unsigned char *) malloc(sizeof(char)*SHA_DIGEST_LENGTH);
    SHA_CTX context;

    if(!SHA1_Init(&context))
        return NULL;

    unsigned char buf[CHUNKSIZE*BUFFERSIZE];
    //unsigned char obuf[25];
    string str="";                                          
    while (!feof(fp)){
        size_t total_read;
        if((total_read=fread(buf, 1, sizeof(buf), fp)>0)){
            if(!SHA1_Update(&context, buf, total_read))
                return NULL;
            if(!SHA1_Final(sha1_digest, &context))
                return NULL;
            char *sha1hash = (char *)malloc(sizeof(char) * 41);
            sha1hash[41] = '\0';
            int i;
            for (i = 0; i < SHA_DIGEST_LENGTH; i++)
            {
                sprintf(&sha1hash[i*2], "%02x", sha1_digest[i]);
            }
            for (int i=0;i<20;i++)
                str+=sha1hash[i];

            memset(sha1hash,0,sizeof(sha1hash));
			memset(sha1_digest,0,sizeof(sha1_digest));
        }
    }
    fclose(fp);

    return str;
}


long findSize(char file_name[]) 
{ 
    // opening the file in read mode 
    FILE* fp = fopen(file_name, "r"); 
  
    // checking if the file exist or not 
    if (fp == NULL) { 
        printf("File Not Found!\n"); 
        return -1; 
    } 
  
    fseek(fp, 0L, SEEK_END); 
  
    // calculating the size of the file 
    long int res = ftell(fp); 
  
    // closing the file 
    fclose(fp); 
  
    return res; 
} 


vecs tokeniseCommand2(string s){
	vecs tokens;
	string str="";
	for(int i=0;i<len(s)-1;i++)
	{
		if(s[i]!=DELI2)
		{
			str+=s[i]; 
		}
		else
		{
			tokens.push_back(str);
			str="";
		}

	}
	tokens.push_back(str);
	str="";

return tokens;	

}

vecs tokeniseCommand(string s){
	vecs tokens;
	string str="";
	for(int i=0;i<len(s)-1;i++)
	{
		if(s[i]!=DELI)
		{
			str+=s[i]; 
		}
		else
		{
			tokens.push_back(str);
			str="";
		}

	}
	tokens.push_back(str);
	str="";

return tokens;	

}

void* quitting(void* pargs);
void* listener(void* pargs);
void * handle_request(void* pargs);
void listenerServerCreate(char ip[],int portNoTracker);
void connectionEstablish(int serverSockDesc,struct sockaddr_in servAddress);
void* handleServices(void * sock);
void* client_func(void *pargs);
void * serverListener(void *);
string prepareBuffer(char buf1[],char buf2[]);


#endif //HEADER_FILES