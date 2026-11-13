#include <cctype>
#include <vector>
#include <iostream>
#include <openssl/evp.h>

bool checkHex(const char& c) {
  if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
    return true;
  }
  return false;
}

bool hashCharValidity(std::string hash) {
  for (char& c : hash) {
    if (!checkHex(c)) {
      return false;
    }
  }
  return true;
}

bool hashLengthValidity(std::string hash, std::string hashType) {
  for (char& c : hashType) {
    c = std::toupper(c);
  }
  if (hashType == "MD5") {
    if (hash.length() != 32) {
      return false;
    }
  } else if (hashType == "SHA-1" || hashType == "SHA1") {
    if (hash.length() != 40) {
      return false;
    }
  } else if (hashType == "SHA-256" || hashType == "SHA256") {
    if (hash.length() != 64) {
      return false;
    }
  } else if (hashType == "SHA512" || hashType == "SHA512") {
    if (hash.length() != 128) {
      return false;
    }
  }
  return true;
}

std::vector<unsigned char> hashWord (std::string word, std::string hashType) {
  EVP_MD_CTX* mdctx;
  const EVP_MD *md;
  unsigned char md_value[EVP_MAX_MD_SIZE];
  unsigned int md_len, i;
  std::vector<unsigned char> result;

  md = EVP_get_digestbyname(hashType.c_str());
  if (md == NULL) {
    std::cout << "Unknown message digest " << hashType << "\n"; 
    exit(1);
  }

  mdctx = EVP_MD_CTX_new();
  if (!EVP_DigestInit_ex2(mdctx, md, NULL)) {
    std::cout << "Message digest initialization failed. \n";
    EVP_MD_CTX_free(mdctx);
    exit(1);
  }

  if (!EVP_DigestUpdate(mdctx, word.c_str(), word.size())) {
    std::cout << "Message digest update failed. \n";
    EVP_MD_CTX_free(mdctx);
    exit(1);
  }

  if (!EVP_DigestFinal_ex(mdctx, md_value, &md_len)) {
    std::cout << "Message digest finalization failed. \n";
    EVP_MD_CTX_free(mdctx);
    exit(1);
  }

  EVP_MD_CTX_free(mdctx);
  for (int i = 0; i < md_len; i++) {
    result.push_back(md_value[i]);
  }
  return result;
}
