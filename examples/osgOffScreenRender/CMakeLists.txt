SET(TARGET_EXTERNAL_LIBRARIES OSMesa32 )
SET(TARGET_SRC osgOffScreenRender.cpp MaterialVisitor.cpp)
# INCLUDE_DIRECTORIES( ${SDL_INCLUDE_DIR} )

#### end var setup  ###
SETUP_EXAMPLE(osgOffScreenRender)

