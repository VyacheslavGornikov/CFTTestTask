#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_

#include "common.h"

int StoreDump (StatData* binData, int size, const char* filePath);
int LoadDump (StatData** binData, int* size, const char* filePath);
int GetValidDataCount (StatData* binData, int size);
int IsValidData(StatData *binData);

#endif /* _SERIALIZER_H_ */
