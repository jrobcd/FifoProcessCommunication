# FifoProcessCommunication

## Description

This project demonstrates inter-process communication (IPC) between two processes (Writer and Reader) using a named FIFO (First In, First Out) special file. The Writer process receives text input from the user and signals, and writes them to the FIFO. The Reader process reads from the FIFO and logs the received text and signals into separate files.

## Features

- **Writer Process:**
  - Prints its own PID.
  - Reads text input from the user and writes it to the FIFO.
  - Handles `SIGUSR1` and `SIGUSR2` signals, writing corresponding messages to the FIFO.

- **Reader Process:**
  - Reads from the FIFO.
  - Logs text data into `log.txt`.
  - Logs signal data into `signals.txt`.

## Requirements

- Linux operating system (for FIFO and signal handling).
- C compiler (e.g., `gcc`).

## Usage

1. **Run the Reader process in one terminal:**
    ```sh
    ./reader
    ```

2. **Run the Writer process in another terminal:**
    ```sh
    ./writer
    ```

3. **Send signals to the Writer process from a third terminal:**
    - Find the PID of the Writer process (displayed when you run it).
    - Send `SIGUSR1`:
        ```sh
        kill -SIGUSR1 <PID_Writer>
        ```
    - Send `SIGUSR2`:
        ```sh
        kill -SIGUSR2 <PID_Writer>
        ```

## Error Handling

Both Writer and Reader processes include basic error handling. If any exceptional case occurs (e.g., unable to open FIFO, write errors), the program will print an error message and terminate.



