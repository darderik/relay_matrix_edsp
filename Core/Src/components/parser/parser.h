/**
 * @file      parser.h
 * @author    darderik
 * @brief     parser module header file
 * @copyright 2024, uniroma3
 */
#include <stdint.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "command.h"
#ifndef PARSER_H_
#define PARSER_H_
void cleanString(unsigned char *string2Clean, unsigned char *cleaned_string);
int8_t grabLastChar(unsigned char *string2Count, uint8_t sizeStr);
unsigned char *cleanCmd(unsigned char *cmdString, unsigned char *cleanedString);
uint8_t grabFirstChar(unsigned char *string2Count, uint8_t sizeStr);
uint8_t grabLastSpecificChar(unsigned char *string2Count, unsigned char key, uint8_t sizeStr);
void parseCommand(command_t *instance, unsigned char *cmdString);
uint8_t preParseCommand(unsigned char *cleanedString);
void strTolower(char *str);
int strcicmp(char const *a, char const *b);
#endif // PARSER_H_
