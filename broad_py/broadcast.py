import socket, traceback
from time import sleep
from contextlib import closing


host = ''   # Bind to all interfaces

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
s.settimeout(10)
s.bind((host,0))

s.sendto("where is raspin?",("255.255.255.255",8000))
try:
 with closing(s):
    #s.bind((host, port))
    mess = ""
    while mess <> "i'm raspin":
     mess,addr = s.recvfrom(1024)
    print(addr)
    print (mess)

except Exception as e:
    #print e.args[1].decode("sjis")
    raise e
    pass


