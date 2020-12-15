total:
	gcc AS.c -o AS -lws2_32 
	gcc TGS.c -o TGS -lws2_32
	gcc V.c -o V -lws2_32
	gcc C.c -o C -lws2_32
	echo 因为不会在不同终端并行执行命令的命令，请手动执行AS.exe,TGS.exe,V.exe,C.exe
clean:
	del /s *.exe
	