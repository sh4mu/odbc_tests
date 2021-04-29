INCLUDEDIRS = -I/home/sceuser/esae4linuxR50SU1_SSAP/add-on/cslee/slee/lib/cc/vendor/unixODBC/LX/odbc64/include 
LIBSDEBUG = -Linstall_dir/lib -lttenD
LIBS = -L/home/sceuser/esae4linuxR50SU1_SSAP/add-on/cslee/slee/lib/cc/vendor/unixODBC/LX/odbc64/lib -lodbc
CFLAGS = -fPIC -Wno-deprecated -std=c++11 -m64
CC = g++

# Link directly with the TimesTen
# ODBC production driver

SOURCES=$(wildcard *.C)
OBJECTS = $(SOURCES:%.C=%.o)

%.o: %.C
	 $(CC) -g $(CFLAGS) $(INCLUDEDIRS) -c $< -o $@

tttest: $(OBJECTS)
	$(CC) -g $(CFLAGS) $(INCLUDEDIRS) $(OBJECTS) -o tttest $(LIBS)

clean:
	rm *.o tttest