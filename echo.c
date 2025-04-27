#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define LISTEN_BACKLOG 10

void *handle_client(void *arg) {
  int client_fd = *(int *)arg;
  free(arg);

  char buffer[BUFFER_SIZE];

  while (1) {
    ssize_t bytes_received = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_received <= 0) {
      break;
    }

    buffer[bytes_received] = '\0';
    printf("Received: %s", buffer);

    write(client_fd, buffer, bytes_received);
  }

  close(client_fd);
  printf("Client disconnected\n");
  return NULL;
}

int main() {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_fd < 0) {
    perror("socket");
    return 1;
  }

  struct sockaddr_in socket_address;
  memset(&socket_address, '\0', sizeof(socket_address));
  socket_address.sin_family = AF_INET;
  socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
  socket_address.sin_port = htons(PORT);

  printf("Binding to port %d...\n", PORT);
  if (bind(socket_fd, (struct sockaddr *)&socket_address,
           sizeof(socket_address)) < 0) {
    perror("bind");
    close(socket_fd);
    return 1;
  }

  if (listen(socket_fd, LISTEN_BACKLOG) < 0) {
    perror("listen");
    close(socket_fd);
    return 1;
  }

  printf("Server listening on port %d...\n", PORT);

  while (1) {
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    int *client_fd_buf = malloc(sizeof(int));
    if (!client_fd_buf) {
      perror("malloc");
      continue;
    }

    *client_fd_buf = accept(socket_fd, (struct sockaddr *)&client_address,
                            &client_address_len);

    if (*client_fd_buf < 0) {
      perror("accept");
      free(client_fd_buf);
      continue;
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_address.sin_addr, client_ip, sizeof(client_ip));
    printf("Accepted connection from %s:%d\n", client_ip,
           ntohs(client_address.sin_port));

    pthread_t thread;
    pthread_create(&thread, NULL, handle_client, client_fd_buf);
    pthread_detach(thread);
  }

  close(socket_fd);
  return 0;
}