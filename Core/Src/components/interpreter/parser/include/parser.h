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
void splitString(unsigned char *string2Split);
void cleanString(unsigned char *string2Clean);
uint8_t countWhiteSpaces(unsigned char *string2Count, uint8_t sizeStr);
int8_t grabLastChar(unsigned char *string2Count, uint8_t sizeStr);
uint8_t readSpacedStr(unsigned char *splittedStr, char *dest,uint8_t ofs,uint8_t size);
int8_t findFirstChar(unsigned char* str,unsigned char key,uint8_t ofs,uint8_t sizeStr);
unsigned char* cleanCmd(unsigned char *cmdString);
uint8_t grabLastChar_piece(unsigned char *string2Count, uint8_t ofs, uint8_t sizeStr);
uint8_t countLastChar_piece(unsigned char *string2Count, uint8_t ofs, uint8_t sizeStr);
void initCommand(command_t *instance, char *cmdString);
uint8_t grabFirstChar(unsigned char *string2Count, uint8_t sizeStr);
uint8_t grabLastSpecificChar(unsigned char *string2Count,unsigned char key, uint8_t sizeStr);
void parseCommand(command_t *instance, unsigned char *cmdString);
uint8_t preParseCommand(unsigned char *cleanedString);
void strTolower(char *str);
int strcicmp(char const *a, char const *b);
#endif // PARSER_H_

