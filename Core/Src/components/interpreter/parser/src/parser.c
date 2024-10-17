#include "parser.h"
#include <string.h>
/**
 * @brief Initializes a command instance by parsing the command string into root and arguments.
 *
 * @param[out] instance The command instance to be initialized.
 * @param[in] cmdString The command string to be parsed.
 */
void parseCommand(command_t *instance, unsigned char *cmdString)
{
    size_t rootLength = 0;
    unsigned char *cleanedCommand = cleanCmd(cmdString);
    size_t commandLength = strlen((char *)cleanedCommand);

    if (commandLength == 0)
    {
        return;
    }

    // Identify root command (e.g., THIS:IS:ROOT arg1 arg2)
    // Note, memchr returns pointer to char, cleanedCommand ptr gets subtracted to rootEnd(first space) and you get length
    unsigned char *rootEnd = (unsigned char *)memchr(cleanedCommand, ' ', commandLength);
    rootLength = rootEnd != NULL ? rootEnd - cleanedCommand : commandLength;
    instance->rootCommand = (unsigned char *)strndup((const char *)cleanedCommand, rootLength);
    instance->unformattedString = cleanedCommand;

    // Check if there are any arguments
    if (rootLength < commandLength)
    {
        size_t argumentStart = rootLength + 1; // Start from the first character of the argument, skip a space
        size_t argumentCount = 0;

        while (argumentStart < commandLength)
        {
            unsigned char *argumentEnd = (unsigned char *)memchr(cleanedCommand + argumentStart, ' ', commandLength - argumentStart);
            size_t argumentLength = argumentEnd == NULL ? commandLength - argumentStart : argumentEnd - (cleanedCommand + argumentStart);
            instance->parameters[argumentCount] = (unsigned char *)strndup((const char *)(cleanedCommand + argumentStart), argumentLength);
            argumentStart += argumentLength + 1;
            argumentCount++;
        }
        instance->paramsCount = argumentCount < MAX_PARAMS ? argumentCount : MAX_PARAMS;
    }
}
uint8_t preParseCommand(unsigned char *cleanedString)
{
    uint8_t res =
        grabLastChar(cleanedString, strlen((char *)cleanedString)) > -1;
    return res;
}
/**
 * @brief Cleans the command string by replacing multiple spaces with a single space and removing leading/trailing spaces.
 *
 * @param command_string The original command string.
 * @return char* The cleaned command string.
 */
unsigned char *cleanCmd(unsigned char *command_string)
{
    size_t command_length = strlen((char *)command_string);
    unsigned char *cleaned_string = malloc(command_length + 1);
    size_t cleaned_string_index = 0;
    size_t in_space = 0;

    for (size_t i = 0; i < command_length; i++)
    {
        unsigned char current_character = command_string[i];
        if (isspace(current_character))
        {
            if (!in_space && cleaned_string_index > 0 && cleaned_string_index < command_length - 1)
            {
                cleaned_string[cleaned_string_index++] = ' ';
                in_space = 1;
            }
        }
        else
        {
            cleaned_string[cleaned_string_index++] = tolower(current_character);
            in_space = 0;
        }
    }
    cleaned_string[cleaned_string_index] = '\0'; // Null terminate the cleaned string

    // Trim trailing space
    if (cleaned_string_index > 0 && cleaned_string[cleaned_string_index - 1] == ' ')
    {
        cleaned_string[cleaned_string_index - 1] = '\0';
    }

    // Trim leading space
    if (cleaned_string[0] == ' ')
    {
        memmove(cleaned_string, cleaned_string + 1, strlen((char *)cleaned_string));
    }

    return cleaned_string;
}
/**
 * @brief Counts the number of spaces in a string, excluding trailing spaces.
 *
 * @param string The string to be analyzed.
 * @param string_length The length of the string.
 * @return uint8_t The number of spaces in the string.
 */
uint8_t count_white_spaces(unsigned char *string, uint8_t string_length)
{
    uint8_t space_count = 0;
    uint8_t i;

    // Find the last non-space character
    for (i = string_length - 1; i != 0 && isspace(string[i]); i--)
        ;

    // Count spaces up to the last non-space character
    for (uint8_t j = 0; j < i; j++)
    {
        if (isspace(string[j]))
            space_count++;
    }

    return space_count;
}

/**
 * @brief Counts the number of characters before a space, starting from a given offset.
 *
 * @param[in] string The string to be analyzed.
 * @param[in] offset The offset to start counting from.
 * @param[in] length The length of the string.
 * @return uint8_t The number of characters before a space.
 */
uint8_t countCharactersBeforeSpace(const unsigned char *string, uint8_t offset, uint8_t length)
{
    uint8_t characterCount = 0;
    // Count characters before a space
    for (uint8_t i = offset; i < length; i++)
    {
        if (isspace(string[i]))
        {
            break;
        }
        characterCount++;
    }
    return characterCount;
}

/**
 * @brief Identifies the index of the last character of a command string.
 *
 * @param[in] string The string to be analyzed.
 * @param[in] string_length The length of the string.
 * @return int8_t The index of the last character.
 */
int8_t grabLastChar(unsigned char *string, uint8_t string_length)
{
    int8_t last_char_pos = -1;

    // Find the last character which is not a space
    for (int8_t i = string_length - 1; i >= 0; i--)
    {
        if (!isspace(string[i]) && string[i] != '\0')
        {
            last_char_pos = i;
            break;
        }
    }

    return last_char_pos;
}

/**
 * @brief Identifies the index of the last occurrence of a specific character in a string.
 *
 * @param string2Count The string to be analyzed.
 * @param key The character to find.
 * @param sizeStr The length of the string.
 * @return uint8_t The index of the last occurrence of the character.
 */
uint8_t grabLastSpecificChar(unsigned char *string2Count, unsigned char key, uint8_t sizeStr)
{
    unsigned char curChar;
    uint8_t lastCharPos = 0;

    // Find the last occurrence of the specific character
    for (uint8_t i = sizeStr; i > 0; i--)
    {
        curChar = string2Count[i];
        if (curChar == key)
        {
            lastCharPos = i;
        }
    }
    return lastCharPos;
}

/**
 * @brief Identifies the index of the first character of a command string.
 *
 * @param string2Count The string to be analyzed.
 * @param sizeStr The length of the string.
 * @return uint8_t The index of the first character.
 */
uint8_t grabFirstChar(unsigned char *string2Count, uint8_t sizeStr)
{
    uint8_t firstCharPos = 0;

    // Find the first character which is not a space
    for (uint8_t i = 0; i < sizeStr; i++)
    {
        if (!isspace(string2Count[i]))
        {
            firstCharPos = i;
            break;
        }
    }
    return firstCharPos;
}

/**
 * @brief Finds the index of the last non-space character in a piece of a string.
 *
 * @param string The string to analyze.
 * @param offset The starting offset.
 * @param length The length of the string.
 * @return uint8_t The index of the last non-space character.
 */
uint8_t findLastNonSpaceChar(const unsigned char *string, uint8_t offset, uint8_t length)
{
    uint8_t lastIndex = offset;

    for (uint8_t i = offset; i < length; ++i)
    {
        if (isspace(string[i]))
        {
            break;
        }

        lastIndex = i;
    }

    return lastIndex;
}

void strTolower(char *str)
{
    for (; *str; ++str)
        *str = tolower(*str);
}
int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++)
    {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}