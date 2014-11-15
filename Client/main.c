#include "common_headers.h"
#include "socket_utilities.h"
#include "json_utilities.h"
#include "webserver.h"
#include "client.h"

pthread_rwlock_t lock;
extern int sock_fd;

main(){
	//init reader-writer lock
	pthread_rwlock_init(&lock, NULL);
  	
	//invoke the webserver
	ws_init();	
	//invoke the socket client
	client_init();	//this is blocking
	while(1){
		char len_str1[JSON_LEN_SIZE+1];
		//getting data length
		Read(sock_fd,len_str1,JSON_LEN_SIZE);
		len_str1[JSON_LEN_SIZE] = 0; // Making it a null terminated string

		//extract length from string
		json_t * tmp = JSON_get_value_from_pair(len_str1, "LENGTH");
		int len = atoi(json_string_value(tmp));
		json_decref(tmp);		//have the length so can decrease reference count
		char strrecv[len+1];
		//reading `len` bytes of data 
		Read(sock_fd,strrecv,len);
		strrecv[len] = 0;	// This is for making the char array as a string
		//need to send data to parse
		printf("some stuff : %s",strrecv);
		client_parse(strrecv);
	}

	pthread_rwlock_destroy(&lock);
	//closing the webserver
	ws_finalize();
}
