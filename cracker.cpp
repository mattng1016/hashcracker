#include <fstream>
#include <iostream>
#include "hashing.h"

FILE* validateFile(std::string fileName) {
  FILE* f = fopen(fileName.c_str(), "r");
  if (!f) {
    std::cout << "Error opening" << fileName << "\n";
    exit(1);
  }
  return f;
}

int asciitovalue(const char c) {
  if (c >= '0' && c <= '9') return c - '0'; 
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}

std::vector<unsigned char> hexToByte(std::string hashValue) {
  std::vector<unsigned char> value;
  for (int i = 0; i < hashValue.length(); i+=2) {
    int first = asciitovalue(hashValue[i]);
    int second = asciitovalue(hashValue[i+1]);
    u_char temp = (first << 4) | second;
    value.push_back(temp);
  }
  return value;
}

bool crackWord(std::string word, std::string hashValue, std::string hashType) {
  std::vector<unsigned char> digest = hashWord(word, hashType);
  std::vector<unsigned char> hV = hexToByte(hashValue);

  if (digest == hV) {
    return true;
  } else {
    return false;
  }
}

void crackList(std::string fileName, std::string hashValue, std::string hashType) {
  std::ifstream file;
  std::string word;
  file.open(fileName);
  if (!file) {
    std::cout << "Invalid filename: " << fileName << "\n";
    exit(1);
  }

  while (std::getline(file, word)) {
    if (crackWord(word, hashValue, hashType)) {
      std::cout << "Hash cracked: " << word << "\n";
      return;
    }
  } 
  std::cout << "Hash not cracked";
}



