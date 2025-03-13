# BrainDamage
## A BrainF*ck interpreter written in C

Visit [brainfuck.org](https://brainfuck.org) for cool stuff and language refference

### Command line parameters
```
BrainDamage [inputFile] -d
    -d - Enable the Debug Mode
```

### Limitations of this interpreter
- Program Code size up to 65536 symbols (Non-instruction symbols also count, including NewLines)
- 65536 Memory Cells (each cell `uint8_t` is type)
- Execution up to 256 nested loops

### Extended instruction set
`#` - display the value of the currently selected cell
