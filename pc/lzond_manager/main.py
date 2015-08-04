#Python based serial port device manager
#Roman Grekov 2015

import os
import threading
from Tkinter import *
import tkMessageBox
import sys
from Queue import Queue
from serial_api import MySerial
from parameters import params
import wx

def open_port_handler(ser, stdout_processor):
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

    stdout_processor.start_stdout()
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
    print param
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

class Window(wx.Frame):

    def __init__(self, serial, parent, title, size):
        mainframe = wx.Frame.__init__(self, parent, title=title, size=size)
        self.ser = serial

        bar = wx.MenuBar()
        file_menu = wx.Menu()
        menu_about = file_menu.Append(wx.ID_ABOUT, "About", "Push the button to get an information about this application")
        menu_exit = file_menu.Append(wx.ID_EXIT, "Exit", "Push the button to leave this application")
        port_menu = wx.Menu()
        port_menu.Append(wx.ID_APPLY, "Refresh", "Refresh ports")
        port_menu.AppendSeparator()
        bar.Append(file_menu, "File")
        bar.Append(port_menu, "Port")
        self.SetMenuBar(bar)

        ports = self.ser.get_serials()
        ports_listbox = wx.ListBox(parent=self, id=-1, choices=ports, style=wx.LB_NEEDED_SB, name="Ports")

        self.Bind(wx.EVT_MENU, self.OnAbout, menu_about)
        self.Bind(wx.EVT_MENU, self.OnExit, menu_exit)

        self.Show()

    def OnAbout(self, event):
        # A message dialog box with an OK button. wx.OK is a standard ID in wxWidgets.
        dlg = wx.MessageDialog( self, "Lambda zond manager", "Info",  wx.OK)
        dlg.ShowModal() # Show it
        dlg.Destroy() # finally destroy it when finished.

    def OnExit(self, e):
        self.Close(True)  # Close the frame.

if __name__ == "__main__":
    q = Queue(100)
    ser = MySerial(log, q)


    app = wx.App()
    wnd = Window(ser, None, "Lambda zond emulator manager v1.0", size=(1024,768))
    wnd.Show(True)
    app.MainLoop()
    """
    root = Tk()
    root.title("Lambda zond emulator manager v1.0")
    if sys.platform.startswith('win'):
        root.iconbitmap('lambda.ico')
    else:
        img = PhotoImage(file='lambda.png')
        root.tk.call('wm', 'iconphoto', root._w, img)
    #root.attributes('-fullscreen', True)

    scrollbar = Scrollbar(root, orient=VERTICAL).grid(row=0, column=1)
    serialsbox = Listbox(root, selectmode=SINGLE, height=3, yscrollcommand=scrollbar.set).grid(row=0, column=0)
    scrollbar.config(command=serialsbox.yview)

    mainFrame = Frame(root, height = 400, width = 640, bd=1)
    mainFrame.pack()

    panelFrame = Frame(mainFrame, bd=2, relief=SUNKEN)
    panelFrame.pack(expand=1, fill = 'x', pady=10, padx=5)

    scrollbar = Scrollbar(panelFrame, orient=VERTICAL)
    serialsbox = Listbox(panelFrame, selectmode=SINGLE, height=3, yscrollcommand=scrollbar.set)
    scrollbar.config(command=serialsbox.yview)
    scrollbar.pack(side=RIGHT, fill=Y)
    serialsbox.pack(side = "left", padx=5, fill=BOTH, expand=1)

    refreshBtn = Button(panelFrame, text = 'Refresh ports', command=lambda: refresh_ports(ser))
    openBtn = Button(panelFrame, text = 'Open port', command=lambda: open_port_handler(ser, stdout_processor))

    stdoutFrame = Frame(mainFrame, bd=2, relief=SUNKEN)
    stdoutFrame.pack(side="left", expand=1, fill = 'x', pady=10, padx=5)

    scrollbar1 = Scrollbar(stdoutFrame, orient=VERTICAL)
    text_stdout=Text(stdoutFrame,height=30,width=50,font='Arial 14',wrap=WORD, yscrollcommand=scrollbar1.set)
    scrollbar1.config(command=text_stdout.yview)
    scrollbar1.pack(side=RIGHT, fill=Y)
    text_stdout.pack(fill=BOTH, expand=1)
    stdout_processor = ShowUsartData(text_stdout, ser, q)
    quitBtn = Button(panelFrame, text = 'Quit', command=lambda: quit(ser, stdout_processor))

    closeBtn = Button(panelFrame, text = 'Close port', command=lambda: close_port_handler(ser, stdout_processor))

    refreshBtn.pack(side = "left", padx=5)
    openBtn.pack(side = "left", padx=5)
    closeBtn.pack(side = "left", padx=5)
    quitBtn.pack(side = "left", padx=5)

    commandsFrame = Frame(mainFrame, bd=2, relief=SUNKEN)
    commandsFrame.pack(expand=1, fill = 'x', pady=10, padx=5)

    class BtnCall():
        def __init__(self, name, params):
            self.name = name
            self.params = params
        def send(self):
            send_params(ser, self.name, self.params[self.name]["text"].get("1.0", END))

    paramsFrame = Frame(mainFrame, bd=2, relief=SUNKEN)
    scrollbar2 = Scrollbar(paramsFrame, orient=VERTICAL)
    #scrollbar2.config(command=paramsFrame.yview)
    scrollbar2.pack(side=RIGHT, fill=Y)
    paramsFrame.pack(expand=1, fill = 'x', pady=10, padx=5)
    for param in params.keys():
        params[param]["frame"] = Frame(paramsFrame, bd=2, relief=SUNKEN)
        params[param]["frame"].pack(expand=1, fill = 'x', pady=1, padx=5)
        params[param]["label"]= Label(params[param]["frame"], text=param, width=20)
        params[param]["label"].pack(side="left", padx=5)
        params[param]["text"]= Text(params[param]["frame"], height=1, width=10, font='Arial 14', wrap=WORD)
        params[param]["text"].pack(side="left", padx=5)
        params[param]["btn"]= Button(params[param]["frame"], text = 'Send', command=BtnCall(param, params).send)
        params[param]["btn"].pack(side="left", padx=5)

    t_btn = Button(panelFrame, text = 'SAVE PARAMETERS TO FLASH', command=lambda: send_params(ser, "save", "1"), bd=5, fg="red")
    t_btn.pack(side="right", padx=5)

    designer = Label(root, text="Designed by RomanG", width=50)
    designer.pack(side="bottom", padx=5)

    refresh_ports(ser)
    root.mainloop()

    """
    #main()
