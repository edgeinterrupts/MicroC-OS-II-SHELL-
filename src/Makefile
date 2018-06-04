# makfile configuration
NAME      	= fnlprjapp
OBJDIR    	= ./obj
OUTDIR    	= ./exe
RTOSDIR   	= /cygdrive/c/RIT/301-742/fnlprj/fnlprj/src/ucosII277
PORTDIR   	= /cygdrive/c/RIT/301-742/fnlprj/fnlprj/src/port
HWDIR     	= /cygdrive/c/RIT/301-742/fnlprj/fnlprj/src/hardware
APPDIR    	= /cygdrive/c/RIT/301-742/fnlprj/fnlprj/src/application
INCDIR	        = -I./includes -I$(RTOSDIR) -I$(PORTDIR)

OBJECTS = $(OBJDIR)/app_shellServices.o	\
	  $(OBJDIR)/app_xmodem.o	\
	  $(OBJDIR)/app_shell.o	\
	  $(OBJDIR)/app_serialInterface.o	\
	  $(OBJDIR)/app_main.o	\
	  $(OBJDIR)/hw_common.o	\
	  $(OBJDIR)/hw_lcd.o	      \
	  $(OBJDIR)/hw_uart0.o	\
	  $(OBJDIR)/hw_init.o       \
	  $(OBJDIR)/Os_cpu_a.o	\
	  $(OBJDIR)/Os_cpu_c.o	\
	  $(OBJDIR)/os_core.o	      \
	  $(OBJDIR)/os_flag.o	      \
	  $(OBJDIR)/os_mbox.o	      \
	  $(OBJDIR)/os_mem.o	      \
	  $(OBJDIR)/os_mutex.o	\
	  $(OBJDIR)/os_q.o	      \
	  $(OBJDIR)/os_sem.o	      \
	  $(OBJDIR)/os_task.o	      \
	  $(OBJDIR)/os_time.o	

#CPU Number
CPU       = msp430x149

#gcc and ld flags
CFLAGS      = -mmcu=${CPU} $(INCDIR) -O2 -Wall -c -gstabs 
LFLAGS	    = -m ${CPU} -T msp430x149.ld 

# need for linker. See http://mspgcc.sourceforge.net/faq/x62.html. 
extra_lib_path   = /cygdrive/c/mspgcc/bin
extra_lib        = /cygdrive/c/mspgcc/lib/gcc-lib/msp430/3.2.3/msp1

#switch the compiler (for the internal make rules)
CC              = msp430-gcc

.PHONY: all FORCE clean download download-jtag download-bsl dist

#all should be the first target. it's built when make is run without args
all: $(OUTDIR)/${NAME}.elf $(OUTDIR)/${NAME}.a43 $(OUTDIR)/${NAME}.lst

#additional rules for files

# Build the exectuable using the provided linker script file. 
# Use the linker through GCC rather than explicity calling it. 
$(OUTDIR)/${NAME}.elf: ${OBJECTS}
	${CC} -mmcu=${CPU} -L[extra_lib] -lgcc -lc -lgcc -T msp430x149.ld -o $@ ${OBJECTS}

#additional rules for files
$(OUTDIR)/${NAME}.a43: $(OUTDIR)/${NAME}.elf
	msp430-objcopy -O ihex $^ $@

#rule for creating a listing file from an elf file.
$(OUTDIR)/${NAME}.lst: $(OUTDIR)/${NAME}.elf
	msp430-objdump -dSt $^ > $@

#rule for downloading elf file on the board.
download-jtag: all
	msp430-jtag -ep $(OUTDIR)/${NAME}.elf

#rule for bsl. bsl? 
download-bsl: all
	msp430-bsl -e $(OUTDIR)/${NAME}.elf

#rule for make clean. deletes all temp files.
clean:
	rm -f $(OUTDIR)/${NAME}.elf $(OUTDIR)/${NAME}.a43 $(OUTDIR)/${NAME}.lst ${OBJECTS}

#backup archive
dist:
	tar czf dist.tgz *.c *.h *.txt makefile

#dummy target as dependecy if something has to be build everytime
FORCE:

#project dependencies

#APPLICATION CODE

$(OBJDIR)/app_main.o : $(APPDIR)/app_main.c
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/app_serialInterface.o : $(APPDIR)/app_serialInterface.c
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/app_shell.o : $(APPDIR)/app_shell.c
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/app_xmodem.o : $(APPDIR)/app_xmodem.c
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/app_shellServices.o : $(APPDIR)/app_shellServices.c
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/hw_init.o : $(HWDIR)/hw_init.c
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/hw_lcd.o : $(HWDIR)/hw_lcd.c
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/hw_uart0.o : $(HWDIR)/hw_uart0.c
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/hw_common.o : $(HWDIR)/hw_common.c
	$(CC) $(CFLAGS) -o $@ $<

#OS CODE
$(OBJDIR)/Os_cpu_c.o : $(PORTDIR)/Os_cpu_c.c
	$(CC) $(CFLAGS) -o $@ $<
	
$(OBJDIR)/os_core.o : $(RTOSDIR)/os_core.c
	$(CC) $(CFLAGS) -I$(RTOSDIR) -o $@ $<
	
$(OBJDIR)/os_flag.o : $(RTOSDIR)/os_flag.c
	$(CC) $(CFLAGS) -o $@ $<
	
$(OBJDIR)/os_mbox.o : $(RTOSDIR)/os_mbox.c
	$(CC) $(CFLAGS) -o $@ $<
	
$(OBJDIR)/os_mem.o : $(RTOSDIR)/os_mem.c
	$(CC) $(CFLAGS) -o $@ $<
	
$(OBJDIR)/os_mutex.o : $(RTOSDIR)/os_mutex.c
	$(CC) $(CFLAGS) -o $@ $<
	
$(OBJDIR)/os_q.o : $(RTOSDIR)/os_q.c
	$(CC) $(CFLAGS) -o $@ $<
	
$(OBJDIR)/os_sem.o : $(RTOSDIR)/os_sem.c
	$(CC) $(CFLAGS) -o $@ $<
	
$(OBJDIR)/os_task.o : $(RTOSDIR)/os_task.c
	$(CC) $(CFLAGS) -o $@ $<
	
$(OBJDIR)/os_time.o : $(RTOSDIR)/os_time.c
	$(CC) $(CFLAGS) -o $@ $<
	
$(OBJDIR)/Os_cpu_a.o : $(PORTDIR)/Os_cpu_a.S
	$(CC) -g -mmcu=$(CPU) -D_GNU_ASSEMBLER_ -gstabs -c -o $@ $<
