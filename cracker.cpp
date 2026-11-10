#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#include "cracker.h"
#include "hashing.h"

void displaySummary(const Summary &summary) {
  double seconds = summary.duration.count();

  std::ostringstream elapsed;
  elapsed << std::fixed << std::setprecision(6) << seconds << " s";

  std::ostringstream speed;
  if (seconds > 0.0) {
    speed << std::fixed << std::setprecision(0) << summary.attempts / seconds
          << " guesses/s";
  } else {
    speed << "N/A";
  }

  std::ostringstream output;
  output << std::left << "+------------------------------------------------+\n"
         << "|                Search Summary                  |\n"
         << "+------------------------------------------------+\n"
         << "| " << std::setw(18) << "Attempts" << std::setw(28)
         << summary.attempts << " |\n"
         << "| " << std::setw(18) << "Elapsed" << std::setw(28) << elapsed.str()
         << " |\n"
         << "| " << std::setw(18) << "Average speed" << std::setw(28)
         << speed.str() << " |\n"
         << "+------------------------------------------------+\n";

  std::cout << output.str();
}

FILE *validateFile(std::string fileName) {
  FILE *f = fopen(fileName.c_str(), "r");
  if (!f) {
    std::cout << "Error opening" << fileName << "\n";
    exit(1);
  }
  return f;
}

int asciitovalue(const char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return -1;
}

std::vector<unsigned char> hexToByte(std::string hashValue) {
  std::vector<unsigned char> value;
  for (int i = 0; i < hashValue.length(); i += 2) {
    int first = asciitovalue(hashValue[i]);
    int second = asciitovalue(hashValue[i + 1]);
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

Summary crackList(std::string fileName, std::string hashValue,
                  std::string hashType) {
  std::ifstream file;
  std::string word;
  Summary s;

  file.open(fileName);
  if (!file) {
    std::cout << "Invalid file: " << fileName << "\n";
    exit(1);
  } else if (file.peek() == EOF) {
    std::cout << "Empty file: " << fileName << "\n";
    exit(1);
  }

  const auto startTime = std::chrono::steady_clock::now();
  while (std::getline(file, word)) {
    s.attempts++;
    if (crackWord(word, hashValue, hashType)) {
      std::cout << "\nHash cracked: " << word << "\n";
      const auto endTime = std::chrono::steady_clock::now();
      s.duration = endTime - startTime;
      return s;
    }
  }
  const auto endTime = std::chrono::steady_clock::now();
  s.duration = endTime - startTime;
  std::cout << "Word not found\n";
  return s;
}
