#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// QoS parameters (can be adjusted based on network conditions)
#define MAX_LATENCY 200  // Maximum latency (ms)
#define MIN_BANDWIDTH 500  // Minimum bandwidth (kbps)
#define PRIORITY_THRESHOLD 10  // Priority threshold (1-10 scale)

// Traffic classes
#define HIGH_PRIORITY 1
#define LOW_PRIORITY 2

// Function to simulate sending a packet and monitoring QoS
int send_packet(const char *ip, int port, const char *message, int priority, const char *mac_address) {
    // Simulate network behavior (latency, packet loss, bandwidth)
    int latency = rand() % 201 + 50;  // Simulate latency in ms (50 to 250 ms)
    int packet_loss = rand() % 11;  // Simulate packet loss chance (0-10%)

    if (packet_loss < 3) {  // 30% chance of packet loss for high-priority traffic
        printf("Packet loss occurred (message: %s)\n", message);
        return 0;
    }

    if (latency > MAX_LATENCY) {
        printf("Packet with high latency (%dms) for message: %s\n", latency, message);
        return 0;
    }

    // Simulate transmission delay based on the message size (emulating bandwidth)
    int bandwidth = rand() % 601 + 400;  // Random bandwidth in kbps (400 to 1000 kbps)

    if (bandwidth < MIN_BANDWIDTH) {
        printf("Insufficient bandwidth for message: %s\n", message);
        return 0;
    }

    printf("Sent message '%s' with %dms latency, %dkbps bandwidth from MAC: %s\n", message, latency, bandwidth, mac_address);
    return 1;
}

// Function to simulate network client
void *network_client(void *args) {
    char *ip = ((char **)args)[0];
    int port = atoi(((char **)args)[1]);
    char *mac_address = ((char **)args)[2];
    char *host_name = ((char **)args)[3];

    int priority = rand() % 10 + 1;
    char message[256];
    snprintf(message, sizeof(message), "Test message with priority %d", priority);

    printf("%s: Sending message with priority %d\n", host_name, priority);
    int success = send_packet(ip, port, message, priority, mac_address);

    if (!success) {
        printf("%s: Failed to send message '%s'\n", host_name, message);
    } else {
        printf("%s: Message sent successfully '%s'\n", host_name, message);
    }
    return NULL;
}

// Function to simulate network server
void *network_server(void *args) {
    char *ip = ((char **)args)[0];
    int port = atoi(((char **)args)[1]);

    printf("Server at %s:%d is ready to receive messages...\n", ip, port);
    while (1) {
        // Simulate receiving messages
        sleep(3);
        printf("Server: Listening for incoming traffic...\n");
    }
    return NULL;
}

// Function to simulate the ad-hoc network and monitor QoS
void monitor_qos() {
    // Prompting the user for necessary details
    char mac_address[18];
    char ip[16];
    char host_name[50];
    
    printf("Enter MAC address of the device:");
    scanf("%s", mac_address);
    printf("Enter IP address for the device:");
    scanf("%s", ip);
    printf("Enter host name:");
    scanf("%s", host_name);

    // Validate IP address format (basic validation)
    struct sockaddr_in sa;
    if (inet_pton(AF_INET, ip, &(sa.sin_addr)) <= 0) {
        printf("Invalid IP address format.\n");
        return;
    }

    printf("Starting network monitoring for host '%s' with IP: %s and MAC address: %s\n", host_name, ip, mac_address);
    int port = 8080;  // Port for communication
    
    // Start server in a separate thread
    pthread_t server_thread;
    char *server_args[] = { ip, "8080" };
    pthread_create(&server_thread, NULL, network_server, (void *)server_args);

    // Start clients in separate threads
    pthread_t client_threads[5];
    for (int i = 0; i < 5; i++) {  // Simulate 5 clients sending messages
        char *client_args[] = { ip, "8080", mac_address, host_name };
        pthread_create(&client_threads[i], NULL, network_client, (void *)client_args);
        sleep(rand() % 3 + 1);  // Random delay between clients sending messages
    }

    // Monitor the QoS performance
    while (1) {
        printf("\nQoS Monitoring - Current Performance Metrics:\n");
        printf("Latency: %d ms\n", rand() % (MAX_LATENCY + 1) + 50);
        printf("Bandwidth: %d kbps\n", rand() % 601 + 400);
        printf("Packet Loss: %d%%\n", rand() % 11);
        printf("Throughput: %d kbps\n", rand() % 601 + 400);
        printf("==================================================\n");
        sleep(5);  // Monitor every 5 seconds
    }
}

int main() {
    srand(time(NULL));  // Seed for random number generation
    monitor_qos();
    return 0;
}
