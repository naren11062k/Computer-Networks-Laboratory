#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

int validateMessage(char *input, char *key)
{
    int i, j, keylen, msglen;
    char temp[30], quot[100], rem[30], key1[30];
    static char message[100];
    keylen = strlen(key);
    msglen = strlen(input);
    strcpy(key1, key);

    for (i = 0; i < keylen - 1; i++)
    {
        input[msglen + i] = '0';
    }

    for (i = 0; i < keylen; i++)
        temp[i] = input[i];

    for (i = 0; i < msglen; i++)
    {
        quot[i] = temp[0];
        if (quot[i] == '0')
        {
            for (j = 0; j < keylen; j++)
            {
                key[j] = '0';
            }
        }
        else
        {
            for (j = 0; j < keylen; j++)
                key[j] = key1[j];
        }

        for (j = keylen - 1; j > 0; j--)
        {
            if (temp[j] == key[j])
            {
                rem[j - 1] = '0';
            }
            else
            {
                rem[j - 1] = '1';
            }
        }
        rem[keylen - 1] = input[i + keylen];
        strcpy(temp, rem);
    }
    quot[i] = '\0';
    strcpy(rem, temp);

    printf("\nREMAINDER = %s", rem);

    for (i = 0; i < strlen(rem); i++)
    {
        if (rem[i] == '1')
        {
            printf("\nThe Received Message Is INCORRECT!");
            return 0;
        }
    }
    printf("\nThe Received Message Is CORRECT!");
    return 1;
}

int main()
{
    //Create Socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        printf("\nSOCKET CREATION FAILED!\n");
        exit(0);
    }
    else
        printf("\nSOCKET CREATED SUCCESSFULLY!\n");

    //Set server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9000);
    server_address.sin_addr.s_addr = INADDR_ANY;
    printf("\nSERVER ADDRESS SET!\n");

    //Bind
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == 0)
    {
        printf("\nBIND SUCCESSFUL!\n");
    }
    else
    {
        printf("\nBIND FAILED!\n");
        exit(0);
    }

    //Listen
    if (listen(server_socket, 5) == 0)
    {
        printf("\nLISTENING...\n");
    }
    else
    {
        printf("\nLISTEN FAILED!\n");
    }

    //Accept
    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);
    if (client_socket < 0)
    {
        printf("\nFAILED IN ACCEPTING!\n");
        exit(0);
    }
    else
    {
        printf("\nACCEPTED SUCCESSFULLY!\n");
    }

    //Send and Receive
    //exit whenever either server or client sends "exit" message
    while (1)
    {
        char message_b[100];
        char poly[8];
        int flag;

        read(client_socket, message_b, sizeof(message_b));
        if (strcmp(message_b, "QUIT") == 0)
        {
            break;
        }
        read(client_socket, poly, sizeof(poly));

        printf("\n\nMESSAGE RECEIVED (FINAL DATA) = %s ", message_b);
        printf("\nENCODED POLYNOMIAL (DIVISOR) = %s", poly);
        flag = validateMessage(message_b, strdup(poly));

        if (flag == 1)
        {
            write(client_socket, "CORRECT DATA", sizeof("CORRECT DATA"));
        }
        else
        {
            write(client_socket, "INCORRECT DATA", sizeof("INCORRECT DATA"));
        }
    }
    printf("\n");
    close(server_socket);
    return 0;
}
