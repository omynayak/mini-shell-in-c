# Mini Shell in C
A *lightweight learning-oriented shell that implements core Unix shell behaviour using pure C.*

## Features
- **Command prompt** showing current working directory.
- **Dynamic Input Reader** using `getline()`.
- **Tokenizer/Parser** using `strtok()` (with dynamic input buffer resizing)
- **Built-in Commands**
    - `call` - change directory (`cd`)
    - `help` - show built-ins
    - `gt` - get time: prints formatted current time
    - `filestruct` - visualise current working directory from `/`
    - `exit` -  quit the shell
- **External Command Execution**
Runs any program using `fork()`, `execvp()`, and `waitpid()`
- **Error Handling** for failed commands, missing arguments, invalid programs
- **Clean, modular structure** mimicking how real shells work under the hood.

## Project Structure:
```
mini-shell-in-c
├── main.cpp
├── Makefile
└── README.md
```

## Build Instructions
### Option 1: Compile manually:
```bash
gcc main.c -o mini-shell
./mini-shell
```

### Option 2: Use the Makefile:
```bash
make
./main
```

## How it works:
1. **Read**
The shell displays the current working directory as a prompt, then reads a full line of user input using `getline()`, which safely handles dynamic input sizes.

2. **Parse**
The input line is split into tokens using a custom parser based on strtok().
This produces an argument list (args[]) that the shell can interpret as either:
- **a builtin command**, or
- **an external program** to execute.

3. **Execute**
- If the first token matches a builtin, the corresponding function is called through a function-pointer table.

- Otherwise, the shell creates a child process using `fork()`, replaces it with the target program using `execvp()`, and the parent waits with `waitpid()` until the command finishes.

## Contributing
Contributions, feedback, and suggestions are welcome! <br>
If you’d like to improve performance, precision, or add benchmarks, feel free to open an issue or PR.
