import socket # required libraries
import sys
import copy
import time
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening (ESP8266-01)
server_address = ('192.168.1.34', 5000)
print('connecting to {} port {}'.format(*server_address))
sock.connect(server_address)

dataNoReceiveCounter = 0    # Counter to keep number of no connection loop
wholeMessage = ""           # String to keep incoming data packege
decodeAll = True            # Flag to detect decoding timing
byteList = [42]             # List to keep incoming bytes individually
temps =[]                   # Creates an empty list
longitudes = []
latitudes = []
cutLoop = True
# All loop is in a try - except structure to disconnect properly, when something goes wrong.
try:
    # Data handling is utilized in this loop.
    # Students can use this loop for parsing and saving data, or it is allowed to create another thread (loop) properly so that the data handling occur within.
    while cutLoop:
        # Try - except structure to handle data transfer
        try:
            data = sock.recv(1024)              # Variable to keep incoming data package in disorder
            # Loop to examine all incoming data individually
            for val in data:
                # Detecting specific characters in incoming data package
                if val == 42:                   # 42 is the ASCII code of '*' character, which means our package ends here.
                    decodeAll = True            # Change flag to true, means that the specified package is ended.
                    break                       # Breaking the for loop, since the specific character is received; ready to transform all data
                elif val == 35:                 # 35 is the ASCII code of '#' character, which means our package starts here.
                    wholeMessage = ""           # Clear the variable for new incoming package
                    byteList.clear()            # Clear the variable for new incoming package
                    break                       # Breaking the for loop, since the specific character is received; ready to construct new package data
                byteList.append(val)            # If incoming character is not specific, add the ASCII value of incoming character to list to keep bytes
            # If structure to decode data; decodeAll is true, means that the data is constructed with its ending character;
            # Ready to transform all data to string and keep it in wholeMessage variable.
            if decodeAll:
                for val in byteList:
                    if ((val <= 57) and (val >= 48)) or (val == 46) or (val == 42) or (val == 44):  # Parsing operations
                        wholeMessage += chr(val)
                        if len(wholeMessage) == 21:
                            temp = wholeMessage[0:5]
                            temp_float = float(temp)
                            longitude = wholeMessage[6:13]
                            longitude_float = float(longitude)
                            latitude = wholeMessage[14:21]
                            latitude_float = float(latitude)
                            temps.append(temp_float)                            # Adds to the empty list sequentially
                            longitudes.append(longitude_float)
                            latitudes.append(latitude_float)
                            print(temp)
                            print(longitude)
                            print(latitude)
                    else:
                        print("parse error")
                        break
                                       
                        
                                 # Print data on screen for debugging.
                decodeAll = False               # Change flag to false, means that the transformation for the receiving package is done. Ready to receive new package.
                print(len(temps))
            if len(temps) == 30 and len(longitudes) == 30 and len(latitudes) == 30: # When the desired number of data arrives, a heat map is created.
                 
                data = {'latitude': latitudes,
                        'longitude': longitudes,
                        'temperature':temps
                        }
                                    

                
                df = pd.DataFrame(data)

                
                plt.figure(figsize=(20, 12))
                heatmap_data = df.pivot_table(values='temperature', index='longitude', columns='latitude')
                sns.heatmap(data=heatmap_data, annot=True, cmap='YlOrRd', cbar_kws={'label': 'temperature (C)'})
                plt.title('Heat map')
                plt.xlabel('longitude')
                plt.ylabel('latitude')
                plt.show()
                cutLoop = False
                break
    
            dataNoReceiveCounter = 0            # Change data to 0, since data is receiving properly
        # Structure for detecting problems in receiving or transforming data. Increase counter by 1 for each loop.
        except:
            dataNoReceiveCounter += 1
            print('no receive')                 # Print message to warn the user.
        # If the problem continues for specified times (20 for this case), break the loop and disconnect.
        if dataNoReceiveCounter > 20:
            break
# If the loop is broken, then close the socket and warn the user.
finally:
    print('closing socket')
    sock.close()
