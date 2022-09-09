---
author: Abdelrahman Farid
email: abdelrahman.farid@gmail.com
date: 2022-09-09
---
# "Smart Wallet System" Assignment
Create a smart wallet system platform in C++ on Linux. Clients should be able to submit their info to the bank system and do transactions on their accounts.

## Build and run
```sh
cmake -Bbuild . -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
./build/bin/Debug/Server-App &
./build/bin/Debug/Client-App &
```

## Assumptions
1. Deposit/Withdrawal/Updating Info are undoable
2. No negative deposit/withdrawal
3. Server persists its data on disk, and loads it on startup
4. Clients have to login using their ID or register a new account
5. Undo/redo works similar to text editors:
	- Undo until the first event is undone
	- Redo until the last event is redone
	- Adding an event in the middle of an undo sequence begins another sequence from that point on

## Functionalities
### Client
1. Fill personal info (name, age, national ID, ...)
2. Deposit money to their account
3. Withdraw money from their account
4. Get the amount of money they have in their account from server
5. Send transaction to server
6. (**BONUS**) Undo/redo a transaction communicated to the server ✅

### Server
1. Receive client info
2. Auto-generate account ID for new clients ($0 initial balance)
3. Save received info
4. Display candidate info
    - Status bar showing client name
    - Multi-line text box
    - Log file of all transactions done
5. Process transaction on client account
6. Send amount of money to client
7. (**BONUS**) Analyze clients info, show statistics and graphs of all accounts ✅
8. (**BONUS**) Undo/redo transaction when requested from client ✅


## Structure
- `/thirdparty` Third-party libraries used
- `/GUI` Windowing and UI module
- `/Smart-Wallet-System` Main library, contains core logic + a TCP module
- `/Client-App` Client application
- `/Server-App` Server application
