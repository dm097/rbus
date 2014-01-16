RDK_LOGGER_DIR?=$(RDK_DIR)/rdklogger
include $(RDK_LOGGER_DIR)/config.mak
CFLAGS?=  -Wno-format -Wunused -DUSE_CC_GETTIMEOFDAY
ARFLAGS?=rcs
SRC_DIR=src
LIBNAME=rdkloggers
SYSROOT_FLAG?="--sysroot=$(PLATFORM_SDK)"

TST_DIR=test
TST_BIN=logger_test
TST_FLAGS=-lpthread  -Llib

DOC_DIR=doc

LIBFILE=$(LIBDIR)/lib$(LIBNAME).so
OBJ_DIR=$(BUILD_DIR)/objs_$(LIBNAME)
TEST_OBJ_DIR=$(BUILD_DIR)/objs_$(TST_BIN)

INCLUDES+= -Iinclude -I$(SRC_DIR)/include -I$(PLATFORM_SDK)/include/glib-2.0 -I$(PLATFORM_SDK)/lib/glib-2.0/include -I$(RDK_DIR)/opensource/src/log4c/src
CFLAGS += -DGCC4_XXX

CFLAGS+= $(INCLUDES)
OBJS=$(OBJ_DIR)/rdk_logger_init.o\
			$(OBJ_DIR)/rdk_logger_util.o \
			$(OBJ_DIR)/rdk_debug.o \
			$(OBJ_DIR)/rdk_debug_priv.o


TESTS=$(TEST_OBJ_DIR)/rdk_logger_test_main.o \
      $(TEST_OBJ_DIR)/rdk_logger_debug_test.o 

.PHONY : all clean test doc

all:  $(LIBFILE) 


$(LIBFILE): $(LIBDIR) $(OBJ_DIR) $(OBJS) $(UTILS_LIBFILE)
	$(CC) -shared -fPIC -o $(LIBFILE) $(OBJS)  -L$(LIBDIR) -L$(RDK_LOGGER_DIR)/../opensource/lib -llog4c -lglib-2.0

$(OBJ_DIR)/%.o :$(SRC_DIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ_DIR)/%.o :$(SRC_DIR)/%.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	
$(LIBDIR):
	mkdir -p $(LIBDIR)

$(TEST_OBJ_DIR):
	mkdir -p $@

$(TEST_OBJ_DIR)/%.o :$(TST_DIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

doc : 
	doxygen $(DOC_DIR)/rdklogger-doxygen.cfg
	

clean :
	rm -rf $(OBJ_DIR) $(LIBFILE) $(TEST_OBJ_DIR)

test : $(TEST_OBJ_DIR) $(LIBFILE)  $(TESTS)
	$(CC) -o $(BUILD_DIR)/$(TST_BIN) $(TESTS) -l$(LIBNAME) -L $(PLATFORM_SDK)/lib -L  $(RDK_LOGGER_DIR)/../opensource/lib -L $(LIBDIR) $(TST_FLAGS)  $(SYSROOT_FLAG) -llog4c
	
