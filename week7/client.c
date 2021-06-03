#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define size 1024

int minimum(int a, int b, int c)
{
    if ((a <= b) && (a <= c))
        return a;
    else if ((b <= a) && (b <= c))
        return b;
    else
        return c;
}

int main()
{
    int sockfd, connfd, n = 0;
    int m, np;
    float rtt, ud;
    struct sockaddr_in serv_addr;
    char buffer[size], packets[100];

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(7500);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Socket creation failed\n");
        return 1;
    }

    connfd = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    printf("Enter RTT: ");
    scanf("%f", &rtt);
    printf("Enter Uniform Delay: ");
    scanf("%f", &ud);
    printf("Enter Number Of Packets: ");
    scanf("%d", &np);
    printf("Enter Packet Message: ");
    scanf("%s", packets);

    int i = 0, count = 0;
    m = (int)(rtt / ud);
    int m2 = m;
    while (i < np)
    {
        if (i == 0)
        {
            write(sockfd, &packets[0], sizeof(packets[0]));
            i++;
        }
        else
        {
            n = minimum(m, m2 - i, np - i);
            int j;
            for (j = 0; j < n; j++)
            {
                buffer[j] = packets[i++];
            }
            buffer[j] = '\0';
            write(sockfd, buffer, strlen(buffer));
            m2 += m;
        }
        count++;

        n = read(sockfd, buffer, size);
        buffer[n] = '\0';
        printf("Receiver: %s\n", buffer);
    }
    printf("Total Time: %lf\n", count * rtt);
}
