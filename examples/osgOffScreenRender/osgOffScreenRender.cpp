/* OpenSceneGraph example, osgviewerSDL.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

// (C) 2005 Mike Weiblen http://mew.cx/ released under the OSGPL.
// Simple example using GLUT to create an OpenGL window and OSG for rendering.
// Derived from osgGLUTsimple.cpp and osgkeyboardmouse.cpp

#include <osgViewer/Viewer>
//#include <osgViewer/ViewerEventHandlers>
//#include <osgGA/TrackballManipulator>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osg/Vec3f>
#include "MaterialVisitor.h"

#include "GL/osmesa.h"
//#include "gl_wrap.h"
#ifndef GL_WRAP_H
#define GL_WRAP_H

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

#ifndef GLAPIENTRY
#define GLAPIENTRY
#endif

#endif /* ! GL_WRAP_H */

#include <stdarg.h>
#include <iostream>


bool writeMatrix(const osg::Matrix& matrix)
{
    std::cout << "MATRIX {" << std::endl;
    std::cout << matrix(0,0) << " " << matrix(0,1) << " " << matrix(0,2) << " " << matrix(0,3) << std::endl;
    std::cout << matrix(1,0) << " " << matrix(1,1) << " " << matrix(1,2) << " " << matrix(1,3) << std::endl;
    std::cout << matrix(2,0) << " " << matrix(2,1) << " " << matrix(2,2) << " " << matrix(2,3) << std::endl;
    std::cout << matrix(3,0) << " " << matrix(3,1) << " " << matrix(3,2) << " " << matrix(3,3) << std::endl;
    std::cout << "}"<< std::endl;
    return true;
}

std::string string_format(const std::string fmt, ...) {
    int size = 100;
    std::string str;
    va_list ap;
    while (1) {
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.c_str(), size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {
            str.resize(n);
            return str;
        }
        if (n > -1)
            size = n + 1;
        else
            size *= 2;
    }
    return str;
}

/* Computing view matrix functions */
void computeViewMatrix( osg::Camera* camera, const osg::Vec3d& eye, const osg::Vec3d& hpr )
{
    osg::Matrixd matrix;
    matrix.makeTranslate( eye );
    matrix.preMult( osg::Matrixd::rotate( hpr[0], 0.0, 1.0, 0.0) );
    matrix.preMult( osg::Matrixd::rotate( hpr[1], 1.0, 0.0, 0.0) );
    matrix.preMult( osg::Matrixd::rotate( hpr[2], 0.0, 0.0, 1.0) );
    camera->setViewMatrix( osg::Matrixd::inverse(matrix) );
}

int main( int argc, char **argv )
{
    osg::ArgumentParser arguments( &argc, argv );
    osg::ApplicationUsage* usage = arguments.getApplicationUsage();
    usage->setDescription( arguments.getApplicationName() +
                           " is generating thumbnail images.");
    usage->setCommandLineUsage( arguments.getApplicationName() + " [options] scene_file" );
    usage->addCommandLineOption( "-h or --help", "Display this information." );
    usage->addCommandLineOption( "--color <r> <g> <b>", "The background color." );
    usage->addCommandLineOption( "--prefix <filename>", "Set output filename(default=thumbnail)." );
    usage->addCommandLineOption( "--ext <extension>", "Set output file extension(default=png)." );
    usage->addCommandLineOption( "--width <width>", "Set thumbnail width(default=640)." );
    usage->addCommandLineOption( "--height <height>", "Set thumbnail height(default=480)." );
    usage->addCommandLineOption( "--eye <x> <y> <z>", "Set camera position." );
    usage->addCommandLineOption( "--up <x> <y> <z>", "Set up direction vector." );

    if ( arguments.read("-h") || arguments.read("--help") )
    {
        usage->write( std::cout );
        return 1;
    }

    osg::Vec4 bgColor(1.0f, 1.0f, 1.0f, 1.0f);
    osg::Vec3d eye(1.0, -2.45, 1.0);
    osg::Vec3d up(0.0, 0.0, 1.0);
    std::string prefix = "thumbnail";
    std::string ext = "png";
    std::string outputName;
    int Width = 600;
    int Height = 400;

    double fovy = 40.0, aspectRatio = 1.5, zNear = 1, zFar = 10.0;

    //[[---------------------------------------------------------------------------------------------------------
    // 0. options
    while ( arguments.read("--color", bgColor.r(), bgColor.g(), bgColor.b()) ) {}
    while ( arguments.read("--prefix", prefix) ) {}
    while ( arguments.read("--ext", ext) ) {}
    while ( arguments.read("--width", Width) ) {}
    while ( arguments.read("--height", Height) ) {}
    while ( arguments.read("--eye", eye.x(), eye.y(), eye.z()) ) {}
    while ( arguments.read("--up", up.x(), up.y(), up.z()) ) {}

    while ( arguments.read("--fov", fovy) ) {}
    while ( arguments.read("--ratio", aspectRatio) ) {}
    while ( arguments.read("--near", zNear) ) {}
    while ( arguments.read("--far", zFar) ) {}

    //---------------------------------------------------------------------------------------------------------]]

    //[[---------------------------------------------------------------------------------------------------------
    // 1. ReadWrite에서 옵션 처리 	
    std::string str;
    while (arguments.read("-O",str))
    {
        osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options;
        options->setOptionString(str);
        osgDB::Registry::instance()->setOptions(options);
    }
    //---------------------------------------------------------------------------------------------------------]]


	/* Allocate the image buffer */
	osg::Image* img = new osg::Image();    
	img->allocateImage( Width, Height, 1, GL_RGBA, GL_UNSIGNED_BYTE );

    OSMesaContext ctx;
    /* Create an RGBA-mode context */
#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 305
    /* specify Z, stencil, accum sizes */
    ctx = OSMesaCreateContextExt( OSMESA_RGBA, 16, 0, 0, NULL );
#else
    ctx = OSMesaCreateContext( OSMESA_RGBA, NULL );
#endif
    if (!ctx) {
        printf("OSMesaCreateContext failed!\n");
        exit(1);
    }

    /* Bind the buffer to the context and make it current */
    if (!OSMesaMakeCurrent( ctx, img->data(), GL_UNSIGNED_BYTE, Width, Height )) {
        printf("OSMesaMakeCurrent failed!\n");
        return 0;
    }

	int z, s, a;
	glGetIntegerv(GL_DEPTH_BITS, &z);
	glGetIntegerv(GL_STENCIL_BITS, &s);
	glGetIntegerv(GL_ACCUM_RED_BITS, &a);
	//printf("Depth=%d Stencil=%d Accum=%d\n", z, s, a);

    // load the scene.
    osg::ref_ptr<osg::Node> scene = osgDB::readNodeFiles( arguments );
    if (!scene)
    {
        std::cout << arguments.getApplicationName() <<": No data loaded." << std::endl;
        return 1;
    }

    const osg::BoundingSphere& bs = scene->getBound();
    float scale = 1/bs.radius();

    //[[---------------------------------------------------------------------------------------------------------
    // 2. obj 파일 로딩 후 material 교체하기 위함. 	
    MaterialVisitor mv = MaterialVisitor();
 	mv.setOffset(bs.center());
    scene->accept(mv);
    //---------------------------------------------------------------------------------------------------------]]

    //[[---------------------------------------------------------------------------------------------------------
    // 3. 
    osg::MatrixTransform* root = new osg::MatrixTransform;
    root->addChild( scene );
    //---------------------------------------------------------------------------------------------------------]]

    osgViewer::Viewer viewer;
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> gw = viewer.setUpViewerAsEmbeddedInWindow(0,0,Width,Height);

    osg::Camera* camera = viewer.getCamera();
    camera->setClearColor( bgColor );
    camera->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    camera->setViewport( 0, 0, Width, Height );
	camera->getProjectionMatrix().makePerspective(fovy, aspectRatio, zNear, zFar);

    // 4. object의 높이에 따라 eye 위치를 변경하기 위함.
    //if (scale < 0.1) eye *= (1-scale);
    osg::Matrixd matrix;
    matrix.makeLookAt( eye, osg::Vec3d( 0,-0.2,0 ), up );
	camera->setViewMatrix( matrix );

    //[[---------------------------------------------------------------------------------------------------------
    // 5. light 설정
	osg::Light* light = viewer.getLight();
	light->setAmbient(osg::Vec4(0.3f,0.3f,0.3f,1.0f));
	light->setDiffuse(osg::Vec4(0.8f,0.8f,0.8f,1.0f));
	light->setSpecular(osg::Vec4(0.0f,0.0f,0.0f,1.0f));
	light->setPosition(osg::Vec4(1.0f,1.0f,1.0f,0.0f));
	light->setDirection(osg::Vec3(-1.0f,-1.0f,-1.0f));
    //---------------------------------------------------------------------------------------------------------]]

    viewer.setSceneData( root );
    viewer.realize();

	for (int i=0; i < 8; i++) {

        root->setMatrix( osg::Matrix::scale(scale, scale, scale) *
					     osg::Matrix::rotate(osg::inDegrees(45.0*i),0.0f,0.0f,1.0f) );

	    viewer.frame();

        outputName = string_format("%s_%d.%s", prefix.c_str(), i, ext.c_str());

        osgDB::writeImageFile( *img, outputName );

	}

    // destroy the context
    OSMesaDestroyContext( ctx );


    std::cout << "==========================" << std::endl;
    std::cout << "all done" << std::endl;

    return 0;
}

/*EOF*/
