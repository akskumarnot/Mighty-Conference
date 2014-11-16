#define PORT 8000
#ifndef CLIENT_H
#define CLIENT_H
#include<string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include"webserver.h"
#include"json_utilities.h"
#define FIFO_PATH "./../probe.fifo"


extern pthread_rwlock_t lock;

int sock_fd;
struct sockaddr_in server_sock;
extern PyObject * pModule; //ref to the python module object

void client_init(){
	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	
	// Initialize the socket structure to zero
	memset(&server_sock,sizeof(struct sockaddr_in),'\0');

	server_sock.sin_port = htons(PORT);
	server_sock.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1",&server_sock.sin_addr);

	//initiate connection 
	connect(sock_fd,(struct sockaddr*)&server_sock,sizeof(struct sockaddr_in));
	
	//connection made
}


static void* heart_beat_message(void* arg)
{
	int * tmp = (int *)arg;
	int sock_fd = *tmp;
	
	json_t * json_obj=NULL;
	json_t * json_obj2=NULL;	
	while(1)
	{
		printf("Sent BEEP\n");
		
		json_obj = JSON_make_str("HEARTBEAT","BEEP");
		char* str = json_dumps(json_obj, JSON_DECODE_ANY);
		
		//length string json object
		json_obj2 = JSON_make_length_str(str); 
		char* len_str =  json_dumps(json_obj2, JSON_DECODE_ANY);
		
		//lock
		pthread_rwlock_wrlock(&lock);

		Write(sock_fd,len_str,JSON_LEN_SIZE);
		Write(sock_fd,str,strlen(str));

		//unlock
		pthread_rwlock_unlock(&lock);

		sleep(4);
	}

	json_decref(json_obj);
	json_decref(json_obj2);
}

void heartbeat_init(){
	pthread_t tid;
	if( pthread_create(&tid, NULL, heart_beat_message, (void*)&sock_fd) != 0)
	{
		printf("Failed to spawn a thread for the list monitor");
	}
}

void init_fifo_server(){
	mkfifo(FIFO_PATH,S_IRUSR| S_IWUSR);
}

void finalize_fifo_server(){
	unlink(FIFO_PATH);
}

/*This is a temp function , need to understand later why the jannsan json won't work*/
int remakeJSON(char * str){
	json_t* tmp;
	char * key=NULL;
	const char * value;
	
	//trying out keys , for auth first
	key = "AUTH";
	tmp  = JSON_get_value_from_pair(str,key);
	if(tmp==NULL){
		key = NULL;	
	}

	//for client list 
	if(key==NULL){
	key="CLIENTS_LIST";
	tmp = JSON_get_value_from_pair(str,key);
	}
	
	if(tmp==NULL){
		key = NULL;
	}
	
	//add all checks before the following
	if(key==NULL)
		return 0;

	json_incref(tmp);
	//means auth data , need to get the dump
	value = json_string_value(tmp);	
	sprintf(str,"{\"%s\":\"%s\"}",key,value);
	json_decref(tmp);
	return 1;		
}

void relay(char * str){	
	//before relaying need to put the data in the req json format
	remakeJSON(str);
	str = strcat(str,"\n");
	int fd = open(FIFO_PATH, O_WRONLY);
	write(fd,str,strlen(str));
	close(fd);	
}

#endif
