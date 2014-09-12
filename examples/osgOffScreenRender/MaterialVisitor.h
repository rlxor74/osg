#ifndef OSGPOSTER_MATERIALVISITOR
#define OSGPOSTER_MATERIALVISITOR

#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/Geometry>

class MaterialVisitor : public osg::NodeVisitor
{
public:
    MaterialVisitor();

    META_NodeVisitor( osgPoster, MaterialVisitor );

    virtual void apply( osg::Geode& geode );
    virtual void apply( osg::Node &node );

    void setAmbient (const osg::Vec4 &color) { m_ambient = color; }
    void setAmbient (const float r, const float g, const float b, const float a = 1.0f) { m_ambient.set( r,g,b,a ); }

    void setDiffuse (const osg::Vec4 &color) { m_diffuse = color; }
    void setDiffuse (const float r, const float g, const float b, const float a = 1.0f) { m_diffuse.set( r,g,b,a ); }

    void setSpecular (const osg::Vec4 &color) { m_specular = color; }
    void setSpecular (const float r, const float g, const float b, const float a = 1.0f) { m_specular.set( r,g,b,a ); }

    void setEmission (const osg::Vec4 &color) { m_emission = color; }
    void setEmission (const float r, const float g, const float b, const float a = 1.0f) { m_emission.set( r,g,b,a ); }

    void setShininess (const unsigned int &value) { m_shininess = value; }
    
    void setOffset(const osg::Vec3 &offset) { m_offset = offset; }

private :
	osg::Vec4 m_ambient;
    osg::Vec4 m_diffuse;
    osg::Vec4 m_specular;
    osg::Vec4 m_emission;
    unsigned int m_shininess;

	osg::Vec3 m_offset;

};

#endif
