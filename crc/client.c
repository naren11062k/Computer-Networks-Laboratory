#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

char *computeMessage(char *input, char *key)
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
    rem[keylen - 1] = '\0';

    printf("\nQUOTIENT = %s", quot);

    printf("\nREMAINDER (CRC) = %s", rem);

    j = 0;
    for (i = 0; i < msglen; i++)
        message[j++] = input[i];

    for (i = 0; i < keylen - 1; i++)
        message[j++] = rem[i];
    message[j] = '\0';

    printf("\nFINAL DATA (DATA WITH CRC APPENDED): %s", message);

    return message;
}

int main()
{
    int network_socket;

    //Creating socket
    network_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (network_socket == -1)
    {
        printf("\nSOCKET CREATION FAILED!\n");
        exit(0);
    }
    else
    {
        printf("\nSOCKET CREATED SUCCESSFULLY!\n");
    }

    //Set Address`
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    //Connect to the socket
    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connection_status != 0)
    {
        printf("\nERROR IN CONNECTING!\n");
        exit(0);
    }
    else
    {
        printf("\nCONNECTION ESTABLISHED!\n");
    }

    //Send & Receive from socket
    char message[100];
    char poly[8];
    char CRC[8];
    char response[100];
    char mess_CRC[100];

    printf("\nEnter Message (Data) In Binary: ");
    scanf("%s", message);

    printf("Enter Encoded Polynomial (Divisor) In Binary: ");
    scanf("%s", poly);

    printf("\nENCODED POLYNOMIAL (DIVISOR) = %s", poly);

    strcpy(mess_CRC, computeMessage(message, strdup(poly)));

    printf("\n\nSENDING MESSAGE (FINAL DATA) %s TO SERVER!", mess_CRC);

    write(network_socket, mess_CRC, sizeof(mess_CRC));
    write(network_socket, poly, sizeof(poly));

    read(network_socket, response, sizeof(response));
    printf("\nRESPONSE: %s\n", response);

    if (mess_CRC[strlen(mess_CRC) - 1] == '0')
    {
        mess_CRC[strlen(mess_CRC) - 1] = '1';
    }
    else
    {
        mess_CRC[strlen(mess_CRC) - 1] = '0';
    }

    printf("\nComplemented Last Bit Of The Message (Final Data) = %s", mess_CRC);
    printf("\n\nSENDING MESSAGE (FINAL DATA) %s TO SERVER!", mess_CRC);

    write(network_socket, mess_CRC, sizeof(mess_CRC));
    write(network_socket, poly, sizeof(poly));

    read(network_socket, response, sizeof(response));
    printf("\nRESPONSE: %s\n", response);

    write(network_socket, "QUIT", sizeof("QUIT"));
    close(network_socket);
    return 0;
}
