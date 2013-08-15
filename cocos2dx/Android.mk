LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dx_static

LOCAL_MODULE_FILENAME := libcocos2d

LOCAL_SRC_FILES := \
CCConfiguration.cpp \
CCDeprecated.cpp \
CCScheduler.cpp \
CCCamera.cpp \
ccFPSImages.c \
ccTypes.cpp \
actions/CCAction.cpp \
actions/CCActionCamera.cpp \
actions/CCActionCatmullRom.cpp \
actions/CCActionEase.cpp \
actions/CCActionGrid.cpp \
actions/CCActionGrid3D.cpp \
actions/CCActionInstant.cpp \
actions/CCActionInterval.cpp \
actions/CCActionManager.cpp \
actions/CCActionPageTurn3D.cpp \
actions/CCActionProgressTimer.cpp \
actions/CCActionTiledGrid.cpp \
actions/CCActionTween.cpp \
base_nodes/CCAtlasNode.cpp \
base_nodes/CCNode.cpp \
cocoa/CCAffineTransform.cpp \
cocoa/CCGeometry.cpp \
cocoa/CCAutoreleasePool.cpp \
cocoa/CCDictionary.cpp \
cocoa/CCNS.cpp \
cocoa/CCObject.cpp \
cocoa/CCSet.cpp \
cocoa/CCString.cpp \
cocoa/CCArray.cpp \
cocoa/CCDataVisitor.cpp \
cocoa/CCData.cpp \
cocos2d.cpp \
CCDirector.cpp \
draw_nodes/CCDrawingPrimitives.cpp \
draw_nodes/CCDrawNode.cpp \
effects/CCGrabber.cpp \
effects/CCGrid.cpp \
kazmath/src/aabb.c \
kazmath/src/mat3.c \
kazmath/src/mat4.c \
kazmath/src/neon_matrix_impl.c \
kazmath/src/plane.c \
kazmath/src/quaternion.c \
kazmath/src/ray2.c \
kazmath/src/utility.c \
kazmath/src/vec2.c \
kazmath/src/vec3.c \
kazmath/src/vec4.c \
kazmath/src/GL/mat4stack.c \
kazmath/src/GL/matrix.c \
keypad_dispatcher/CCKeypadDelegate.cpp \
keypad_dispatcher/CCKeypadDispatcher.cpp \
keyboard_dispatcher/CCKeyboardDispatcher.cpp \
label_nodes/CCFont.cpp \
label_nodes/CCFontAtlas.cpp \
label_nodes/CCFontAtlasCache.cpp \
label_nodes/CCFontAtlasFactory.cpp \
label_nodes/CCFontCache.cpp \
label_nodes/CCFontDefinition.cpp \
label_nodes/CCFontFNT.cpp \
label_nodes/CCFontFreeType.cpp \
label_nodes/CCFontRenderFreeType.cpp \
label_nodes/CCLabel.cpp \
label_nodes/CCLabelAtlas.cpp \
label_nodes/CCLabelBMFont.cpp \
label_nodes/CCLabelTTF.cpp \
label_nodes/CCLabelTextFormatter.cpp \
label_nodes/CCStringBMFont.cpp \
label_nodes/CCStringTTF.cpp \
label_nodes/CCTextImage.cpp \
layers_scenes_transitions_nodes/CCLayer.cpp \
layers_scenes_transitions_nodes/CCScene.cpp \
layers_scenes_transitions_nodes/CCTransitionPageTurn.cpp \
layers_scenes_transitions_nodes/CCTransition.cpp \
layers_scenes_transitions_nodes/CCTransitionProgress.cpp \
menu_nodes/CCMenu.cpp \
menu_nodes/CCMenuItem.cpp \
misc_nodes/CCClippingNode.cpp \
misc_nodes/CCMotionStreak.cpp \
misc_nodes/CCProgressTimer.cpp \
misc_nodes/CCRenderTexture.cpp \
particle_nodes/CCParticleExamples.cpp \
particle_nodes/CCParticleSystem.cpp \
particle_nodes/CCParticleBatchNode.cpp \
particle_nodes/CCParticleSystemQuad.cpp \
platform/CCSAXParser.cpp \
platform/CCThread.cpp \
platform/CCFileUtils.cpp \
platform/CCEGLViewProtocol.cpp \
script_support/CCScriptSupport.cpp \
shaders/ccShaders.cpp \
shaders/CCGLProgram.cpp \
shaders/ccGLStateCache.cpp \
shaders/CCShaderCache.cpp \
sprite_nodes/CCAnimation.cpp \
sprite_nodes/CCAnimationCache.cpp \
sprite_nodes/CCSprite.cpp \
sprite_nodes/CCSpriteBatchNode.cpp \
sprite_nodes/CCSpriteFrame.cpp \
sprite_nodes/CCSpriteFrameCache.cpp \
support/ccUTF8.cpp \
support/CCNotificationCenter.cpp \
support/CCProfiling.cpp \
support/TransformUtils.cpp \
support/user_default/CCUserDefaultAndroid.cpp \
support/base64.cpp \
support/ccUtils.cpp \
support/CCVertex.cpp \
support/data_support/ccCArray.cpp \
support/image_support/TGAlib.cpp \
support/tinyxml2/tinyxml2.cpp \
support/zip_support/ZipUtils.cpp \
support/zip_support/ioapi.cpp \
support/zip_support/unzip.cpp \
support/component/CCComponent.cpp \
support/component/CCComponentContainer.cpp \
text_input_node/CCIMEDispatcher.cpp \
text_input_node/CCTextFieldTTF.cpp \
textures/CCTexture2D.cpp \
textures/CCTextureAtlas.cpp \
textures/CCTextureCache.cpp \
platform/third_party/common/etc/etc1.cpp\
platform/third_party/common/s3tc/s3tc.cpp\
tilemap_parallax_nodes/CCParallaxNode.cpp \
tilemap_parallax_nodes/CCTMXLayer.cpp \
tilemap_parallax_nodes/CCTMXObjectGroup.cpp \
tilemap_parallax_nodes/CCTMXTiledMap.cpp \
tilemap_parallax_nodes/CCTMXXMLParser.cpp \
tilemap_parallax_nodes/CCTileMapAtlas.cpp \
touch_dispatcher/CCTouchDispatcher.cpp \
touch_dispatcher/CCTouchHandler.cpp \
touch_dispatcher/CCTouch.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/kazmath/include \
                    $(LOCAL_PATH)/platform/android \
                    $(LOCAL_PATH)/platform/third_party/common/etc\
                    $(LOCAL_PATH)/platform/third_party/common/s3tc

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/kazmath/include \
                    $(LOCAL_PATH)/platform/android \
                    $(LOCAL_PATH)/platform/third_party/common/etc\
                    $(LOCAL_PATH)/platform/third_party/common/s3tc


LOCAL_LDLIBS := -lGLESv2 \
                -llog \
                -lz \
                -landroid

LOCAL_EXPORT_LDLIBS := -lGLESv2 \
                       -llog \
                       -lz \
                       -landroid

LOCAL_WHOLE_STATIC_LIBRARIES := cocos_libpng_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_jpeg_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_libxml2_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_libtiff_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_libwebp_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_freetype2_static

# define the macro to compile through support/zip_support/ioapi.c
LOCAL_CFLAGS   := -Wno-psabi -DUSE_FILE32API
LOCAL_EXPORT_CFLAGS   := -Wno-psabi -DUSE_FILE32API

include $(BUILD_STATIC_LIBRARY)

$(call import-module,libjpeg)
$(call import-module,libpng)
$(call import-module,libtiff)
$(call import-module,libwebp)
$(call import-module,libfreetype2)
