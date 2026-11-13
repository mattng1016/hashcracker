#include <cctype>
#include <iostream>
#include <openssl/evp.h>

/*clang++ main.cpp \
  -I"$(brew --prefix openssl@3)/include" \
  -L"$(brew --prefix openssl@3)/lib" \
  -lcrypto -o hashcracker */

#include "cracker.h"
#include "hashing.h"

enum mode { wordlist, salt };

int main(int argc, char *argv[]) {
  std::string wordlist, targetHash, word, saltValue, hashType;
  enum mode option;
  Salt salt;

  if (argc < 2) {
    std::cout << "Usage: ./hashcracker [--wordlist] [--hashvalue] "
                 "[--hashtype] [--salt] [--saltpos]\n";
    exit(1);
  }

  for (int i = 1; i < argc; i++) {
    std::string flag = argv[i];
    if (flag != "--wordlist" && flag != "--hashvalue" && flag != "--hashtype" &&
        flag != "--salt" && flag != "--saltpos") {
      std::cerr << "Unknown option: " << flag << '\n';
      return 1;
    }
    if (i + 1 >= argc) {
      std::cerr << "Missing flag value for " << flag << '\n';
      return 1;
    }
    std::string value = argv[++i];
    if (value.rfind("--", 0) == 0) {
      std::cerr << "Missing flag value for " << flag << '\n';
      return 1;
    }
    if (flag == "--wordlist") {
      wordlist = value;
      option = mode::wordlist;
    } else if (flag == "--hashvalue") {
      targetHash = value;
    } else if (flag == "--hashtype") {
      hashType = value;
    } else if (flag == "--salt") {
      salt.saltValue = value;
      option = mode::salt;
    } else if (flag == "--saltpos") {
      for (auto &c : value) {
        c = tolower(c);
      }
      if (value == "append") {
        salt.pos = SaltPosition::Append;
      } else if (value == "prepend") {
        salt.pos = SaltPosition::Prepend;
      } else {
        std::cerr << "Invalid value for --saltpos: " << value << "\n";
        exit(1);
      }
    }
  }

  if (wordlist.empty()) {
    std::cerr << "--wordlist requires a file path\n";
    exit(1);
  }

  if (targetHash.empty()) {
    std::cerr << "--hashvalue requires a target digest\n";
    exit(1);
  }

  if (hashType.empty()) {
    std::string guess = guessHash(targetHash);
    std::cout << "Guess hash: " << guess << "\n";
    if (guess == "NULL") {
      std::cerr << "Can't infer hash type from target digest\n";
      exit(1);
    }
    hashType = guess;
  }

  if (option == mode::wordlist) {
    Summary s = crackList(wordlist, targetHash, hashType, salt);
    displaySummary(s);
  } else if (option == mode::salt) {
    Summary s = crackList(wordlist, targetHash, hashType, salt);
    displaySummary(s);
  }
  exit(0);
}
