DIRS+=lib/swig/java
DIRS+=lib/swig/java/com
DIRS+=lib/swig/java/com/opensec

LIBBINDING=aaa

JAVA_RUNDIG=bin
JAVA_INCLUDE+=-I$(JAVA_HOME)/include
ifdef CONFIG_LINUX
JAVA_INCLUDE+=-I$(JAVA_HOME)/include/linux
endif
ifdef CONFIG_WINDOWS
JAVA_INCLUDE+=-I$(JAVA_HOME)/include/win32
endif
ifdef CONFIG_DARWIN
JAVA_INCLUDE+=-I/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers
JAVA_INCLUDE+=-I$(JAVA_HOME)/include/darwin
endif
ifdef CONFIG_DARWIN
JAVA_CFLAGS+=-x c -DTARGET_RT_MAC_CFM
endif

JAVA_PACKAGE=com.opensec.aaa
JAVA_PACKAGE_PATH=com/opensec/aaa

JAVA_CLASSES=Client Option Bind Link Attr Test AttrConstants
JAVA_PACKAGE_PREFIX=-C $(o)/lib/swig/java 

LIBAAA_JAR=$(o)/lib/swig/java/libaaa.jar

JAVA_PACKAGE_BUILD=$(addprefix $(JAVA_PACKAGE_PREFIX), $(addsuffix .class, $(addprefix $(JAVA_PACKAGE_PATH)/, $(JAVA_CLASSES))))

PROGS+=$(LIBAAA_JAR)

$(LIBAAA_JAR): $(o)/lib/swig/java/aaa.oo $(LIBAAA)
	$(Q)javac $(s)/lib/swig/java/com/opensec/aaa/*.java -d $(o)/lib/swig/java
	$(M)JAR $@
	$(Q)jar cf $(LIBAAA_JAR) $(JAVA_PACKAGE_BUILD)

#$(s)/lib/swig/java/aaa.c: $(addprefix $(s)/lib/swig/java/, $(addsuffix .java, $(addprefix $(JAVA_PACKAGE_PATH)/, $(JAVA_CLASSES))))

$(o)/lib/swig/java/aaa.o: $(s)/lib/swig/java/aaa.c
$(o)/lib/swig/java/aaa.o: CFLAGS+=-Wall $(JAVA_CFLAGS) $(JAVA_INCLUDE) $(SWIG_NO_WARRN)
$(o)/lib/swig/java/aaa.oo: $(s)/lib/swig/java/aaa.c
$(o)/lib/swig/java/aaa.oo: CFLAGS+=-Wall $(JAVA_CFLAGS) $(JAVA_INCLUDE) $(SWIG_NO_WARRN) 

LIBAAA_OBJECTS+=$(o)/lib/swig/java/aaa

$(s)/lib/swig/java/aaa.c: $(s)/lib/aaa/lib.i 
	$(M)SW-JAVA $<
	$(Q)swig -package $(JAVA_PACKAGE) -I$(s) -I$(s)/lib -I$(s)/lib/aaa -java -outdir $(s)/lib/swig/java/$(JAVA_PACKAGE_PATH) -o $@ $<
