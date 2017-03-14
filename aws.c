/*aws.c-Program boots up the aws server on the static TCP port 25894 and accepts the client connecting to it.It then receives the data and extracts the reduction function and count. The data is divided equally among backend servers using UDP connection and a static UDP port 24894*/

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

/*MAX_BUFFER is the maximum number of bytes the server can receive the data in the recv() call*/
/*MAX_DATA is the maximum number of bytes of data it can send to backend server in the send() call*/
#define MAX_BUFFER 4100
#define MAX_DATA 1500

/*Function bkserver takes the one third of entire data at once and sends to one of the backend server each time it is called*/
int bkserver(long int array[],int port,char bk_name)
{
	struct sockaddr_in bkA,aws_udp;
	int lenA,sockA,m,x,p,r,n;
	const char *red_func1=NULL;
	int size=(array[1]);
	int32_t res1;

/*A UDP socket is created using socket() call which returns a socket descriptor that would be used to exchange the data between the servers*/
	if((sockA=socket(AF_INET,SOCK_DGRAM,0))==-1)
		{
			perror("Error in socket");
			exit(-1);
		}

	bkA.sin_family = AF_INET;
	bkA.sin_port= htons(port);  
	bkA.sin_addr.s_addr = inet_addr("127.0.0.1");   
	bzero(&bkA.sin_zero,0);        
	lenA=sizeof(struct sockaddr_in);

/*sendto() call sends the data in the array to the backend server along with the reduction type and count of the data in the array which are included in the integer array*/
	if((n=sendto(sockA,array,MAX_DATA,0,(struct sockaddr *)&bkA,lenA))<0)
		{
			perror("Error in sending to UDP1\n");
			exit(-1);
		}
	printf("The AWS has sent %d numbers to Backend-Server %c \n",size,bk_name);

	aws_udp.sin_port=htons(24894);

/*The aws server receives the result computed by the backend servers using the recvfrom() call*/
	if((p=recvfrom(sockA,&res1,sizeof(res1),0,(struct sockaddr *)&bkA,&lenA))<0)
	{
		perror("Error in sending to UDP1\n");
		exit(-1);
	}
/*Switch case condition check to extract the information of the reduction type requested in the program*/
	switch (array[0])
	{
		case 1:
			red_func1="MIN";
			break;
		case 2:
			red_func1="MAX";
			break;
		case 3:
			red_func1="SUM";
			break;
		case 4:
			red_func1="SOS";
			break;
		default:
		printf("Invalid reduction function\n");
	}
	printf("The AWS received reduction result of %s from Backend-Server %c using UDP over port %d and it is %d\n",red_func1,bk_name,ntohs(aws_udp.sin_port),res1);
	return res1;
	}

/*Main function performs the data exchange between server and client using TCP connection and computes the final result*/
int main()
{
	int sockfds,sockfdc,len;
	struct sockaddr_in server,client;
	long int data[1050],data1[400],data2[400],data3[400];
	int i,j,k,red_type; const char *red_func= NULL;
	int data_len,u,count,bk_count;
	int32_t output[3],result,min_no,max_no,sum_no,sos_no;

/*A TCP socket is created using socket() call which returns a socket descriptor that would be used to exchange the data between the server and the client*/
	if((sockfds=socket(AF_INET, SOCK_STREAM,0))==-1)
	{
		perror("Socket Error:\n");
		exit(-1);	
	}

/*setsockopt() function allows reusing the socket that is not closed and bound to a particular IP address and port number*/
	if (setsockopt(sockfds, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
    		{
			perror("setsockopt(SO_REUSEADDR) failed");
			exit(-1);
		}
	server.sin_family = AF_INET; 
	server.sin_port= htons(25894);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");   
	bzero(&server.sin_zero,0);            
	len=sizeof(struct sockaddr_in);

/*The socket descriptor sockfds of the aws server is mapped to its IP and static port number in the bind()*/
	if(bind(sockfds,(struct sockaddr *)&server,len)==-1)
		{
			perror("Error in Bind:\n");
			exit(-1);
		}	

/*Listen() call signifies the server can buffer a maximum of 5 client requests in the queue*/
	if((listen(sockfds,5))==-1)	
		{
			perror("Error in Listen\n");
			exit(-1);
		}
	printf("the AWS server is up and running \n");

/*while(1) loop is used to keep the server on and wait for the client requests all the time*/
	while(1)
		{
			
			if((sockfdc=accept(sockfds,(struct sockaddr*)&client,&len))==-1)   
			{
				perror("Accept");
				exit(-1);
			}
			
/*recv() receives the data from the client which includes the reduction information mapped as a integer and the total count of the file elements*/
	bzero(data,MAX_BUFFER);
	if((data_len=recv(sockfdc,data,MAX_BUFFER,0)) < 0)
	{
		perror("Server recv in error\n");
		exit(-1);
	}
/*count gives the number of file elements and bk_count is the number of elemets sent to backend servers*/		
	count=data[1];
	bk_count=count/3;
	red_type=data[0];
	printf("The AWS has received %d numbers from the client using TCP over port %d\n",count,ntohs(server.sin_port));

/*Loop to divide the entire integer array into 3 equal parts and send to the function bkserver in order to be sent to backend servers*/
	i=2;
	for(j=2;j<bk_count+2;j++)
		{
			data1[i]=data[j];
			i++;
		}
		data1[0]=red_type;	
		data1[1]=bk_count;		
		output[0]=bkserver(data1,21894,'A');		

	i=2;
	for(j=bk_count+2;j<(2*bk_count)+2;j++)
		{
			data2[i]=data[j];
			i++;
		}
		data2[0]=red_type;
		data2[1]=bk_count;
		output[1]=bkserver(data2,22894,'B');
		
	i=2;
	for(j=(2*bk_count)+2;j<(3*bk_count)+2;j++)
		{
			data3[i]=data[j];
			i++;
		}
		data3[0]=red_type;
		data3[1]=bk_count;
		output[2]=bkserver(data3,23894,'C');
		
/*Extracting the reduction function from the mapped integer array and perform the computation on the results obtained from the backend servers to compute the final result*/
	if(red_type==1)
	{
		red_func="MIN";
		min_no=output[0];
    			for (k=0;k<3;k++)
        			{
        			     if(output[k]<min_no)  {
        	   			    min_no = output[k];
        	    			}		
      				}
      		result=min_no;
	}
	else if(red_type==2)
	{
		red_func="MAX";  
		max_no=output[0];
		for (k=1;k<3;k++)
        		{
	             		if(output[k]>max_no)  
					{
	                			max_no = output[k];
		       		     	}
      			}
	     	result=max_no;
	}
	else if(red_type==3)
		{
			red_func="SUM";
			sum_no=0;
    			for (k=0;k<3;k++)
      			{
        			sum_no=sum_no+output[k];
       			}
   			result=sum_no;
		}
	else if(red_type==4)
		{    
			red_func="SOS";
			sos_no=0;
			    for (k=0;k<3;k++)
			      {
					sos_no=sos_no+(output[k]);
			       }
			   result=sos_no;
		}
	else
		{
			printf("Invalid Reduction type\n");
		}

	printf("The AWS has successfully finished the reduction %s: %d\n",red_func,result);

/*This send() call sends the final result to the client on the same TCP connection established*/
	u=send(sockfdc,&result,sizeof(result),0);
	if(u<0)
		{
			perror ("Error in sending result\n");
			exit(-1);
		}
	printf("The AWS has successfully finished sending the reduction value to client\n\n");

	close(sockfdc);
	}
	close(sockfds);
}




