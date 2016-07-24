# vim: set noexpandtab:
# 
# A simple implementation of DES (Data Encryption Algorithm)
#
# Sun Dec 20 14:19:33 IST 2009
# Aniruddha. A (aniruddha.a@gmail.com)
#  

CC = gcc
CFLAGS = -Wall -g -ggdb
OBJECTS = getopts.o flblkread.o aes.o
#LINKOPTS = -lefence
all : aes

aes : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LINKOPTS) -o $@ 

%.o : %.c
	$(CC) $(CFLAGS) -c $<
clean:
	@rm -f $(OBJECTS) aes
