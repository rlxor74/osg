#include "MaterialVisitor.h"
#include <osg/Material>
#include <iostream>

/* PosterVisitor: A visitor for adding culling callbacks to newly allocated paged nodes */
MaterialVisitor::MaterialVisitor()
: osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
{
    m_offset.set(0.0f, 0.0f, 0.0f);

    m_ambient.set(1.0f, 1.0f, 1.0f, 1.0f);
    m_diffuse.set(0.7f, 0.7f, 0.7f, 1.0f);
    m_specular.set(0.5f, 0.5f, 0.5f, 1.0f);
    m_emission.set(0.0f, 0.0f, 0.0f, 1.0f);
    m_shininess = 100;
}

void MaterialVisitor::apply ( osg::Node& node )
{
    traverse( node );
}

void MaterialVisitor::apply( osg::Geode& geode )
{
    for(unsigned int i = 0; i < geode.getNumDrawables(); i++ ) {

        osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));

        if (geom) {

			if (m_offset.x() != 0 || m_offset.y() != 0 || m_offset.z() != 0) {
				osg::ref_ptr<osg::Vec3Array> va;
				va = dynamic_cast<osg::Vec3Array*>( geom->getVertexArray() );

				if( va ){
					for( unsigned int i=0; i < va->size(); i++ ){
						osg::Vec3* v = &va->operator[](i);
						*v -= m_offset;
					}
				}
				
				geom->dirtyDisplayList();
				geom->dirtyBound();
			}

            osg::StateSet* stateset = geom->getOrCreateStateSet();

            if (stateset) {

                osg::ref_ptr<osg::Material> material = new osg::Material();

                material->setAmbient  (osg::Material::FRONT_AND_BACK, m_ambient);
                material->setDiffuse  (osg::Material::FRONT_AND_BACK, m_diffuse);
                material->setSpecular (osg::Material::FRONT_AND_BACK, m_specular);
                material->setShininess(osg::Material::FRONT_AND_BACK, m_shininess);
                material->setEmission (osg::Material::FRONT_AND_BACK, m_emission);

				stateset->setMode( GL_RESCALE_NORMAL, osg::StateAttribute::ON );

                stateset->setAttributeAndModes(material, osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);

            }
      
        }
    }
    
    traverse( geode );
    
}
