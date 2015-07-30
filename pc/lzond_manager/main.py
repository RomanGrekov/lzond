#Python based serial port device manager
#Roman Grekov 2015

import time
import threading
from Tkinter import *
import tkMessageBox
import sys
from Queue import Queue
from serial_api import MySerial

def open_port_handler(ser):
    data = ser.get_serials()
    if data:
        items = serialsbox.curselection()
        items = [data[int(item)] for item in items]
        if items:
            if not ser.open_port(items[0]):
                tkMessageBox.showinfo("Error!", "Can't open port %s" % items[0])
        else:
            tkMessageBox.showinfo("Error!", "Can't define selected port")
    else:
        tkMessageBox.showinfo("Error!", "Can't get ports list")

    print items

def close_port_handler(ser, stdout_proc):
    try:
        ser.close_port()
        stdout_proc.stop_stdout()
    except:
        pass

def refresh_ports(ser):
    serialsbox.delete(0, END)
    for s in ser.get_serials():
        serialsbox.insert(END, s)
        
        
def send_params(ser, param, val):
    ser.send_cmd(param, val)

def quit(ser, stdout_proc):
    close_port_handler(ser, stdout_proc)
    sys.exit(0)

def log(label, cmd):
    tkMessageBox.showinfo(label, cmd)
    
class ShowUsartData():
    def __init__(self, w, serial, queue):
        self.w = w
        self.ser = serial
        self.q = queue

    def stdout_show(self):
        while 1:
            self.w.insert(END, self.q.get(block=True))
            self.w.see(END)
            #time.sleep(1)

    def start_stdout(self):
        self.ser.start_receiving()
        self.thread = threading.Thread(target=self.stdout_show)
        self.thread.start()
        #thread.join()

    def stop_stdout(self):
        self.thread._Thread__stop()

def start_stdout_handler(stdout_processor):
    stdout_processor.start_stdout()

if __name__ == "__main__":
    q = Queue(100)
    ser = MySerial(log, q)

    params = {"v_def":{}, "v_outref":{}, "start_pause":{}, "start_timout":{}}

    root = Tk()
    #root.attributes('-fullscreen', True)
    mainFrame = Frame(root, height = 500, width = 640, bd=1)
    mainFrame.pack()
    panelFrame = Frame(mainFrame, bd=2, relief=SUNKEN)
    panelFrame.pack(expand=1, fill = 'x', pady=10, padx=5)

    serialsbox = Listbox(panelFrame, selectmode=SINGLE)
    serialsbox.pack(side = "left", padx=5)

    refreshBtn = Button(panelFrame, text = 'Refresh ports', command=lambda: refresh_ports(ser))
    openBtn = Button(panelFrame, text = 'Open port', command=lambda: open_port_handler(ser))

    stdoutFrame = Frame(mainFrame, bd=2, relief=SUNKEN)
    stdoutFrame.pack(expand=1, fill = 'x', pady=10, padx=5)

    text_stdout=Text(stdoutFrame,height=15,width=100,font='Arial 14',wrap=WORD)
    text_stdout.pack()
    stdout_processor = ShowUsartData(text_stdout, ser, q)
    stdoutBtn = Button(panelFrame, text = 'Start receiver', command=lambda: start_stdout_handler(stdout_processor))
    quitBtn = Button(panelFrame, text = 'Quit', command=lambda: quit(ser, stdout_processor))

    closeBtn = Button(panelFrame, text = 'Close port', command=lambda: close_port_handler(ser, stdout_processor))

    refreshBtn.pack(side = "left", padx=5)
    openBtn.pack(side = "left", padx=5)
    closeBtn.pack(side = "left", padx=5)
    stdoutBtn.pack(side = "left", padx=5)
    quitBtn.pack(side = "left", padx=5)

    commandsFrame = Frame(mainFrame, bd=2, relief=SUNKEN)
    commandsFrame.pack(expand=1, fill = 'x', pady=10, padx=5)

    for param in params.keys():
        print param
        params[param]["frame"] = Frame(mainFrame, bd=2, relief=SUNKEN)
        params[param]["frame"].pack(expand=1, fill = 'x', pady=10, padx=5)
        params[param]["label"]= Label(params[param]["frame"], text=param, width=20)
        params[param]["label"].pack(side="left", padx=5)
        params[param]["text"]= Text(params[param]["frame"], height=1, width=10, font='Arial 14', wrap=WORD)
        params[param]["text"].pack(side="left", padx=5)
        params[param]["btn"]= Button(params[param]["frame"], text = 'Send %s' % param, command=lambda: send_params(ser, param, params[param]["text"].get("1.0", END)))
        params[param]["btn"].pack(side="left", padx=5)

    t_btn = Button(mainFrame, text = 'SAVE!!!', command=lambda: send_params(ser, "save", "1"))
    t_btn.pack(side="left", padx=5)

    refresh_ports(ser)

    root.mainloop()
    #main()
