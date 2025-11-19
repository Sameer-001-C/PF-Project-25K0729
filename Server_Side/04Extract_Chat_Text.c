// Extracts the chat text from the buffer

void Extract_Chat_Text(char *buffer, char *Chat_Text_Out, int buffer_length)
{
    // Chat text starts from index 15
    int chat_start = 15;

    if (buffer_length <= chat_start) {
        // No chat text available
        Chat_Text_Out[0] = '\0';
        return;
    }

    // Copy the rest of the message into Chat_Text_Out
    int chat_length = buffer_length - chat_start;

    // Be careful: Chat_Text_Out should be large enough to hold chat_length + 1
    memcpy(Chat_Text_Out, buffer + chat_start, chat_length);

    // Null-terminate the string
    Chat_Text_Out[chat_length] = '\0';
}
// Use it like this: Extract_Chat_Text(buffer, message, BUFFER_SIZE);
