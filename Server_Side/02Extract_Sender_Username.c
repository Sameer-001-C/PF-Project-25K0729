//message format is this:
// <Server_Code><Sender_Username><Reciever_Username><message>

void Extract_Sender_Username(char *buffer, char *Sender_Username_Out)
{
    // Copy 7 characters starting from message[1]
    for (int i = 0; i < 7; i++) {
        Sender_Username_Out[i] = buffer[i + 1];
    }

    // Add null terminator at the end
    Sender_Username_Out[7] = '\0';
}
// Use it like this: Extract_Sender_Username(buffer, Sender_Username);   ---> automatically stores in Sender_Username
