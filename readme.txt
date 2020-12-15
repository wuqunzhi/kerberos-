在四个终端：
注意链接库 -lws2_32
gcc AS.c -o AS -lws2_32; ./AS
gcc TGS.c -o TGS -lws2_32; ./TGS
gcc V.c -o V -lws2_32; ./V
gcc C.c -o C -lws2_32; ./C

或者
gcc AS.c -o AS -lws2_32 && AS.exe
gcc TGS.c -o TGS -lws2_32 && TGS.exe
gcc V.c -o V -lws2_32 && V.exe
gcc C.c -o C -lws2_32 && C.exe

或者
make
TGS.exe
AS.exe
V.exe
C.exe

不同终端命令可能不同，这里编译和运行写在了同一行，也可全部编译再运行
用户名密码随便输入都可登陆成功

注意数据单位都是8字节
