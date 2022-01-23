import serial
import ctypes
import application1

class SerialFrame:
    
    custom_heart_rate, custom_spo2_value = 0, 0 
    custom_tissue_detected = 0
    custom_ir_buffer, custom_red_buffer, custom_green_buffer = [], [], []
    custom_ir_buffer_updated, custom_red_buffer_updated, custom_green_buffer_updated = 0, 0, 0
    custom_heart_rate_spo_updated = 0
    
    max_heart_rate, max_spo2_value = 0, 0 
    max_tissue_detected = 0
    max_ir_buffer, max_red_buffer, max_green_buffer = [], [], []
    max_ir_buffer_updated, max_red_buffer_updated, max_green_buffer_updated = 0, 0, 0
    custom_heart_rate_spo_updated = 0
    deviceDisconnected = False
    error_frame_counter = 0

    def __init__(self, serialCOM, baudRate):
            self.serialHandler = serial.Serial(port=serialCOM, 
                                               baudrate=baudRate, 
                                               parity=serial.PARITY_NONE,
                                               stopbits=serial.STOPBITS_ONE,
                                               bytesize=serial.EIGHTBITS)
            if self.serialHandler.isOpen() is False:
                self.serialHandler.open()
                
            self.frame = []
            
    def checkFrameIntegrity(self):

        if(self.frameBytes[0] != 0xDA) or \
          ((self.frameBytes[len(self.frameBytes) - 1] != 0xDC) and 
           (self.frameBytes[len(self.frameBytes) - 2] != 0xEA)):
              
            return -1
        
        else:
            
            return 0
    
    def close_conection(self):
        self.serialHandler.close()
        
    def flush(self):
        self.serialHandler.flushInput()    
        
    def is_open(self):
        if self.serialHandler.isOpen():
            return True
        else:
            return False
        
    def readFrame(self):
        self.frame = self.serialHandler.readline()
        self.frameBytes = bytes.fromhex(hex(int(self.frame.decode(), 16))[2:])
        
        return self.checkFrameIntegrity()
    
    def sendCommand(self, command):
        frame = [0] * (len(command) + 3)   
        frame[0] = 0xDA
        
        for i in range(1, len(command) + 1):
            frame[i] = command[i-1]
            
        frame[len(command) + 1] = 0xEA
        frame[len(command) + 2] = 0xDC
    
        self.serialHandler.write(frame)
        
    def readAndProcessFrame(self):
        tempBuff = []
        tempBuff2 = []
        try:
            if(self.readFrame() == 0):
                
                self.custom_heart_rate_spo_updated = 0
                self.custom_ir_buffer_updated = 0
                self.custom_red_buffer_updated = 0
                self.custom_green_buffer_updated = 0
                
                self.max_ir_buffer_updated = 0
                self.max_red_buffer_updated = 0
                self.max_green_buffer_updated = 0
                
                if(self.frameBytes[1] == 0x83):
                    # process PARAM frame
                    self.custom_tissue_detected = self.frameBytes[3]
                    if(self.custom_tissue_detected):
                        self.custom_heart_rate = self.frameBytes[3]
                        self.custom_spo2_value = self.frameBytes[4] + self.frameBytes[5]/100
                        self.custom_heart_rate_spo_updated = 1
                elif(self.frameBytes[1] == 0x7C):
                    # process channel_data frame
                    # Check sensor type
                    if(self.frameBytes[2] == 0x64):
                        self.custom_tissue_detected = self.frameBytes[3]
                        
                        if(self.custom_tissue_detected):
                        
                            tempBuff = self.frameBytes[5:len(self.frameBytes) - 2]
                            
                            counterBuff = 0 
                            if(self.frameBytes[4] == 0x00):
                                for x in range(0, len(tempBuff), 4):
                                   tempBuff2.append(tempBuff[x] + (tempBuff[x+1] << 8))
                        
                                self.custom_red_buffer_updated = 1
                                self.custom_red_buffer = [x for x in tempBuff2]
                                
                            elif(self.frameBytes[4] == 0x02):
                                for x in range(0, len(tempBuff), 4):
                                    tempBuff2.append(tempBuff[x] + (tempBuff[x+1] << 8))
                                    counterBuff += 1
                                    
                                self.custom_ir_buffer_updated = 1
                                self.custom_ir_buffer = [x for x in tempBuff2]
        
                            elif(self.frameBytes[4] == 0x01):
                                for x in range(0, len(tempBuff), 4):
                                    tempBuff2.append(tempBuff[x] + (tempBuff[x+1] << 8))
                                    counterBuff += 1
                                    
                                self.custom_green_buffer_updated = 1
                                self.custom_green_buffer = [x for x in tempBuff2]
                               
                    elif(self.frameBytes[2] == 0x21):
                        self.max_tissue_detected = self.frameBytes[3]
                        
                        if(self.max_tissue_detected):
                        
                            tempBuff = self.frameBytes[5:len(self.frameBytes) - 2]
                            
                            counterBuff = 0 
                            if(self.frameBytes[4] == 0x00):
                                for x in range(0, len(tempBuff), 4):
                                   tempBuff2.append(tempBuff[x] + (tempBuff[x+1] << 8))
                        
                                self.max_red_buffer_updated = 1
                                self.max_red_buffer = [x for x in tempBuff2]
                                
                            elif(self.frameBytes[4] == 0x02):
                                for x in range(0, len(tempBuff), 4):
                                    tempBuff2.append(tempBuff[x] + (tempBuff[x+1] << 8))
                                    counterBuff += 1
                                    
                                self.max_ir_buffer_updated = 1
                                self.max_ir_buffer = [x for x in tempBuff2]
        
                            elif(self.frameBytes[4] == 0x01):
                                for x in range(0, len(tempBuff), 4):
                                    tempBuff2.append(tempBuff[x] + (tempBuff[x+1] << 8))
                                    counterBuff += 1
                                    
                                self.max_green_buffer_updated = 1
                                self.max_green_buffer = [x for x in tempBuff2]
            
                else:
                    # skip frame (DEBUG FRAME NOT SUPPORTED)
                    self.error_frame_counter = self.error_frame_counter + 1
                    self.custom_heart_rate_spo_updated = 0
                    self.custom_ir_buffer_updated = 0
                    self.custom_red_buffer_updated = 0
      
        except (IndexError, ValueError) as wrongFrame:
            self.error_frame_counter = self.error_frame_counter + 1
        except:
            self.deviceDisconnected = True
            ctypes.windll.user32.MessageBoxW(0, "Device disconnected.", "Communication error", 16)