total:
	gcc AS.c -o AS -lws2_32 
	gcc TGS.c -o TGS -lws2_32
	gcc V.c -o V -lws2_32
	gcc C.c -o C -lws2_32
	echo ��Ϊ�����ڲ�ͬ�ն˲���ִ�������������ֶ�ִ��AS.exe,TGS.exe,V.exe,C.exe
clean:
	del /s *.exe
	