
/**
 * \file SWGLUtility.h
 * \brief defines utility qt opengl functions
 * \author Florian Lance
 * \date 25/11/13
 */

#ifndef _SWGLUTILITY_
#define _SWGLUTILITY_

#include "commonTypes.h"
#include "swExceptions.h"
#include <QGLBuffer>
#include <QGLShaderProgram>


/**
 * \brief Initialize the input QGL buffer.
 * \param [out] oBuffer  : buffer to be initialized.
 */
static void initVertexBuffer(QGLBuffer &oBuffer)
{
    oBuffer = QGLBuffer(QGLBuffer::VertexBuffer);

    if(!oBuffer.create())
    {
        qWarning() << "Error initVertexBuffer : QGLBuffer cannot be created. ";
        throw swExcept::swBufferGLError();
    }
}

/**
 * \brief Initialize the input QGL buffer.
 * \param [out] oBuffer  : buffer to be initialized.
 */
static void initIndexBuffer(QGLBuffer &oBuffer)
{
    oBuffer = QGLBuffer(QGLBuffer::IndexBuffer);

    if(!oBuffer.create())
    {
        qWarning() << "Error initIndexBuffer : QGLBuffer cannot be created. ";
        throw swExcept::swBufferGLError();
    }
}

/**
 * \brief Allocates the input QGL buffer.
 * \param [in,out] oBuffer  : buffer to be allocated.
 * \param [in] pData        : data
 * \param [in] i32SizeData  : size of the data
 */
static void allocateBuffer(QGLBuffer &oBuffer, const void * pData, cint i32SizeData)
{
    if(!oBuffer.bind())
    {
        qWarning() << "Error allocateBuffer : QGLBuffer not binded. ";
        throw swExcept::swBufferGLError();
    }

    oBuffer.allocate(pData, i32SizeData);
}

/**
 * \brief Check if gl shader is linked.
 * \param [in,out] oShader : input gl shader
 * \throw swShaderGLError()
 */
static void checkShader(QGLShaderProgram &oShader)
{
    if (!oShader.isLinked())
    {
        qWarning() << "Error checkShader : QGLShaderProgram not linked. ";
        throw swExcept::swShaderGLError();
    }
}

/**
* \brief Check if gl buffer is created and allocated.
* \param [in,out] oBuffer : input gl buffer
* \throw swBufferGLError()
*/
static void checkBuffer(QGLBuffer &oBuffer)
{
    if(!oBuffer.isCreated())
    {
        qWarning() << "Error checkBuffer : QGLBuffer not created. ";
        throw swExcept::swBufferGLError();
    }
    else if(oBuffer.size() == 0)
    {
        qWarning() << "Error checkBuffer : QGLBuffer not allocated. ";
        throw swExcept::swBufferGLError();
    }
}

/**
* \brief Bind the vertex buffer and add attribute to the shader
* \param [in,out] oBuffer : vertexBuffer to be binded
* \param [in,out] oShader : oShader
*/
static void bindVertexBuffer(QGLBuffer &vertexBuffer, QGLShaderProgram &oShader)
{
    checkBuffer(vertexBuffer);

    vertexBuffer.bind();
    oShader.enableAttributeArray("vertex");
    oShader.setAttributeBuffer("vertex", GL_FLOAT, 0, 3 );
}

/**
* \brief Bind the color buffer and add attribute to the shader
* \param [in,out] colorBuffer : colorBuffer to be binded
* \param [in,out] oShader : oShader
*/
static void bindColorBuffer(QGLBuffer &colorBuffer, QGLShaderProgram &oShader)
{
    checkBuffer(colorBuffer);

    colorBuffer.bind();
    oShader.enableAttributeArray("color");
    oShader.setAttributeBuffer("color", GL_FLOAT, 0, 3 );
}

/**
* \brief Bind the texture buffer and add attribute to the shader
* \param [in,out] textureBuffer : textureBuffer to be binded
* \param [in,out] oShader : oShader
*/
static void bindTextureBuffer(QGLBuffer &textureBuffer, QGLShaderProgram &oShader)
{
    checkBuffer(textureBuffer);

    textureBuffer.bind();
    oShader.enableAttributeArray("texture_coord");
    oShader.setAttributeBuffer("texture_coord", GL_FLOAT, 0, 2);
}

/**
* \brief Bind the normal buffer and add attribute to the shader
* \param [in,out] normalBuffer : normalBuffer to be binded
* \param [in,out] oShader : oShader
*/
static void bindNormalBuffer(QGLBuffer &normalBuffer, QGLShaderProgram &oShader)
{
    checkBuffer(normalBuffer);

    normalBuffer.bind();
    oShader.enableAttributeArray("normal");
    oShader.setAttributeBuffer("normal", GL_FLOAT, 0, 3);
}

// TODO : finish doc
/**
 * \brief ...
 * \param [in] indexBuffer          : ...
 * \param [in] vertexBuffer         : ...
 * \param [in,out] oShader          : ...
 * \param [in] oPrimitiveToRender   : ...
 */
static GLenum drawBuffer(QGLBuffer &indexBuffer, QGLBuffer &vertexBuffer, QGLShaderProgram &oShader, GLenum oPrimitiveToRender)
{
    checkShader(oShader);
    bindVertexBuffer(vertexBuffer, oShader);

    indexBuffer.bind();
    glDrawElements(oPrimitiveToRender, indexBuffer.size(), GL_UNSIGNED_INT, 0);

    return glGetError();
}

/**
 * \brief ...
 * \param [in] indexBuffer          : ...
 * \param [in] vertexBuffer         : ...
 * \param [in] normalBuffer         : ...
 * \param [in,out] oShader          : ...
 * \param [in] oPrimitiveToRender   : ...
 */
static GLenum drawBuffer(QGLBuffer &indexBuffer, QGLBuffer &vertexBuffer, QGLBuffer &normalBuffer, QGLShaderProgram &oShader, GLenum oPrimitiveToRender)
{
    checkShader(oShader);
    bindVertexBuffer(vertexBuffer, oShader);
    bindNormalBuffer(normalBuffer, oShader);

    indexBuffer.bind();
    glDrawElements(oPrimitiveToRender, indexBuffer.size(), GL_UNSIGNED_INT, 0);

    return glGetError();
}

/**
* \brief ...
* \param [in] indexBuffer          : ...
* \param [in] vertexBuffer         : ...
* \param [in] colorBuffer          : ...
* \param [in,out] oShader          : ...
* \param [in] oPrimitiveToRender   : ...
*/
static GLenum drawBufferWithColor(QGLBuffer &indexBuffer, QGLBuffer &vertexBuffer, QGLBuffer &colorBuffer,
                            QGLShaderProgram &oShader, GLenum oPrimitiveToRender)
{
    checkShader(oShader);
    bindVertexBuffer(vertexBuffer, oShader);
    bindColorBuffer(colorBuffer, oShader);

    indexBuffer.bind();
    glDrawElements(oPrimitiveToRender, indexBuffer.size(), GL_UNSIGNED_INT, 0);

    return glGetError();
}

/**
* \brief ...
* \param [in] indexBuffer          : ...
* \param [in] vertexBuffer         : ...
* \param [in] colorBuffer          : ...
* \param [in] normalBuffer         : ...
* \param [in,out] oShader          : ...
* \param [in] oPrimitiveToRender   : ...
*/
static GLenum drawBufferWithColor(QGLBuffer &indexBuffer, QGLBuffer &vertexBuffer, QGLBuffer &colorBuffer, QGLBuffer &normalBuffer,
                            QGLShaderProgram &oShader, GLenum oPrimitiveToRender)
{
    checkShader(oShader);
    bindVertexBuffer(vertexBuffer, oShader);
    bindColorBuffer(colorBuffer, oShader);
    bindNormalBuffer(normalBuffer, oShader);

    indexBuffer.bind();
    glDrawElements(oPrimitiveToRender, indexBuffer.size(), GL_UNSIGNED_INT, 0);

    return glGetError();
}

/**
 * \brief ...
 * \param [in] indexBuffer          : ...
 * \param [in] vertexBuffer         : ...
 * \param [in] textureBuffer        : ...
 * \param [in,out] oShader          : ...
 * \param [in] oPrimitiveToRender   : ...
 */
static GLenum drawBufferWithTexture(QGLBuffer &indexBuffer, QGLBuffer &vertexBuffer, QGLBuffer &textureBuffer,
                                    QGLShaderProgram &oShader, GLenum oPrimitiveToRender)
{
    checkShader(oShader);
    bindVertexBuffer(vertexBuffer, oShader);
    bindTextureBuffer(textureBuffer, oShader);

    indexBuffer.bind();
    glDrawElements(oPrimitiveToRender, indexBuffer.size(), GL_UNSIGNED_INT, 0);

    return glGetError();
}


/**
 * \brief ...
 * \param [in] indexBuffer          : ...
 * \param [in] vertexBuffer         : ...
 * \param [in] textureBuffer        : ...
 * \param [in] normalBuffer         : ...
 * \param [in,out] oShader          : ...
 * \param [in] oPrimitiveToRender   : ...
 */
static GLenum drawBufferWithTexture(QGLBuffer &indexBuffer, QGLBuffer &vertexBuffer, QGLBuffer &textureBuffer, QGLBuffer &normalBuffer,
                                    QGLShaderProgram &oShader, GLenum oPrimitiveToRender)
{
    checkShader(oShader);
    bindVertexBuffer(vertexBuffer, oShader);
    bindTextureBuffer(textureBuffer, oShader);
    bindNormalBuffer(normalBuffer, oShader);

    indexBuffer.bind();
    glDrawElements(oPrimitiveToRender, indexBuffer.size(), GL_UNSIGNED_INT, 0);

    return glGetError();
}


#endif
