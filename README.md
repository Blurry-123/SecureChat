# Team SecureChat — Secure Chat Application

A multi-client secure chat application implemented in C using TCP sockets.

The application supports:

- User registration
- Multiple simultaneous clients
- Encrypted client-server communication
- Message routing between users
- Text file transfer
- File validation
- File size limitation
- Error handling
- Client disconnection
- Online user listing
- Testing between multiple computers on the same Wi-Fi network

The project was developed as a three-member team using GitHub for collaboration and integration.


# Group Members

- Member 1 — Shreya Acharya (24156096)
  - Server implementation
  - TCP socket programming
  - Client-server communication
  - Multiple-client handling
  - Server-side message routing
  - Integration and final testing

- Member 2 — Saswati Tripathy (24156092)
  - Client implementation
  - Encryption and decryption
  - User registration
  - Chat commands
  - Client-server communication testing

- Member 3 — Alipriya Mandal (24156040)
  - Text file transfer
  - File validation
  - File size limitation
  - Error handling
  - Protocol testing
  - README and documentation


# Project Collaboration

The project was maintained using a GitHub repository so that all three team members could work on the same SecureChat project.

The repository contains the common project files:

    server.c
    client.c
    crypto.c
    crypto.h
    README.md

Each team member cloned the same repository onto their own laptop.

The files were kept in the same project structure so that changes made by one member could be integrated by the other members using Git.

The repository was also used to correct the original file extensions.

Initially, some source files were stored with `.txt` extensions:

    client.c.txt
    crypto.c.txt
    crypto.h.txt

These were renamed to:

    client.c
    crypto.c
    crypto.h

The corrected files were committed and pushed to the repository so that all team members could use the proper C source files.


# Development Environment

The project was developed and tested on Windows.

Required software:

- Windows 10/11
- Visual Studio Code
- MinGW GCC
- Git
- TCP/IP networking

The project uses the Windows Winsock2 library for socket communication.

No external cryptography library is required.

The encryption algorithm is implemented manually in C.


# Project Folder

Each team member keeps a local copy of the project on their own laptop.

Example:

    C:\Users\KIIT\Desktop\SecureChat

The folder contains:

    SecureChat
    |
    |-- server.c
    |-- client.c
    |-- crypto.c
    |-- crypto.h
    |-- README.md
    |
    |-- server.exe
    |-- client.exe
    
# Opening the Project

The project can be opened using Visual Studio Code.

Open the SecureChat folder in VS Code.

Then open:

    Terminal → New Terminal

The VS Code terminal can be:

- PowerShell
- Command Prompt
- Git Bash

A separate Windows PowerShell window is not required.

All compilation and execution commands can be performed directly inside the VS Code terminal.


# GitHub Setup

The project was shared through GitHub.

Each team member first clones the repository.

Example:

    git clone https://github.com/Blurry-123/SecureChat.git

Then enter the project folder:

    cd SecureChat

After making changes, the changes can be checked using:

    git status

Changes can be added using:

    git add .

A commit can then be created:

    git commit -m "Description of changes"

Finally, the changes can be uploaded:

    git push

When another member has already pushed changes, the latest version can be obtained using:

    git pull

This allows all three members to keep their local SecureChat folders synchronized.


# Important Git Collaboration Rule

The same project folder should be used after cloning the repository.

A second unrelated SecureChat folder should not be created for the same project.

Each team member has their own local copy of the repository on their own laptop.

For example:

    Laptop 1
        C:\...\SecureChat

    Laptop 2
        C:\...\SecureChat

    Laptop 3
        C:\...\SecureChat

These are separate local folders, but they all contain the same project obtained from the GitHub repository.


# Checking GCC Installation

Open a VS Code terminal and run:

    gcc --version

If GCC is installed correctly, its version information will be displayed.

For example:

    gcc.exe (MinGW.org GCC-6.3.0-1)


# Compiling the Server

The server and encryption source files are compiled separately.

For the current project, the server is compiled using:

    gcc server.c -o server.exe -lws2_32

If compilation succeeds, no error message should appear.

The following executable will be created:

    server.exe


# Compiling the Client

The client uses both `client.c` and `crypto.c`.

Compile it using:

    gcc client.c crypto.c -o client.exe -lws2_32

If compilation succeeds, the following executable will be created:

    client.exe


# Important Compilation Note

The encryption function `xor_crypt()` is implemented in `crypto.c`.

Therefore, `server.c` should not contain another definition of the same function when compiling the server together with `crypto.c`.

If the same function is defined in both files and both are compiled together, GCC produces an error similar to:

    multiple definition of `xor_crypt`

The final project should contain only one implementation of the encryption function.


# Starting the Server

One computer acts as the server computer.

Open the SecureChat folder in VS Code and open a terminal.

Compile:

    gcc server.c -o server.exe -lws2_32

Then start the server on port 8080:

    .\server.exe 8080

The server displays:

    ==============================
           Secure Chat Server
    ==============================
    Server running on port 8080
    Waiting for clients...


The server terminal must remain open.

Do not close the server terminal while clients are connected.


# Finding the Server IP Address

Because the three computers are connected to the same Wi-Fi network, the server computer's local IPv4 address can be used.

On the server computer, open a terminal and run:

    ipconfig

Find the network adapter currently connected to the Wi-Fi.

Look for:

    IPv4 Address

For example:

    IPv4 Address. . . . . . . . . . . : 192.168.1.105

The actual IP address will be different depending on the network.

The clients must use the server computer's IPv4 address.


# Same Wi-Fi Integration

The final demonstration uses three different laptops connected to the same Wi-Fi network.

The architecture is:

    Laptop 1
    Server
    IP: 192.168.x.x
         |
         |
         | Wi-Fi / TCP
         |
    +----+---------------------+
    |                          |
    v                          v
    Laptop 2                  Laptop 3
    Client                    Client
    Monk                      Alice


The server runs only on one laptop.

The other two laptops run the client.

All three computers must be connected to the same Wi-Fi network.


# Three-Laptop Setup

## Laptop 1 — Server

Laptop 1 runs:

    server.exe

Start it using:

    .\server.exe 8080

Find the IPv4 address using:

    ipconfig

For example:

    192.168.1.105


## Laptop 2 — Client 1

Laptop 2 compiles the client:

    gcc client.c crypto.c -o client.exe -lws2_32

Then connects to Laptop 1 using the server's IP address:

    .\client.exe 192.168.1.105 8080

Replace `192.168.1.105` with the actual server IP.


## Laptop 3 — Client 2

Laptop 3 also compiles the client:

    gcc client.c crypto.c -o client.exe -lws2_32

Then connects using:

    .\client.exe 192.168.1.105 8080

Again, replace the example IP address with the actual server IP.


# Three-User Demonstration

For example:

    Laptop 1:
        Server

    Laptop 2:
        Username: sumo
        Key: sumo123

    Laptop 3:
        Username: monk
        Key: monk123

Both clients connect to the same server.

The server maintains both users simultaneously.

A third client can also connect from another laptop:

    Username: alice
    Key: alice123


# Client Registration

When a client starts, it asks for:

    Username:
    Key:

For example:

    Username: sumo
    Key: sumo123

The server registers the user and responds:

    server$ OK REGISTERED sumo

Another client can register:

    Username: monk
    Key: monk123

The response is:

    server$ OK REGISTERED monk


# Unique Usernames

Every connected client must have a unique username.

For example:

    sumo
    monk
    alice

are valid simultaneously.

If another client tries to register as `sumo` while `sumo` is already connected, the server returns:

    ERROR username already exists

The server continues running after rejecting the duplicate username.


# Chat Commands

The available commands are:

    SEND TO <user> <message>

    SENDFILE TO <user> <filename>

    LIST

    QUIT


# Sending Messages

Suppose the connected users are:

    sumo
    monk
    alice

Sumo can send a message to Monk using:

    SEND TO monk Hello Monk!

The server receives the message, processes the destination username, and routes it to Monk.

Monk receives:

    FROM sumo: Hello Monk!


Monk can then reply:

    SEND TO sumo Hello Sumo!

Sumo receives:

    FROM monk: Hello Sumo!


# Important Command Format

The destination username must be entered immediately after:

    SEND TO

Correct:

    SEND TO monk Hello Sumo!

Incorrect:

    SEND TO Hello Sumo!

The second command attempts to send a message to a user named `Hello`, which normally does not exist.

This results in:

    ERROR user is not online


# Simultaneous Communication

The server supports multiple connected clients.

For example:

    Sumo  → Monk
    Monk  → Sumo
    Alice → Monk
    Monk  → Alice
    Sumo  → Alice

All three users can remain connected simultaneously.

The server creates a separate client-handling thread for each connected client.

Therefore, one connected user does not have to disconnect before another user can communicate.


# Online User List

The command:

    LIST

shows the users currently connected to the server.

For example:

    ONLINE sumo, monk, alice


# Encryption

The project uses a Repeating-Key XOR cipher.

The encryption operation is:

    Ciphertext = Plaintext XOR Key

The same operation is used to decrypt:

    Plaintext = Ciphertext XOR Key


For example, if a user uses:

    Key: sumo123

the key is repeatedly applied to the message bytes.

The encryption implementation is contained in:

    crypto.c

The function declaration is provided through:

    crypto.h


# Why XOR Was Used

Repeating-Key XOR was selected because:

1. It is simple to understand.
2. It can be implemented manually in C.
3. It does not require an external cryptography library.
4. Encryption and decryption use the same operation.
5. It demonstrates the concept of symmetric-key encryption.


# Important Security Limitation

Repeating-Key XOR is not considered secure modern cryptography.

It is used in this project for educational demonstration.

Because the key is repeated, an attacker with enough ciphertext may be able to discover patterns.

A production secure chat application should use a modern authenticated encryption algorithm such as AES-GCM or ChaCha20-Poly1305 and a proper key-exchange mechanism.


# Hop-by-Hop Encryption

The project uses hop-by-hop encryption.

The communication process is:

    Sumo Client
         |
         | Encrypted using Sumo's key
         v
       Server
         |
         | Decrypt using Sumo's key
         |
         | Route message
         |
         | Encrypt using Monk's key
         v
    Monk Client


Therefore, the server temporarily has access to the plaintext message.

This is different from true end-to-end encryption.

The design is intentionally simplified for demonstrating socket programming and symmetric-key encryption.


# File Transfer

The application supports text-file transfer.

Only `.txt` files are accepted.

Example:

    SENDFILE TO monk test_files\notes.txt


The client checks:

1. Whether the file exists.
2. Whether the file is a `.txt` file.
3. Whether the file is within the allowed size.
4. Whether the file can be read.

If all checks succeed, the file is encrypted and sent to the server.


# Receiving a File

The receiving client decrypts the transferred file and saves it with a `received_` prefix.

For example:

    notes.txt

is saved as:

    received_notes.txt


This prevents the original file from being overwritten.


# Relative File Path Test

A relative path can be used when the file exists relative to the current working directory.

Example:

    SENDFILE TO monk test_files\relative_test.txt

If the file is found and valid, it is transferred.

The received file can then be compared with the original.


# Full File Path Test

A full Windows path can also be used.

Example:

    SENDFILE TO monk C:\Users\KIIT\Documents\notes.txt

The client checks the specified location and transfers the file if it exists and satisfies the requirements.


# File Comparison Test

After receiving a file, the original and received files can be compared.

PowerShell uses `fc` as an alias for `Format-Custom`, so the following command should not be used directly in PowerShell:

    fc /b test_files\notes.txt received_notes.txt

Instead, use the Windows executable explicitly:

    fc.exe /b test_files\notes.txt received_notes.txt

If the files are identical, the comparison should indicate that no differences were found.

This was used to verify that transferred files were byte-identical to the originals.


# File Validation Tests

The following file-transfer tests were successfully performed.


## Test — Missing File

Command:

    SENDFILE TO monk missing.txt

Expected result:

    ERROR file not found: missing.txt


## Test — Unsupported File Type

Command:

    SENDFILE TO monk test_files\report.pdf

Expected result:

    ERROR only .txt files are supported


## Test — Oversized File

Command:

    SENDFILE TO monk test_files\large.txt

Expected result:

    ERROR file too large


These tests confirm that invalid file requests are rejected without terminating the server.


# Message Testing

Basic message communication was tested between users.

Example:

    Sumo → Monk

    SEND TO monk Hello Monk!


Monk receives:

    FROM sumo: Hello Monk!


The reverse direction was also tested:

    Monk → Sumo

    SEND TO sumo Hello Sumo!


Sumo receives:

    FROM monk: Hello Sumo!


Both directions were successfully tested.


# Multiple Client Testing

Three clients can be connected at the same time.

Example:

    Sumo
    Monk
    Alice


The following communication can occur:

    Sumo  → Monk
    Monk  → Sumo
    Alice → Monk
    Monk  → Alice
    Sumo  → Alice


The server continues running while all clients communicate.


# Error Handling

The application handles invalid commands without terminating the server.


## Unknown Command

Example:

    BLAHBLAH

Expected response:

    ERROR unknown command


## Offline User

Example:

    SEND TO ghost Hello

Expected response:

    ERROR user is not online


## Duplicate Username

Expected response:

    ERROR username already exists


## Unsupported File

Expected response:

    ERROR only .txt files are supported


## Oversized File

Expected response:

    ERROR file too large


## Missing File

Expected response:

    ERROR file not found


# Client Disconnection

A client can disconnect using:

    QUIT

The server responds:

    GOODBYE <username>

The server then removes the client from its active-client table.

Other connected users can continue communicating.


# Abrupt Disconnection

A client can also be terminated using:

    Ctrl+C

The server detects the lost connection and removes the client.

Other connected clients remain connected and can continue using the chat application.


# Network Requirements for Three-Laptop Demonstration

For the final demonstration using three different laptops:

1. All three laptops should be connected to the same Wi-Fi network.
2. One laptop runs the server.
3. The other two laptops run clients.
4. The server laptop's IPv4 address must be obtained using `ipconfig`.
5. Both clients must connect to that IP address.
6. Port `8080` must be available for the server.
7. The server program must remain running.

The connection looks like:

                    Wi-Fi Network
                         |
          +--------------+--------------+
          |              |              |
          v              v              v
       Laptop 1       Laptop 2       Laptop 3
       SERVER           SUMO           MONK
       :8080             |              |
          |               \            /
          |                \          /
          +-----------------SERVER---+
                    TCP connections


# Windows Firewall

If clients cannot connect to the server even though all computers are on the same Wi-Fi network, Windows Firewall may be blocking the server.

When Windows asks whether to allow the application through the firewall, allow access on the appropriate private network.

The server must be reachable from the client laptops over the local network.


# Complete Three-Laptop Execution

## Laptop 1 — Server

Open VS Code.

Open the SecureChat folder.

Open a terminal.

Compile:

    gcc server.c -o server.exe -lws2_32

Run:

    .\server.exe 8080

Find the IP:

    ipconfig

Example:

    IPv4 Address: 192.168.1.105

Keep this terminal open.


## Laptop 2 — Sumo

Open the SecureChat project.

Compile:

    gcc client.c crypto.c -o client.exe -lws2_32

Connect:

    .\client.exe 192.168.1.105 8080

Register:

    Username: sumo
    Key: sumo123


## Laptop 3 — Monk

Open the SecureChat project.

Compile:

    gcc client.c crypto.c -o client.exe -lws2_32

Connect:

    .\client.exe 192.168.1.105 8080

Register:

    Username: monk
    Key: monk123


# Final Demonstration

Once all three computers are connected:

    Sumo
    Monk
    Alice

Use:

    LIST

to confirm that all users are online.

Then demonstrate communication.

For example, from Sumo:

    SEND TO monk Hello Monk!


From Monk:

    SEND TO sumo Hello Sumo!


From Alice:

    SEND TO monk Hello Monk!


Then demonstrate communication in the opposite directions.

This proves that the server can maintain multiple TCP connections and route messages between different users.


# Testing Summary

The following functionality was tested:

    ✓ Server compilation
    ✓ Client compilation
    ✓ Client registration
    ✓ Unique username validation
    ✓ Two-way messaging
    ✓ Multiple connected clients
    ✓ Online user listing
    ✓ Unknown command handling
    ✓ Offline user handling
    ✓ Duplicate username handling
    ✓ Client disconnection
    ✓ Text file transfer
    ✓ Relative file path
    ✓ Full file path
    ✓ File comparison
    ✓ Missing file handling
    ✓ Unsupported file type handling
    ✓ Oversized file handling
    ✓ Encryption/decryption
    ✓ Multiple-client communication


# Known Limitations

1. Repeating-Key XOR is not suitable for real-world secure communication.

2. The encryption is hop-by-hop rather than true end-to-end encryption.

3. The server temporarily has access to plaintext messages.

4. Only `.txt` files are supported.

5. File transfers have a maximum size limit.

6. File contents are held in memory during transfer.

7. The application is designed primarily for Windows.

8. The final multi-computer demonstration requires the computers to be reachable over the same network.

9. GitHub is used for source-code synchronization, but the chat application itself communicates directly using TCP sockets and does not require GitHub during execution.


# Dependencies

The project uses:

- C
- MinGW GCC
- Visual Studio Code
- Git
- Windows Winsock2
- TCP sockets

The following library is required during compilation:

    ws2_32

No external cryptography library is required.


# Final Project Architecture

The complete system consists of three main components:

    client.c
        |
        | Uses encryption functions
        v
    crypto.c
        |
        v
    Server
        |
        | Routes messages/files
        v
    Other Clients


For three simultaneous users:

                       +----------------+
                       |     SERVER     |
                       |   Port 8080    |
                       +--------+-------+
                                |
              +-----------------+-----------------+
              |                 |                 |
              v                 v                 v
        +-----------+     +-----------+     +-----------+
        |   SUMO    |     |   MONK    |     |   ALICE   |
        |  CLIENT   |     |  CLIENT   |     |  CLIENT   |
        +-----------+     +-----------+     +-----------+


# Summary

SecureChat is a multi-client TCP chat application written in C.

The project demonstrates:

- TCP socket programming
- Client-server architecture
- Multiple simultaneous clients
- Concurrent client handling
- User registration
- Username management
- Symmetric-key encryption
- Repeating-Key XOR encryption
- Message routing
- Text-file transfer
- File validation
- File size limitation
- Error handling
- Client disconnection
- Online user listing
- GitHub-based team collaboration
- Testing across multiple laptops on the same Wi-Fi network

For the final demonstration, one laptop runs the server and two or more other laptops run clients.

All clients connect to the server using the server laptop's local IPv4 address and port 8080.

Example:

    Server:
        .\server.exe 8080

    Client 1:
        .\client.exe 192.168.1.105 8080

    Client 2:
        .\client.exe 192.168.1.105 8080

Once connected, multiple users can communicate simultaneously through the same server.
