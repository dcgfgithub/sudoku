LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := appjni
LOCAL_CFLAGS    := -Werror -DDEBUG=1  

LOCAL_SRC_FILES := app_jni.c \
../../sw/com/com_app.c  	\
../../sw/com/com_def.c  	\
../../sw/com/com_rnd.c  	\
../../sw/com/com_list.c 	\
../../sw/com/com_msg.c  	\
../../sw/com/com_anm.c  	\
../../sw/com/com_spr.c  	\
../../sw/ogl/ogl_gfx.c  	\
../../sw/ogl/ogl_img.c  	\
../../sw/ogl/ogl_tex.c  	\
../../sw/ogl/ogl_quad.c 	\
../../sw/ogl/ogl_tran.c 	\
../../sw/ogl/ogl_spr.c  	\
../../sw/and/and_dbg.c  	\
../../sw/and/and_tex.c  	\
../../sw_app.c				\
../../src/solver.c          \
../../src/sudoku.c          \
../../src/sv.c              \
../../src/tex_main.c          	

LOCAL_C_INCLUDES := $(LOCAL_PATH) 				\
                    $(LOCAL_PATH)/../.. 		\
                    $(LOCAL_PATH)/../../src 	\
                    $(LOCAL_PATH)/../../sw 		\
                    $(LOCAL_PATH)/../../sw/com 	\
                    $(LOCAL_PATH)/../../sw/ogl 	\
                    $(LOCAL_PATH)/../../sw/and
                    
LOCAL_CFLAGS    += -std=c99
                    
LOCAL_LDLIBS    := -llog -lGLESv1_CM

include $(BUILD_SHARED_LIBRARY)
