INCLUDEDIRS = -I/home/sceuser/esae4linuxR50SU1_SSAP/add-on/cslee/slee/lib/cc/vendor/unixODBC/LX/odbc64/include 
LIBSDEBUG = -Linstall_dir/lib -lttenD
LIBS = -L/home/sceuser/esae4linuxR50SU1_SSAP/add-on/cslee/slee/lib/cc/vendor/unixODBC/LX/odbc64/lib -lodbc
GCC = g++ -fPIC -Wno-deprecated -std=c++11 -m64

# Link directly with the TimesTen
# ODBC production driver
tttest:tttest.o
	$(GCC) -o tttest tttest.o $(LIBS)

tttest.o:
	$(GCC) -c tttest.C $(INCLUDEDIRS)

clean:
	rm *.o tttest