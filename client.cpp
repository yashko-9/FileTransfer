#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <cstring>


void listFiles(int sockID){
    const uint32_t sizeOfCommand = 4;
    const char* command = "list";
    send(sockID, &sizeOfCommand, sizeof(sizeOfCommand), 0);
    send(sockID, command, sizeOfCommand, 0);
}

void getFile(int sockID, const char* filename){
    const uint32_t sizeOfCommand = 3;
    const char* command = "get";
    const uint32_t fileNameLength = strlen(filename);
    send(sockID, &sizeOfCommand, sizeof(sizeOfCommand), 0);
    send(sockID, command, sizeOfCommand, 0);
    send(sockID, &fileNameLength, sizeof(fileNameLength), 0);
    send(sockID, filename, fileNameLength, 0);

}

void uploadFile(int sockID, const char* filename){
    const uint32_t sizeOfCommand = 6;
    const char* command = "upload";
    const uint32_t fileNameLength = strlen(filename);
    send(sockID, &sizeOfCommand, sizeof(sizeOfCommand), 0);
    send(sockID, command, sizeOfCommand, 0);
    send(sockID, &fileNameLength, sizeof(fileNameLength), 0);
    send(sockID, filename, fileNameLength, 0);

}

void deleteFile(int sockID, const char* filename){
    const uint32_t sizeOfCommand = 6;
    const char* command = "delete";
    const uint32_t fileNameLength = strlen(filename);
    send(sockID, &sizeOfCommand, sizeof(sizeOfCommand), 0);
    send(sockID, command, sizeOfCommand, 0);
    send(sockID, &fileNameLength, sizeof(fileNameLength), 0);
    send(sockID, filename, fileNameLength, 0);
    
}

void send(int sockID){
    char buff[1024];
    char* command = NULL;
    char* filename = NULL;

    while(true){
        std::cout << "Enter a command: " << std::endl;
        std::cin.getline(buff, 30);
        command = strtok(buff, " ");
        filename = strtok(NULL, " ");

        if(strcmp(command, "list") == 0){
            listFiles(sockID);
        }
        else if (filename != NULL){
            if(strcmp(command, "get") == 0){
                getFile(sockID, filename);
            }
            else if(strcmp(command, "upload") == 0){
                uploadFile(sockID, filename);
            }
            else if(strcmp(command, "delete") == 0){
                deleteFile(sockID, filename);
            }
        }
        else{
            std::cout << "Wrong input" << std::endl;
        }
    
    }
}


int main(){
    hostent *host = gethostbyname("127.0.0.1");
    int sockID = 0; 
    if((sockID = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cout << "Socket creation error" << std::endl;
        return -1;
    }
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(6000);
    std::memcpy(&serverAddress.sin_addr.s_addr, host->h_addr, host->h_length);

    if(connect(sockID, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
        std::cout << "Connection failed" << std::endl;
        return -1;
    }
    
    std::cout << "Connection successful" << std::endl;
    
    send(sockID);
    
}