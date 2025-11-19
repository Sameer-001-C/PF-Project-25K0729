//message format is this:
// <Server_Code><Sender_Username><Reciever_Username><message>

void Extract_Reciever_Username(char *buffer, char *Reciever_Username_Out)
{
    // Copy 7 characters starting from message[8]
    for (int i = 0; i < 7; i++) {
        Reciever_Username_Out[i] = buffer[i + 8];
    }

    // Add null terminator at the end
    Reciever_Username_Out[7] = '\0';
}
// Use it like this: Extract_Reciever_Username(buffer, Reciever_Username);   ---> automatically stores in Reciever_Username
