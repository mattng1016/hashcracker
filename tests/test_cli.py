import hashlib
import tempfile
from pathlib import Path

import subprocess
import unittest

class TestCLI(unittest.TestCase):
    def test_no_args(self):
        result = subprocess.run(["./hashcracker"], capture_output=True, text=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Usage: ", result.stdout + result.stderr)
    def test_wrong_args(self):
        result = subprocess.run(["./hashcracker", "--flag"], capture_output=True, text=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Unknown option: ", result.stdout + result.stderr)
    def test_no_hashvalue(self):
        result = subprocess.run(["./hashcracker", "--wordlist" , "wordlist.txt"], capture_output=True, text=True)
        self.assertNotEqual((result.returncode), 0)        
        self.assertIn("--hashvalue requires a target digest", result.stdout + result.stderr)
    def test_no_wordlist(self):
        result = subprocess.run(["./hashcracker", "--hashvalue", "3a7bd3e2360a3d29eea436fcfb7e44c735d117c42d1c1835420b6b9942dd4f1b"], capture_output=True, text=True)
        self.assertNotEqual((result.returncode), 0)        
        self.assertIn("--wordlist requires a file path", result.stdout + result.stderr)
    def test_invalid_hash(self):
        result = subprocess.run(["./hashcracker", "--hashvalue", "3a7bd3e2360a3d29eea436fcfb7e44c735d117c42d1c1835420b6b9942dzgf1b", "--wordlist", "wordlist.txt"], capture_output=True, text=True)
        self.assertNotEqual((result.returncode), 0)        
        self.assertIn("Invalid hash character", result.stdout + result.stderr)
    def test_invalid_hashlength(self):
        result = subprocess.run(["./hashcracker", "--hashvalue", "abc", "--wordlist", "wordlist.txt", "--hashtype", "sha256"], capture_output=True, text=True)
        self.assertNotEqual((result.returncode), 0)        
        self.assertIn("Invalid hash length for ", result.stdout + result.stderr)


class TestWordlistSearch(unittest.TestCase):
    def setUp(self):
        directory = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        self.wordlist = Path(directory.name) / "words.txt"
        self.wordlist.write_text("apple\nbanana\ncherry\n", encoding="utf-8")

    def search(self, text, *options):
        digest = hashlib.sha256(text.encode("utf-8")).hexdigest()
        result = subprocess.run(
            ["./hashcracker", "--wordlist", str(self.wordlist),
             "--hashvalue", digest, *options],
            capture_output=True,
            text=True,
            timeout=5,
        )
        self.assertEqual(result.returncode, 0, result.stderr)
        return result.stdout

    def test_first_and_last_match(self):
        for word, attempts in [("apple", 1), ("cherry", 3)]:
            with self.subTest(word=word):
                output = self.search(word, "--hashtype", "sha256")
                self.assertIn(f"Hash cracked: {word}\n", output)
                self.assertRegex(output, rf"\|\s+Attempts\s+{attempts}\s+\|")

    def test_no_match(self):
        output = self.search("pear", "--hashtype", "sha256")
        self.assertIn("Word not found", output)
        self.assertRegex(output, r"\|\s+Attempts\s+3\s+\|")

    def test_infers_sha256(self):
        output = self.search("banana")
        self.assertIn("Hash cracked: banana\n", output)

    def test_salt_positions(self):
        for position, salted in [("prepend", "demobanana"), ("append", "bananademo")]:
            with self.subTest(position=position):
                output = self.search(salted, "--hashtype", "sha256",
                                     "--salt", "demo", "--saltpos", position)
                self.assertIn("Hash cracked:", output)
                self.assertRegex(output, r"\|\s+Attempts\s+2\s+\|")
