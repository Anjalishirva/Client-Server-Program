a. Name: Anjali Shirva

b. USC ID: 5334984894

c. What I have done in the assignment:

  1. In this project,I have implemented a computational offloading model which computes the function MAX,MIN,SUM,SUMofSQUARES(SOS) on a given set of integers.
  2. There are 4 Servers- ServerA,ServerB,ServerC and AWS Server and a Client which offloads computation to the AWS Server which in turn distributes the load to 3           backend servers(Server A,B and C).In this project,the servers are listening on a particular static port for the clients to connect.
  3. The Client  reads the data from a csv file given and takes the reduction type as a command line argument.Then it sends the file elements,number of elements in the      file and the reduction function to be performed to the AWS server in a single integer array using TCP connection. The TCP port chosen by the client for the TCP         connection is dynamic.
  4. AWS server receives the data from the client using TCP connection on a static TCP port assigned and then extracts the reduction function,total count and integer        data.
  5. The data is then divided into 3 equal parts with the reduction type and the count embedded to each part and is distributed among Server A,B and C using UDP           connection on a static UDP port.
  6. The backend servers A,B and C on receiving the data extracts the information of the reduction type and performs the computation on the received data.The results        of each backend server is sent to the AWS server using the same UDP connection.The UDP ports used for the UDP connection is static for Server A,B and C.
  7. On receiving the partial result from backend servers, AWS server performs the reduction function to compute the final result and is sent to the client on the same      TCP connection established.
  8. The client then outputs the final result.

d.CODE Files:

  1. serverA.c - It receives the first one-third of the data elements of the file using UDP connection and computes the Reduction function requested on that data.
  2. serverB.c - It receives the second one-third of the data elements of the file using UDP connection and computes the Reduction function requested on that data. 
  3. serverC.c - It receives the last one-third of the data elements in the file using UDP connection and computes the Reduction function requested on that data.
  4. aws.c     - It receives entire data from the client using TCP connection and distributes them equally among Servers A,B and C using UDP connection. ALso computes                   the final result from the data obtained from backend servers and sends back to client on the same TCP connection.
     All server programs uses static port numbers.
  5. client.c  - It takes the command line argument for the reduction type and the data on which the computation is performed is read from the csv file.Then it                          forwards the entire data with the reduction type and total data count to the AWS server using TCP connection and a dynamic TCP port.

e.Steps to run the program.

  Step 1: Type 'make' to compile all the server and client programs and generate the output files.
  Step 2: Run the program of backend servers first and then the aws server as below.
          make serverA
          make serverB
          make serverC
          make aws
  Step 3: Run the client program with the reduction function required.
          ./client {function}
 
f.Format of all the messages exchanged are of signed long integer or int32_t type.All the messages are put in one integer array and exchanged between servers or        client.The result is an integer of type int32.

g.The project fails under following conditions.
  1. For more than 3 backend servers.
  2. Input file with data of type float or strings.
  3. Input file other than csv and when total count of the file elements is not the multiple of 3.
 
h. Reused Code: Yes, I have referred beej tutorial and internet (https://www.youtube.com/watch?v=eVYsIolL2gE&list=PLGvfHSgImk4bJ7TfqJjrYHy8m3YKbGj5D) and used the code                 parts of echo server and client in my program(in send() and recv () calls) 
 

	

