/* client.c - Program takes the reduction type as the command line argument and reads the data elements from the csv file.It then puts the count of the file elements, data and the reduction type in a single array and then is sent to the aws server using send() call*/
/*The follwing material of Vivek Ramachandran is refered https://www.youtube.com/watch?v=pTYNQwWqB2Y&list=PLGvfHSgImk4bJ7TfqJjrYHy8m3YKbGj5D&index=6 and used in the code parts of send() and recv()*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

//MAX_BUFFER holds the maximum number of bytes of data the program can send to aws server
#define MAX_BUFFER 4100

int main(int argc,char **argv)
{
	int sock,len,r,s,getsock_check;
	struct sockaddr_in aws_server,local_client;
	struct hostent *hp;
	FILE *fp;
	char line[4100];
	int i=1,cnt=0; long int a[1050];
	int32_t res; char red_type[10]; 
 
//Only the reduction type is given as the command line argument
	if(argc>2)
		{
			printf("Too many arguments\n");
			exit(-1);
		}	

/*Getting a socket for the connection, The socket() call returns a socket descrptor that can be used to reference this socket further*/
	if((sock=socket(AF_INET, SOCK_STREAM,0))==-1)
		{
			perror("Socket descriptor Error:\n");
			exit(-1);	
		}

/*gethostbyname is used to get the IP address of the server it connects to.Since all the servers are on the same machine, localhost gives the loopback address which is 127.0.0.1*/
	if((hp = gethostbyname("localhost"))==NULL)
		{
			perror("error in gethostbyname \n");
			exit(-1);
		}
	memcpy(&aws_server.sin_addr,hp->h_addr_list[0],hp->h_length);
	aws_server.sin_family = AF_INET;
	aws_server.sin_port= htons(25894);   
	//aws_server.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	bzero(&aws_server.sin_zero,0);             
	len=sizeof(struct sockaddr_in);

/*connect() call issued is used to connect the client to the listening port of the corresponding aws_server*/
	if(connect(sock,(struct sockaddr *)&aws_server,len)==-1)
		{
			perror("Error in Connect:\n");
			exit(-1);
		}

	printf("The Client is up and running \n");

/*getsockname() retrieves the locally bound port number of the client port which is assigned dynamically*/	
	getsock_check=getsockname(sock,(struct sockaddr *)&local_client,(socklen_t *)&len);
	if(getsock_check==-1)
		{
			perror("getsockname_func in error\n");
			exit(-1);
		}

/*Condition to map the reduction type of string to a integer and stored in the first element of the array. It is assumed that min=1,max=2,sum=3 and sos=4 in the context of the program*/
	if(strcmp(argv[1],"min")==0) 
		{
			a[0]=1;
			strcpy(red_type,"MIN");
		}
	else if(strcmp(argv[1],"max")==0)	{
			a[0]=2;
			strcpy(red_type,"MAX");
		}
	else if (strcmp(argv[1],"sum")==0)	{
			a[0]=3;
			strcpy(red_type,"SUM");
		}
	else if (strcmp(argv[1],"sos")==0)	{
			a[0]=4;
			strcpy(red_type,"SOS");
		}
	else
		{
			printf("Incorrect Reduction type entered\n");
			exit(-1);
		}

/*Following code will open the file in read format and reads each line as character and converts to integer.Then the numbers are stored in the integer array a[] with a[0] being reduction type and a[1] being the count of the file elements*/
	fp=fopen("nums.csv","r");
	if(fp== NULL)
		{
			printf("cant open file\n");
			exit(-1);
		}
	i=2;
	while(fgets(line, 1050, fp)!=NULL) 
		{       /* keep looping until NULL pointer */    
		        a[i]=atoi(line);  
		       // printf("%ld",a[i]);
			i++;       
			cnt++;
		}
	a[1]=cnt;
	fclose(fp); 

/*The array a[] is sent to the aws server using TCP connection and send() call.*/
	if((s=send(sock,a,MAX_BUFFER,0))<0)
		{
			perror("Client send error\n");
			exit(-1);
		}
	printf("The client has sent the reduction type %s to AWS\n",red_type);
	printf("The client has sent %d numbers to AWS\n",cnt);

/*recv() call to receive the final computed result from the aws server on the same TCP connection*/
	if((r=recv(sock,&res,sizeof(res),0))<0)
		{
			perror("Error in receiving result\n");
			exit(-1);
		}
	printf("The client has received the reduction type %s : %d\n",red_type,res);

	close(sock);

}




