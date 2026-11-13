# hashcracker

A C++ command-line dictionary hash cracker built with OpenSSL on macOS. It hashes candidate words and compares their digest bytes against a supplied target hash.

This is a learning project covering OpenSSL's EVP API, command-line arguments, hexadecimal conversion, file reading, and performance measurement.

## Features

- Dictionary search with one candidate per line.
- MD5, SHA-1, SHA-256, and SHA-512 through OpenSSL, with optional digest-length inference.
- Raw-byte digest comparison.
- Optional text salt, prepended or appended to each candidate.
- Search summary showing attempts, elapsed time, and average guesses per second.

## Build

Requires a C++17 compiler, Xcode Command Line Tools, Homebrew, and OpenSSL 3.

```bash
brew install openssl@3

clang++ -std=c++17 -Wall -Wextra \
  main.cpp hashing.cpp cracker.cpp \
  -I"$(brew --prefix openssl@3)/include" \
  -L"$(brew --prefix openssl@3)/lib" \
  -lcrypto -o hashcracker
```

Run the build command from the project directory. Only the three listed source files are needed.

## Usage

### Search a wordlist

Create a small example wordlist and search for the SHA-256 hash of `password`.

```bash
printf 'apple\ndragon\npassword\n' > words.txt

./hashcracker \
  --wordlist words.txt \
  --hashvalue 5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8
```

The program stops at the first matching candidate, or reports `Word not found` after exhausting the list. In this example, it should report three attempts.

```text
Hash cracked: password
+------------------------------------------------+
|                Search Summary                  |
+------------------------------------------------+
| Attempts          3                            |
| Elapsed           ... s                        |
| Average speed     ... guesses/s                |
+------------------------------------------------+
```

Elapsed time and speed depend on the machine, algorithm, and number of candidates tried.

### Search with a salt

Salt support uses direct concatenation of literal text.

```text
prepend → hash(salt + candidate)
append  → hash(candidate + salt)
```

Generate a target locally for salt `demo` followed by candidate `password`, then search for it.

```bash
target=$(printf '%s' 'demopassword' | shasum -a 256 | awk '{print $1}')

./hashcracker \
  --wordlist words.txt \
  --hashtype SHA256 \
  --hashvalue "$target" \
  --salt demo \
  --saltpos prepend
```

Prepend is the default when `--saltpos` is omitted. For append mode, generate the target from `passworddemo` and use `--saltpos append`.

The current match message displays the combined salted input, such as `demopassword`.

### Arguments

| Flag | Value | Purpose |
| --- | --- | --- |
| `--wordlist` | File path | Read candidate words from a file |
| `--hashvalue` | Hexadecimal digest | Target hash to compare against |
| `--hashtype` | Algorithm name | Optional. Select a digest such as `SHA256` |
| `--salt` | Text | Salt used when searching a wordlist |
| `--saltpos` | `prepend` or `append` | Salt position; defaults to prepend |

Supply both `--wordlist` and `--hashvalue`. If you omit `--hashtype`, the program guesses from the hexadecimal digest length. It maps 32, 40, 64, and 128 characters to MD5, SHA-1, SHA-256, and SHA-512, respectively. Different algorithms can share a digest length, so specify the type explicitly when you know it.

## Tests

Build the executable first, then run the CLI tests from the project root with Python 3:

```bash
python3 -m unittest discover -s tests -v
```

The suite covers CLI validation, successful and unsuccessful searches, SHA-256 inference, and both salt positions. Search tests create a temporary wordlist with known contents.

## Project structure

```text
main.cpp       Parse arguments and select a mode
cracker.cpp    Convert target hex, search candidates, and print statistics
cracker.h      Search declarations, salt settings, and summary structure
hashing.cpp    Validate supported digest lengths and calculate hashes with EVP
hashing.h      Hashing declarations
```

Each word is hashed independently. A dictionary search succeeds only if a matching candidate appears in the file under the chosen algorithm and salt settings.
