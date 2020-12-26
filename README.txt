

before run please follow bellow steps exactly!!!!
1- go to entryDir:
entryDIR conatain src for entry.c  (entry proccess)
compilation:
cc -c *.c *.h -l
*.o -oentry -lm
2- go to recptionDir:
recptionDir contain src for recption.c (recption process)
compilation:
cc recption.c -orecption
3- go to serviceDir
serviceDIR  contain src for service.c (service proccess)
compilation:
cc -c *.c *.h -lm
*.o -oservice -lm
4- to run the program:
main process: pelecom.c
compilation:
cc pelecom.c -opelecom

to start the program use the command: ./pelegram 													
:)
