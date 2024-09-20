#include "parser.h"

/**
 * @brief Initializes a command instance by parsing the command string into root and arguments.
 *
 * @param instance The command instance to be initialized.
 * @param cmdString The command string to be parsed.
 */
void parseCommand(command_t *instance, unsigned char *cmdString)
{
    unsigned char *rootHolder;
    unsigned char *unformattedString = cleanCmd(cmdString); // Clean the command string
    uint8_t isValid = preParseCommand(unformattedString);
    if (!isValid)
    {
        return;
    }

    // Identify root command (e.g., THIS:IS:ROOT arg1 arg2)
    uint8_t rootSize = countLastChar_piece(unformattedString, 0, strlen((char *)unformattedString)); // Include null terminator
    rootHolder = calloc(rootSize + 1, sizeof(char));
    strncpy((char *)rootHolder, (char *)unformattedString, rootSize);
    rootHolder[rootSize] = '\0'; // Null terminate
    instance->rootCommand = rootHolder;
    instance->unformattedString = unformattedString;

    // Check if there are any arguments
    if (rootSize != strlen((char *)unformattedString))
    {
        uint8_t curArgStart = rootSize + 1; // Start from the first character of the argument, skip a space
        uint8_t argsCount = countWhiteSpaces(unformattedString, strlen((char *)unformattedString));
        uint8_t offset = 0;

        for (uint8_t i = 0; i < argsCount && i < MAX_PARAMS; i++)
        {
            curArgStart += offset;
            uint8_t curArgLength = countLastChar_piece(unformattedString, curArgStart, strlen((char *)unformattedString));
            unsigned char *curArg = calloc(curArgLength + 1, sizeof(char));
            strncpy((char *)curArg, (char *)(unformattedString + curArgStart), curArgLength);
            curArg[curArgLength] = '\0'; // Null terminate
            offset = curArgLength + 1;
            instance->parameters[i] = curArg;
        }
        instance->paramsCount = argsCount < MAX_PARAMS ? argsCount : MAX_PARAMS;
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
 * @param cmdString The original command string.
 * @return char* The cleaned command string.
 */
unsigned char *cleanCmd(unsigned char *cmdString)
{
    uint8_t strL = strlen((char *)cmdString);
    unsigned char cleanedStr[strL + 1];
    unsigned char *cleanedStrFinal;
    uint8_t j = 0;
    uint8_t in_space = 0;
    for (uint8_t i = 0; i < strL; i++)
    {
        unsigned char curChar = cmdString[i];
        if (isspace(curChar) || (0) /*add other characters*/)
        {
            if (!in_space && j != 0 && j != strL - 1)
            {
                cleanedStr[j++] = ' ';
                in_space = 1;
            }
        }
        else
        {
            cleanedStr[j++] = tolower(curChar);
            in_space = 0;
        }
    }
    cleanedStr[j] = '\0'; // Null terminate the cleaned string

    // Trim trailing space
    if (j > 0 && cleanedStr[j - 1] == ' ')
    {
        cleanedStr[j - 1] = '\0';
    }

    // Trim leading space
    if (cleanedStr[0] == ' ')
    {
        memmove(cleanedStr, cleanedStr + 1, strlen((char *)cleanedStr));
    }
    cleanedStrFinal = malloc(strlen((char *)cleanedStr) + 1);
    strcpy((char *)cleanedStrFinal, (char *)cleanedStr);
    return cleanedStrFinal;
}
/**
 * @brief Counts the number of spaces in a string, excluding trailing spaces.
 *
 * @param string2Count The string to be analyzed.
 * @param sizeStr The length of the string.
 * @return uint8_t The number of spaces in the string.
 */
uint8_t countWhiteSpaces(unsigned char *string2Count, uint8_t sizeStr)
{
    uint8_t spacesCtr = 0;
    unsigned char curChar;
    uint8_t lastCharPos = 0;

    // Find the last character which is not a space
    for (uint8_t i = 0; i < sizeStr; i++)
    {
        curChar = string2Count[i];
        if (curChar != ' ' && curChar != '\0')
        {
            lastCharPos = i;
        }
    }

    // Count spaces up to the last character
    for (uint8_t i = 0; i < lastCharPos; i++)
    {
        curChar = string2Count[i];
        if (curChar == ' ')
        {
            spacesCtr++;
        }
    }
    return spacesCtr;
}

/**
 * @brief Counts the number of characters before a space, starting from a given offset.
 *
 * @param string2Count The string to be analyzed.
 * @param ofs The offset to start counting from.
 * @param sizeStr The length of the string.
 * @return uint8_t The number of characters before a space.
 */
uint8_t countLastChar_piece(unsigned char *string2Count, uint8_t ofs, uint8_t sizeStr)
{
    unsigned char curChar;
    uint8_t lastCharPos = ofs;

    // Find the last character before a space
    for (uint8_t i = ofs; i < sizeStr; i++)
    {
        curChar = string2Count[i];
        if (!isspace(curChar) && curChar != '\0')
        {
            lastCharPos = i;
        }
        else
        {
            break;
        }
    }
    return lastCharPos + 1 - ofs;
}

/**
 * @brief Identifies the index of the last character of a command string.
 *
 * @param string2Count The string to be analyzed.
 * @param sizeStr The length of the string.
 * @return uint8_t The index of the last character.
 */
int8_t grabLastChar(unsigned char *string2Count, uint8_t sizeStr)
{
    unsigned char curChar;
    int8_t lastCharPos = -1;
    // Find the last character which is not a space
    for (uint8_t i = sizeStr; i > 0; i--)
    {
        curChar = string2Count[i];
        if (!isspace(curChar) && curChar != '\0')
        {
            lastCharPos = i;
            break;
        }
    }
    return lastCharPos;
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

/**
 * @brief Finds the first occurrence of a specific character in a string, starting from a given offset.
 *
 * @param str The string to be analyzed.
 * @param key The character to find.
 * @param ofs The offset to start from.
 * @param sizeStr The length of the string.
 * @return int8_t The index of the first occurrence of the character, or -1 if not found.
 */
int8_t findFirstChar(unsigned char *str, unsigned char key, uint8_t ofs, uint8_t sizeStr)
{
    for (uint8_t i = ofs; i < sizeStr; i++)
    {
        unsigned char curChar = str[i];
        if (curChar == key)
        {
            return i;
        }
    }
    return -1; // Character not found
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