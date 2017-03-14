/*serverC.c-Program to receive last one third of the data and perform the required computation*/
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

# define MAX_DATA 1500
int main()
	{
		int sockC,len,len_cli;
		struct sockaddr_in serverC,aws_client;
		long int buf[400],min_no,max_no,sum_no,sos_no,output1;
		int n,m,k,red_type3,size; 
		const char *reduction=NULL;

/*A UDP socket is created using socket() call which returns a socket descriptor that would be used to exchange the data between the servers*/
	if((sockC=socket(AF_INET,SOCK_DGRAM,0))==-1)
		{
			perror("Error in creating socket\n");
			exit(-1);
		}

/*setsockopt() function allows reusing the socket that is not closed and bound to a particular IP address and port number*/
	if (setsockopt(sockC, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
		{
			perror("setsockopt failed");
			exit(-1);
		}

	serverC.sin_family = AF_INET;
	serverC.sin_port= htons(23894);  
	serverC.sin_addr.s_addr = inet_addr("127.0.0.1");   
	bzero(&serverC.sin_zero,0);    
//memset(&serverA.sin_zero,0,sizeof(serverA.sin_zero));
	len=sizeof(serverC);

	if((bind(sockC,(struct sockaddr*)&serverC,len))==-1)
		{
			perror("Error in Bind:\n");
			exit(-1);
		}

	len_cli=sizeof(struct sockaddr_in);
	bzero(buf,MAX_DATA);

/*while(1) loop is used to keep the server on and wait for the client requests all the time*/
	while(1)
		{

			printf("The server C is up and running using UDP on port %d\n",ntohs(serverC.sin_port));
			n=recvfrom(sockC,buf,MAX_DATA,0,(struct sockaddr*)&aws_client,&len_cli);
			if(n<0)
			{
				perror("Error in receive\n");
				exit(-1);
			}
		
			size=buf[1];
			red_type3=buf[0];
	printf("The server C has received %d numbers\n",size);

/*Following check condition performs the computation of the reduction type max,min,sum,sos*/
	if(red_type3==1)
		{
			reduction = "MIN";
			min_no=buf[2];
			    for (k=2;k<size+2;k++)
			        {
			             if(buf[k]<min_no)  {
			               min_no = buf[k];
			             }
		     		 }
	      		output1=min_no;
		}
	else if(red_type3==2)
		{  
			reduction="MAX";
			max_no=buf[2];
				for (k=2;k<size+2;k++)
	        			{
			        	     if(buf[k]>max_no)  {
			        	       max_no = buf[k];
	        			     }
	      				}
	      		output1=max_no;
		}
	else if(red_type3 == 3)
		{
			reduction ="SUM";
			sum_no=0;
	   		 for (k=2;k<size+2;k++)
	      			{
	        			sum_no=sum_no+buf[k];
	       			}
	   		output1=sum_no;
		}
	else if(red_type3 == 4)
		{    
			reduction ="SOS";
			sos_no=0;
		    	for (k=2;k<size+2;k++)
	      			{
	        			sos_no=sos_no+ (buf[k]*buf[k]);
	       			}
	   		output1=sos_no;
		}
	else
		printf("Invalid Reduction type");
	
	printf("The Server C has successfully finished the reduction %s : %ld \n",reduction,output1);

/*The result computed is sent to aws server using sendto()*/		
	if((m=sendto(sockC,&output1,sizeof(output1),0,(struct sockaddr *)&aws_client,len_cli))<0)
		{
			perror("Error in send\n");
			exit(-1);
		}
	printf("The Server C has successfully finished sending the reduction value to AWS server\n\n");
	}

}	
