// Bluegiga ANSI C BGLib UART interface header file
// BLE SDK v1.2.1-91
//
// Contact: support@bluegiga.com
//
// This is free software distributed under the terms of the MIT license reproduced below.
//
// Copyright (c) 2013 Bluegiga Technologies
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C"{
#endif

void uart_list_devices();
int uart_find_serialport(char *name);
int uart_open(char *port);
void uart_close();
int uart_tx(int len, unsigned char *data);
int uart_rx(int len, unsigned char *data, int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif // UART_H
