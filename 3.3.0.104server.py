import socket
import  threading
import struct
import json
HOST='127.0.0.1'
#大小端监测
#bool isLittleEndian()
#{
#    short a=0x0061;
#    if((char)a=='a') return true;
#    else return false;
#}
PORT=8989

# D:\code\python\server.py
sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)      #定义socket类型，网络通信，TCP

sock.bind((HOST,PORT))       #要连接的IP与端口

def send():
    while 1:
        msg = input() #获取输入
        data=msg.encode("utf16") #转码
        len_head=len(data[2:]) #从第三个元素开始计算长度
        print(struct.pack(">I",len_head))#以大端模式转换成unsigned int
        client.sendall(struct.pack(">I",len_head)+data[2:]) #发送数据
        print("编码后: ",msg.encode("utf16"))

def recv():
    while 1:
        try:
            # "hhaha111pythonهه٥11111122   22"
            recv_data = ""
            data_len = client.recv(4)  # 把接收的数据定义为变量

            if(len(data_len)<=0):
                continue
            else:
                print(data_len)
                data_len=struct.unpack('>I', data_len)[0]
                recv_data= client.recv(data_len)
                # tmp=tmp.decode("utf16","ignore")

                print(recv_data.decode("utf16","ignore"))
        except socket.error:
            print("断开连接")
            break
while(True):
    sock.listen(5)
    client, addr = sock.accept()
    print("连接成功")
    th1 = threading.Thread(target=send)
    th2 = threading.Thread(target=recv)
    th1.start()
    th2.start()
    th1.join()
    th2.join()

sock.close()   #关闭连接



