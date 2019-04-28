# INYEONG-shell
2019-1 System Programming(CSE4009) Term Project (Make own shell)
<hr/>

## Compile:
        make
        gcc -o myshell myshell.o
        
## Execute:
        ./myshell

## Note:
        Please choose line 88 or line 89, or None of that.
        
        Mac : line 89 (signal(SIGTSTP, (void *)signal_easter_egg);)
        
        Ubuntu : line 88 (signal(20, (void *)signal_easter_egg);)
