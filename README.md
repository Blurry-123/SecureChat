# Team SecureChat — Secure Chat Application

A multi-client secure chat application implemented in C using TCP sockets.
The application supports user registration, encrypted messaging, text-file
transfer, multiple simultaneous clients, error handling, and encrypted
client-server communication.

---

## Group Members

- **Member 1 — Shreya Acharya (24156096)**
  - Server implementation
  - TCP socket programming
  - Client-server communication
  - Concurrency and multiple-client handling
  - Server-side routing
  - Integration and final testing

- **Member 2 — Saswati Tripathy (24156092)**
  - Client implementation
  - Encryption and decryption
  - User registration
  - Chat commands
  - Client-server communication testing

- **Member 3 — Alipriya Mandal (24156040)**
  - Text file transfer
  - File validation and size limitation
  - Error handling and testing
  - Protocol testing
  - README and documentation

---

# How to Build

## 1. Requirements

The following software is required:

- Windows 10/11
- Visual Studio Code
- MinGW GCC compiler
- TCP/IP networking
- Tailscale (only when testing between computers on different networks)

The project does not require any external cryptography library.

The encryption algorithm is implemented manually in C.

The project uses the Windows **Winsock2** library for TCP socket
communication.

---

## 2. Project Files

The project contains the following important files:

    server.c
    client.c
    crypto.c
    crypto.h
    README.md

After compilation, the following executable files are generated:

    server.exe
    client.exe

---

## 3. Open the Project

Open Visual Studio Code and open the project folder.

Example:

    C:\Users\KIIT\Downloads\securechat

Open a terminal in VS Code:

    Terminal → New Terminal

The VS Code terminal can be PowerShell, Command Prompt, or Git Bash.

A separate Windows PowerShell window is NOT required.

---

## 4. Check GCC Installation

Before compiling, verify that MinGW GCC is installed.

Run:

    gcc --version

Example output:

    gcc.exe (MinGW.org GCC-6.3.0-1) 6.3.0

The project was developed and tested using MinGW GCC.

---

## 5. Compile the Server

From inside the project folder, run:

    gcc server.c -o server.exe -lws2_32

Explanation:

- `gcc` starts the GCC compiler.
- `server.c` is the server source file.
- `-o server.exe` creates the server executable.
- `-lws2_32` links the Windows Winsock2 library required for TCP socket
  programming.

If compilation is successful, no error message will be displayed and
`server.exe` will be created.

---

## 6. Compile the Client

Compile the client and encryption implementation using:

    gcc client.c crypto.c -o client.exe -lws2_32

Explanation:

- `client.c` contains the client implementation.
- `crypto.c` contains the encryption and decryption functions.
- `crypto.h` contains the declarations for the cryptographic functions.
- `-lws2_32` links the Windows Winsock2 library.

After successful compilation, `client.exe` will be created.

---

## 7. Check the Executables

The project directory should now contain:

    server.exe
    client.exe

along with:

    server.c
    client.c
    crypto.c
    crypto.h
    README.md

---

# How to Run

## 1. Start the Server

The computer acting as the server runs:

    .\server.exe 8080

The server displays:

    ==============================
          Secure Chat Server
    ==============================
    Server running on port 8080
    Waiting for clients...

The server must remain running while clients are connecting and
communicating.

Do not close the server terminal.

---

## 2. Find the Server IP Address

If the clients are on the same Wi-Fi/network as the server, the server's
local IPv4 address can be used.

Run:

    ipconfig

Find:

    IPv4 Address

For example:

    192.168.29.228

The client can then connect using:

    .\client.exe 192.168.29.228 8080

---

## 3. Testing Between Different Networks

If the server and clients are connected to different Wi-Fi networks,
the local IPv4 address generally cannot be used directly.

Tailscale can be used to create a private network between the computers.

After installing and signing into the same Tailscale network, obtain the
server's Tailscale IP address.

On Windows, if the `tailscale` command is not available directly, use:

    & "C:\Program Files\Tailscale\tailscale.exe" ip

For example, the server may have a Tailscale IPv4 address such as:

    100.104.194.122

The client then connects using:

    .\client.exe 100.104.194.122 8080

Both computers must be connected to the same Tailscale network.

---

# Client Registration

After the client successfully connects to the server, the client
registers with a unique username and encryption key.

For example:

    Username: sumo
    Key: sumokey123

Another client can register as:

    Username: monk
    Key: monkkey456

The server maintains a table of connected users and their corresponding
encryption keys.

Usernames must be unique.

If a duplicate username is registered, the server rejects the request.

Example:

    ERROR username already exists

---

# Chat Communication

The client can send a message to another online user.

Example:

    sumo$ - SEND TO monk: Hello, There!

The communication process is:

    Sumo Client
         |
         | Encrypted using Sumo's key
         v
    Server
         |
         | Decrypt using Sumo's key
         |
         | Look up Monk's key
         |
         | Re-encrypt using Monk's key
         v
    Monk Client
         |
         | Decrypt using Monk's key
         v
    Hello, There!

The receiver sees:

    monk$ - FROM sumo: Hello, There!

If the receiver is offline:

    server$ - ERROR monk is not online

---

# Cipher Choice

## Repeating-Key XOR Cipher

We chose the **Repeating-Key XOR cipher** for this project.

The XOR operation is performed between each character of the message and
a character of the repeating encryption key.

For example, if the message is longer than the key, the key is reused
from the beginning.

The same operation can be used for both encryption and decryption because:

    A XOR B XOR B = A

Therefore:

    Ciphertext = Plaintext XOR Key

and:

    Plaintext = Ciphertext XOR Key

---

## Why We Chose XOR

We selected the Repeating-Key XOR cipher because:

1. It is simple to understand and implement manually.
2. It does not require external cryptographic libraries.
3. Encryption and decryption use the same operation.
4. It is efficient for text communication.
5. It can be implemented easily in C.
6. It can operate on arbitrary bytes, allowing encrypted data to be
   transferred without depending on printable characters.

The simplicity of XOR also makes it suitable for demonstrating the
basic concept of symmetric-key encryption.

---

## Known Weakness of Repeating-Key XOR

Repeating-Key XOR is **not considered secure modern cryptography**.

The main weakness is that the encryption key is repeatedly reused.
If an attacker obtains enough ciphertext, patterns in the plaintext may
become visible.

It is therefore suitable for demonstrating encryption concepts in this
assignment, but it should not be used to protect sensitive information
in a real-world application.

A production secure chat application would use a modern authenticated
encryption algorithm such as AES-GCM or ChaCha20-Poly1305 together with
proper key exchange and authentication.

---

# Encryption / Decryption

All important communication between the clients and server is encrypted
before being sent through the socket.

This includes:

- Registration information
- Chat messages
- Text-file contents
- Server responses where applicable

The encryption and decryption functions are implemented in:

    crypto.c

Their declarations are provided in:

    crypto.h

The same key used for encryption is required for decryption.

---

# Design Notes

## 1. Hop-by-Hop Encryption

The application uses **hop-by-hop encryption**, not true end-to-end
encryption.

The communication follows:

    Client → Server → Client

For example:

    Sumo
      |
      | encrypted using Sumo's key
      v
    Server
      |
      | plaintext temporarily exists in server memory
      |
      | encrypted using Monk's key
      v
    Monk

The server decrypts the message using the sender's key and then
re-encrypts it using the receiver's key.

Therefore, the server temporarily has access to the plaintext.

This is a deliberate simplification required for the assignment.

A true end-to-end encrypted system would require the communicating
clients to establish a shared secret/key directly, so that the server
would forward ciphertext without being able to decrypt the message.
File Transfer Size Cap:1 MB

---

# 2. Server Architecture

The server uses TCP sockets.

The general server process is:

    1. Create socket
    2. Bind socket to port
    3. Listen for connections
    4. Accept clients
    5. Register clients
    6. Receive commands
    7. Decrypt incoming data
    8. Process the command
    9. Route the message/file
    10. Encrypt the response
    11. Send the response
    12. Continue serving clients

The server maintains information about connected users so that messages
can be routed to the correct recipient.

---

# 3. Concurrency

The server is designed to support multiple clients simultaneously.

For example:

    Sumo
      \
       \
       Server ---- Monk
       /
      /
    Alice

Multiple users can remain connected and communicate without one client
blocking all other clients.

The server uses a concurrent client-handling approach so that each
connected client can be processed independently.

This allows the following situation:

    Sumo  → Monk
    Alice → Sumo
    Monk  → Alice

to occur while all three clients remain connected.

---

# 4. Text File Transfer

The application supports sending `.txt` files between users.

Example:

    SENDFILE TO monk: notes.txt

The client:

1. Checks whether the file exists.
2. Checks that it is a `.txt` file.
3. Reads the file into memory.
4. Encrypts its contents using the sender's key.
5. Sends the request to the server.

The server:

1. Decrypts the content using the sender's key.
2. Checks that the receiver is online.
3. Encrypts the content using the receiver's key.
4. Sends the encrypted file to the receiver.

The receiver:

1. Decrypts the content.
2. Creates a local file.
3. Saves the received content using a `received_` prefix.

For example:

    notes.txt

is saved as:

    received_notes.txt

This prevents the original local file from being overwritten.

---

# 5. File Size Limitation

A maximum file size is used to prevent extremely large files from
being transferred.

The project uses a reasonable size limit so that file transfer can be
performed in memory without requiring chunked or streaming transfer.

Files exceeding the configured limit are rejected with an error.

Example:

    ERROR file too large

Only text files are supported.

Example:

    SENDFILE TO monk: report.pdf

results in:

    ERROR only .txt files are supported

---

# 6. File Framing

Text files can contain newline characters.

Therefore, a simple line-based delimiter cannot reliably determine where
the file content ends.

The protocol uses length information for file data so that the receiver
knows exactly how many bytes belong to the file payload.

Conceptually:

    SENDFILE TO monk notes.txt <length>
    <encrypted file data>

This allows files containing multiple lines to be transferred correctly.

---

# 7. Error Handling

The server must continue running even when clients send invalid input.

Examples include:

### Unknown command

    client$ - BLAHBLAH

Response:

    server$ - ERROR unknown command

### Invalid command format

    client$ - SEND monk Hello

Response:

    server$ - ERROR invalid command format

### Offline user

    SENDFILE TO ghost: notes.txt

Response:

    ERROR ghost is not online

### Missing file

    SENDFILE TO monk: missing.txt

Response:

    ERROR file not found: missing.txt

### Unsupported file type

    SENDFILE TO monk: report.pdf

Response:

    ERROR only .txt files are supported

The server should not terminate because of malformed input from one
client.

---

# 8. Client Disconnection

A client can disconnect using:

    QUIT

The server responds:

    GOODBYE <username>

The server then:

1. Closes the client's socket.
2. Removes the user from the connected-client table.
3. Releases the resources associated with that client.
4. Continues serving the remaining clients.

An abrupt disconnection, such as pressing `Ctrl+C`, should also be
handled without terminating the server.

---

# 9. Online Users

The client may use:

    LIST

to request the currently connected users.

The server responds with a list of online users.

Example:

    server$ - ONLINE monk, alice, bob

This allows clients to determine which users are currently connected.

---

# Testing

The following tests are performed to verify the application.

## Test 1 — Multiple Clients

At least three clients are connected:

    Sumo
    Monk
    Alice

They communicate simultaneously through the server.

Messages are delivered only to the intended recipient.

---

## Test 2 — Duplicate Username

A username that is already registered is used again.

Expected result:

    ERROR username already exists

The server continues running.

---

## Test 3 — Offline User

A message is sent to a user who is not connected.

Example:

    SEND TO ghost: Hello

Expected result:

    ERROR ghost is not online

---

## Test 4 — Abrupt Disconnection

A client is terminated using:

    Ctrl+C

The server detects the disconnection and removes the client.

Other connected clients continue to work.

---

## Test 5 — Malformed Input

Invalid commands and garbage input are sent to the server.

Expected behavior:

    ERROR invalid command format

or:

    ERROR unknown command

The server does not crash.

---

## Test 6 — Encryption/Decryption Round Trip

The encryption function is tested using:

- Empty input
- Short text
- Long text
- Spaces
- Numbers
- Special characters
- Delimiters
- Newline characters

The expected result is:

    Original
       ↓
    Encrypt
       ↓
    Ciphertext
       ↓
    Decrypt
       ↓
    Original

Therefore:

    decrypt(encrypt(message, key), key) = message

---

## Test 7 — Different Client Keys

Two clients use different encryption keys.

For example:

    Sumo → sumokey123
    Monk → monkkey456

The same plaintext message produces different ciphertext on the two
client-server communication legs because different keys are used.

The server decrypts the sender's ciphertext and re-encrypts the
plaintext using the receiver's key.

---

## Test 8 — File Transfer

A small `.txt` file is sent from one client to another.

Example:

    notes.txt

The receiver obtains:

    received_notes.txt

The decrypted file contents are compared with the original file.

The contents should be byte-identical.

---

## Test 9 — Relative File Path

A file in the current working directory is sent using its filename.

Example:

    SENDFILE TO monk: notes.txt

The transfer should succeed if the file exists.

---

## Test 10 — Full File Path

A file located elsewhere is sent using its full path.

Example:

    SENDFILE TO monk: C:\Users\KIIT\Documents\notes.txt

The client reads the file and sends it if it satisfies the file
requirements.

---

## Test 11 — Nonexistent File

Example:

    SENDFILE TO monk: missing.txt

Expected:

    ERROR file not found

---

## Test 12 — Oversized File

A file larger than the configured size limit is selected.

Expected:

    ERROR file too large

The server remains operational.

---

# Example Complete Execution

## Server Computer

Open VS Code terminal:

    cd C:\Users\KIIT\Downloads\securechat

Compile:

    gcc server.c -o server.exe -lws2_32

Start server:

    .\server.exe 8080

Output:

    ==============================
          Secure Chat Server
    ==============================
    Server running on port 8080
    Waiting for clients...

---

## Client 1 — Sumo

On the first client computer:

    gcc client.c crypto.c -o client.exe -lws2_32

Connect:

    .\client.exe 100.104.194.122 8080

Register:

    Username: sumo
    Key: sumokey123

---

## Client 2 — Monk

On another computer:

    gcc client.c crypto.c -o client.exe -lws2_32

Connect:

    .\client.exe 100.104.194.122 8080

Register:

    Username: monk
    Key: monkkey456

---

## Client 3 — Alice

On the third computer:

    gcc client.c crypto.c -o client.exe -lws2_32

Connect:

    .\client.exe 100.104.194.122 8080

Register:

    Username: alice
    Key: alicekey789

All three clients can now communicate through the server.

Example:

    sumo$ - SEND TO monk: Hello, There!

The server receives the encrypted message, decrypts it using Sumo's key,
looks up Monk's key, encrypts the message again, and forwards it.

Monk receives:

    monk$ - FROM sumo: Hello, There!

---

# How the Complete System Works

The complete communication architecture is:

                 +----------------+
                 |     Sumo       |
                 |    Client      |
                 +-------+--------+
                         |
                  Encrypted with
                   Sumo's key
                         |
                         v
                +--------+--------+
                |      SERVER     |
                |                 |
                | Decrypt message |
                |      ↓          |
                | Route message   |
                |      ↓          |
                | Encrypt message |
                +--------+--------+
                         |
                  Encrypted with
                   Monk's key
                         |
                         v
                 +-------+--------+
                 |      Monk      |
                 |     Client     |
                 +----------------+

The same process is used for file transfers, except that the file
contents are encrypted before transmission and decrypted after receipt.

---

# Design Limitations

The following limitations apply to the project:

1. The application uses Repeating-Key XOR, which is not suitable for
   real-world secure communication.

2. Encryption is hop-by-hop rather than true end-to-end encryption.

3. The server temporarily has access to plaintext while routing messages.

4. Only `.txt` files are supported.

5. File transfers are limited to the configured maximum file size.

6. File contents are held in memory during transfer rather than using
   chunked/streaming transfer.

7. The application is intended as an educational demonstration of
   socket programming, concurrency, protocol design, and symmetric
   encryption.

---

# Dependencies

No external cryptographic libraries are required.

The project uses:

- C
- MinGW GCC
- Windows Winsock2 (`ws2_32`)
- TCP sockets

Tailscale is only required when clients and the server are being tested
across different networks.

---

# Summary

The SecureChat application demonstrates:

- TCP client-server communication
- Multiple simultaneous clients
- User registration
- Username management
- Symmetric-key encryption
- Repeating-Key XOR encryption
- Message routing
- Text-file transfer
- File validation
- Error handling
- Client disconnection handling
- Network testing across different networks
- Hop-by-hop encrypted communication

The system provides a practical demonstration of how an encrypted
multi-client communication system can be designed and implemented using
C and TCP sockets.
