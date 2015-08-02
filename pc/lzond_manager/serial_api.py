import threading
import sys
import serial
import glob

class MySerial():
    def __init__(self, log, queue):
        self.log = log
        self.ser = None
        self.timer_flag = False
        self.data = []
        self.data_size = 50
        self.d_start = 0
        self.d_end = self.data_size - 1
        self.data_updated = False
        self.q = queue

    def get_serials(self):
        """Lists serial ports

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of available serial ports
        """
        if sys.platform.startswith('win'):
            ports = ['COM' + str(i + 1) for i in range(256)]

        elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
            # this is to exclude your current terminal "/dev/tty"
            ports = glob.glob('/dev/tty[A-Za-z]*')

        elif sys.platform.startswith('darwin'):
            ports = glob.glob('/dev/tty.*')

        else:
            raise EnvironmentError('Unsupported platform')

        result = []
        for port in ports:
            try:
                s = serial.Serial(port)
                s.close()
                result.append(port)
            except (OSError, serial.SerialException):
                pass
        return result

    def open_port(self, s_port):
        self.ser = serial.Serial()
        self.ser.port = s_port
        self.ser.baudrate = 9600
        self.ser.bytesize = serial.EIGHTBITS #number of bits per bytes
        self.ser.parity = serial.PARITY_NONE #set parity check: no parity
        self.ser.stopbits = serial.STOPBITS_ONE #number of stop bits
        #ser.timeout = None          #block read
        self.ser.timeout = 1            #non-block read
        #ser.timeout = 2              #timeout block read
        self.ser.xonxoff = False     #disable software flow control
        self.ser.rtscts = False     #disable hardware (RTS/CTS) flow control
        self.ser.dsrdtr = False       #disable hardware (DSR/DTR) flow control
        self.ser.writeTimeout = 2     #timeout for write

        try:
            self.ser.open()
        except Exception, err:
            print "Serial port open error: %s" % err
            return False

        if self.ser.isOpen():
            return True

    def close_port(self):
        self.thread._Thread__stop()
        self.ser.close()

    def get_data(self):
        if self.data_updated:
            self.data_updated = False
            return self.data
        return None

    def receive_data(self):
        while 1:
            if self.ser.inWaiting() > 0:
                try:
                    got_byte = self.ser.read(1)
                    self.q.put(got_byte, block=True)
                except Exception, err:
                    self.log("Error!", "Can't push to queue: %s" % str(err))

    def start_receiving(self):
        try:
            self.thread = threading.Thread(target = self.receive_data)
            self.thread.start()
            #self.thread.join()
        except Exception, err:
            self.log("Error!", "Can't start data receiving: %s" % str(err))

    def send_data(self, data):
        self.ser.flushInput() #flush input buffer, discarding all its contents
        self.ser.flushOutput()#flush output buffer, aborting current output and discard all that is in buffer
        self.ser.write(data)

    def send_cmd(self, cmd, val):
        self.send_data("CMD_%s=%s\r" % (cmd, str(val)))

    def __del__(self):
        self.ser.close_port()


