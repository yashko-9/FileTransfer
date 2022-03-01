#include <thread>
#include <iostream>
#include <string.h>

#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>

#include <fstream>

bool authenticate(){
	return true;
}

void list_file(int sock_fd){
	std::cout << "\n list func\n";
	//list response
	
	//uint32_t number_of_files = 0;
	//send(sock_fd, &number_of_files, sizeof(number_of_files), 0);
	
	
}
void get_file(int sock_fd){
	std::cout << "\n get func\n";
	uint32_t fname_lenght = 0;
	recv(sock_fd, &fname_lenght, sizeof(fname_lenght), 0);
	
	char *filename = new char[fname_lenght];
	recv(sock_fd, &filename, fname_lenght, 0);
	
	//get response
	/*
	//file check
	std::ifstream f_stream("filename");
	if(!f_stream.is_open()){			//add button auth
		std::cout << "file not found!";
		send(sock_fd, "file not found", 14, 0);
	}else{
		f_stream.seekg(0, f_stream.end);
		int length = f_stream.tellg();
		f_stream.seekg(0, f_stream.beg);
		
		char * buffer = new char[length];
		file.read(buffer, length);
		
		
	}
	*/
	
	
	delete[] filename;
}

void upload_file(int sock_fd){
	std::cout << "\n upload func\n";
    const uint64_t MAX_FILE_SIZE = 10000;
	uint32_t fname_lenght = 0;
	recv(sock_fd, &fname_lenght, sizeof(fname_lenght), 0);
	
	char *filename = new char[fname_lenght];
	recv(sock_fd, filename, fname_lenght, 0);
	
	uint64_t file_size = 0;
	recv(sock_fd, &file_size, sizeof(file_size), 0);
	
    uint8_t responseStatus;

	std::ifstream file(filename);

    if(file.is_open()){
        responseStatus = 2;
		send(sock_fd, &responseStatus, sizeof(responseStatus), 0);
    }
    else if(file_size > MAX_FILE_SIZE){
        responseStatus = 3;
		send(sock_fd, &responseStatus, sizeof(responseStatus), 0);
    }
    else{
        responseStatus = 1;
		send(sock_fd, &responseStatus, sizeof(responseStatus), 0);

		std::ofstream file(filename, std::ios::binary);

		uint8_t *content = new uint8_t[file_size];
		recv(sock_fd, content, file_size, 0);

		file.write((const char*)content, file_size);

		delete[] content;
    }

	delete[] filename;
}

void delete_file(int sock_fd){
	std::cout << "\n delete func\n";
	uint32_t fname_lenght = 0;
	recv(sock_fd, &fname_lenght, sizeof(fname_lenght), 0);
	
	char *filename = new char[fname_lenght];
	recv(sock_fd, filename, fname_lenght, 0);

	uint8_t responseStatus;

	bool authenticated = authenticate();
	if(!authenticated){
		responseStatus = 3;
		send(sock_fd, &responseStatus, sizeof(responseStatus), 0);
	}
	else{
		std::ifstream file(filename);

    	if(!file.is_open()){
    	    responseStatus = 2;
			send(sock_fd, &responseStatus, sizeof(responseStatus), 0);
    	}
		else{
			std::remove(filename);

			responseStatus = 1;
			send(sock_fd, &responseStatus, sizeof(responseStatus), 0);
		}
	}
	
	delete[] filename;
}

void receive(int sock_fd){
	while(true){
		uint32_t command_size = 0;
		uint32_t r_size = recv(sock_fd, &command_size, sizeof(command_size) , 0);
		std::cout << "received " << command_size << "  " << r_size << "\n";
		if(r_size < sizeof(command_size)){
			if(r_size == 0){
				std::cout << "connection ended by client" << std::endl;
				close(sock_fd);
				break;
			}else if(r_size == -1){
				std::cerr << "failed to receive data " << strerror(errno) << std::endl;
			}else{
				continue;
			}
		}else{
			char *command = new char[command_size];
			recv(sock_fd, command, command_size, 0);
			

			if(strcmp("list", command) == 0){
				list_file(sock_fd);
			}
			else if(strcmp("get", command) == 0){
				get_file(sock_fd);
			}
			else if(strcmp("upload", command) == 0){
				upload_file(sock_fd);
			}
			else if(strcmp("delete", command) == 0){
				delete_file(sock_fd);
			}
			
			delete[] command;
		}
	}
}

int main(){
	int socket_fd;
	
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd == -1){
		std::cerr << "failed to create socket " << std::endl;
	}
	
	sockaddr_in my_addr;
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(6000);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	
	if(bind(socket_fd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1){
		std::cerr << "failed to bind " << strerror(errno) << std::endl;
	}
	
	
	if(listen(socket_fd, 10) == -1 ){
		std::cerr << "failed to listen " << strerror(errno) << std::endl;
	}
	

	while(1){
		int new_fd;
		sockaddr_in client_addr;
		socklen_t len = sizeof(client_addr);
		new_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &len);
		if(new_fd == -1){
			std::cerr << "failed to accept " << strerror(errno) << std::endl;
		}
		
		std::thread r(receive, new_fd);
		
		
		r.join();
	}
	close(socket_fd);
}