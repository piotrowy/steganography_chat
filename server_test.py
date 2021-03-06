#!/usr/bin/env python
# -*- coding: utf-8 -*-
import socket
__author__ = 'piotrowy'
HOST = '192.168.1.30'
PORT_S = 5000
PORT_R = 5001


def main():
    for i in range(0, 150):
        message = 'test' + '-'*50 + str(i)
        print('[send_to_server] message: ' + message)
        sck = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            sck.connect((HOST, PORT_S))
            print('[send_to_server]: Connection established.\n')
        except socket.error:
            print('[send_to_server]: Connection can\'t be established.\n')
            return
        sck.send(message.encode('utf-8'))
        sck.close()

    sck = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sck.connect((HOST, PORT_R))
    except socket.error:
        print('[SOCKETS.PY][receive_from_server]: Connection can\'t be established')

    data_str = ''
    data = sck.recv(256)
    while data.decode('utf-8') != '':
        data_str += data.decode('utf-8')
        data = sck.recv(256)
    sck.close()
    print(data_str)


if __name__ == '__main__':
    main()
