#ifndef _GLSCENEGRAPHRENDERER_H_
#define _GLSCENEGRAPHRENDERER_H_

#include "INode.h"
#include "OpenGLFunctions.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "Material.h"
#include "TextureImage.h"
#include "ObjectInstance.h"
#include "IVertexData.h"
#include "ShaderLocationsVault.h"
#include <string>
#include <map>
#include <stack>
using namespace std;

namespace sgraph
{

/**
 * This is a scene graph renderer implementation that works specifically
 * with the Qt library
 * It mandates OpenGL 3 and above.
 * \author Amit Shesh
 */
class GLScenegraphRenderer
{
    /**
     * The Qt specific rendering context
     */
private:
    util::OpenGLFunctions *glContext;
    /**
     * A table of shader locations and variable names
     */
protected :
    util::ShaderLocationsVault shaderLocations;
    /**
     * A table of shader variables -> vertex attribute names in each mesh
     */
    map<string,string> shaderVarsToVertexAttribs;

    /**
     * A map to store all the textures
     */
    map<string, util::TextureImage *> textures;
    /**
     * A table of renderers for individual meshes
     */
    map<string, util::ObjectInstance *> meshRenderers;

    /**
     * A variable tracking whether shader locations have been set. This must be done before
     * drawing!
     */
    bool shaderLocationsSet;

public:
    GLScenegraphRenderer()
    {
        shaderLocationsSet = false;
    }

    /**
     * Specifically checks if the passed rendering context is the correct JOGL-specific
     * rendering context
     * \param obj the rendering context (should be OpenGLFunctions)
     */
    void setContext(util::OpenGLFunctions *obj)
    {
        glContext = obj;
    }

    /**
     * Add a mesh to be drawn later.
     * The rendering context should be set before calling this function, as this function needs it
     * This function creates a new sgraph::GLMeshRenderer object for this mesh
     * \param name the name by which this mesh is referred to by the scene graph
     * \param mesh the util::PolygonMesh object that represents this mesh
     */
    template <class K>
    void addMesh(const string& name,
                 util::PolygonMesh<K>& mesh) throw(runtime_error)
    {
        if (!shaderLocationsSet)
            throw runtime_error("Attempting to add mesh before setting shader variables. Call initShaderProgram first");
        if (glContext==NULL)
            throw runtime_error("Attempting to add mesh before setting GL context. Call setContext and pass it a GLAutoDrawable first.");

        //verify that the mesh has all the vertex attributes as specified in the map
        if (mesh.getVertexCount()<=0)
            return;
        K vertexData = mesh.getVertexAttributes()[0];
        for (map<string,string>::iterator it=shaderVarsToVertexAttribs.begin();
             it!=shaderVarsToVertexAttribs.end();it++) {
            if (!vertexData.hasData(it->second))
                throw runtime_error("Mesh does not have vertex attribute "+it->second);
        }
        util::ObjectInstance *mr = new util::ObjectInstance(name);
        mr->initPolygonMesh<K>(*glContext,
                            shaderLocations,
                            shaderVarsToVertexAttribs,
                            mesh);
        this->meshRenderers[name] = mr;
    }

    void addTexture(const string& name,const string& path)
    {
        util::TextureImage *image = NULL;
        try {
            std::cout << "[texture creating]: " << name << " -- " << path << "\n";
            image = new util::TextureImage(path,name);
        } catch (runtime_error e) {
            throw runtime_error("Texture "+path+" cannot be read!");
        }
        textures[name]=image;
    }


    /**
     * Begin updating the bounding box from the root
     * \param root
     * \param modelView
     */
    void updateBoundingBox(INode *root, stack<glm::mat4>& modelView){
        root->updateBoundingBox(*this, modelView);
    }


    /**
     * Begin rendering of the scene graph from the root
     * \param root
     * \param modelView
     */
    void draw(INode *root, stack<glm::mat4>& modelView)
    {
        root->draw(*this,modelView);
    }

//    void drawLight(INode *root, stack<glm::mat4>& _mv)
//    {
//        std::vector<util::Light> lights;
//        root->drawLight(*this, _mv, lights);
//    }

    void dispose()
    {
        for (map<string,util::ObjectInstance *>::iterator it=meshRenderers.begin();
             it!=meshRenderers.end();it++)
          {
            it->second->cleanup(*glContext);
          }
    }
    /**
     * Draws a specific mesh.
     * If the mesh has been added to this renderer, it delegates to its correspond mesh renderer
     * This function first passes the material to the shader. Currently it uses the shader variable
     * "vColor" and passes it the ambient part of the material. When lighting is enabled, this method must
     * be overriden to set the ambient, diffuse, specular, shininess etc. values to the shader
     * \param name
     * \param material
     * \param transformation
     */
    void drawMesh(const string& name,
                  const util::Material& material,
                  const string& textureName,
                  const glm::mat4& transformation,
                  const glm::mat4& texturematrix)
    {
        if (meshRenderers.count(name)==1)
        {
            /*Material properties are now being sent to the shader*/
            glContext->glUniform3fv(shaderLocations.getLocation("material.ambient"), 1, glm::value_ptr(material.getAmbient()));
            glContext->glUniform3fv(shaderLocations.getLocation("material.diffuse"), 1, glm::value_ptr(material.getDiffuse()));
            glContext->glUniform3fv(shaderLocations.getLocation("material.specular"), 1,glm::value_ptr(material.getSpecular()));
            glContext->glUniform1f(shaderLocations.getLocation("material.shininess"), material.getShininess());

            /*Sending the final modelview matrix which contains all the transformations from the root of the scenegraph to the respective
              leaf node.*/
            int loc = shaderLocations.getLocation("modelview");
            if (loc<0)
                throw runtime_error("No shader variable for \" modelview \"");
            glContext->glUniformMatrix4fv(loc,1,false,glm::value_ptr(transformation));

            /*Creating and sending the normalmatrix the shader requires. The normal matrix is the inverse-transpose of the final modelview matrix.*/
            glm::mat4 normalmatrix = glm::inverse(glm::transpose((transformation)));
            glContext->glUniformMatrix4fv(shaderLocations.getLocation("normalmatrix"), 1, false,glm::value_ptr(normalmatrix));

            // Texture shader
            loc = shaderLocations.getLocation("image");
            //enable texture mapping
            glContext->glEnable(GL_TEXTURE_2D);
            glContext->glActiveTexture(GL_TEXTURE0);//up to 8 textures can be used.
            //tell the shader to look for GL_TEXTURE"0"
            glContext->glUniform1i(loc, 0);

            bool hasTexture = false;
            util::TextureImage *image;

            loc = shaderLocations.getLocation("texturematrix");
            glContext->glUniformMatrix4fv(loc, 1, false, glm::value_ptr(texturematrix));

            //cout << "Texture Size : " << textures.size() << endl;
            if (textures.find(textureName) != textures.end())
            {
                image = textures[textureName];
                hasTexture = true;
            }

            if (hasTexture == false)
            {
                // using white textures;
            }
            else
            {
                if(textureName == "white")
                {
                    //cout << "texture is white!\n";
                    loc = shaderLocations.getLocation("existImage");
                    glContext->glUniform1i(loc, 0);
                }
                else
                {
                    loc = shaderLocations.getLocation("existImage");
                    glContext->glUniform1i(loc, 1);
                }
                image->getTexture()->setWrapMode(QOpenGLTexture::Repeat);
                image->getTexture()->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
                image->getTexture()->bind();
            }
            meshRenderers[name]->draw(*glContext);
        }
    }

    /**
     * Queries the shader program for all variables and locations, and adds them to itself
     * \param shaderProgram
     */
    void initShaderProgram(util::ShaderProgram& shaderProgram, map<string,string>& shaderVarsToVertexAttribs)
    {
        if (glContext==NULL)
          throw runtime_error("No context set");

        shaderLocations = shaderProgram.getAllShaderVariables(*glContext);
        this->shaderVarsToVertexAttribs = shaderVarsToVertexAttribs;
        shaderLocationsSet = true;

    }

    int getShaderLocation(const string& name)
    {
        return shaderLocations.getLocation(name);
    }

    util::TextureImage getTextureImage(const string& textureName)
    {
        if (textures.find(textureName) != textures.end())
            return *textures[textureName];
        else
            throw runtime_error("texture not found in scene renderer.\n");
    }
};
}
#endif
