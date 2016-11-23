###########################################  
#Makefile for simple programs  
###########################################  
INC=-I./include/libusb-1.0  
LIB=-L./lib -lusb-1.0  
  
CC=gcc  
CC_FLAG=-Wall -Wl,rpath,  
  
PRG=usbdemo  
OBJ= usb_vendor_cmd.o main.o ATtest.o md5.o
  
$(PRG):$(OBJ)  
	$(CC) $(INC) -o $@ $(OBJ) $(LIB)
	      
.SUFFIXES: .c .o .cpp  
.cpp.o:
	$(CC) $(CC_FLAG) $(INC) -c $*.cpp -o $*.o 
	  
.PRONY:clean  
clean:  
	@echo "Removing linked and compiled files......"  
	rm -f $(OBJ) $(PRG) 
