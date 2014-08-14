
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
 * @brief checkGlError : check gl error
 * @param [in] bDisplay : display all gl errors
 * @return return the last gl error
 */
static GLenum checkGlError(bool bDisplay = true)
{
   QString l_sSError;

    GLenum l_glError;

    while((l_glError = glGetError()) != GL_NO_ERROR)
    {
        std::cout << l_glError << " ";
        switch(l_glError)
        {
            case GL_INVALID_ENUM :
                l_sSError = QString("GL_INVALID_ENUM");
            break;
            case GL_INVALID_VALUE :
                l_sSError = QString("GL_INVALID_VALUE");
            break;
            case GL_INVALID_OPERATION :
                l_sSError = QString("GL_INVALID_OPERATION");
            break;
            case GL_INVALID_FRAMEBUFFER_OPERATION :
                l_sSError = QString("GL_INVALID_FRAMEBUFFER_OPERATION");
            break;
            case GL_OUT_OF_MEMORY :
                l_sSError = QString("GL_OUT_OF_MEMORY");
            break;
            case GL_STACK_UNDERFLOW :
                l_sSError = QString("GL_STACK_UNDERFLOW");
            break;
            case GL_STACK_OVERFLOW :
                l_sSError = QString("GL_STACK_OVERFLOW");
            break;
            default :
                l_sSError = QString("GL_UNKNOW_ERROR");
            break;
        }

        if(bDisplay)
        {
            qWarning() << "GL Error : " << l_sSError;
        }
    }

    return l_glError;
}


/**
 * \brief Initialize the input QGL buffer.
 * \param [out] oBuffer  : buffer to be initialized.
 */
static void initVertexBuffer(QGLBuffer &oBuffer)
{
    oBuffer = QGLBuffer(QGLBuffer::VertexBuffer);

    oBuffer.create();
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "initVertexBuffer ->  oBuffer.create()";
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
        qWarning() << "initIndexBuffer -> error creating buffer. ";
    }

    if(checkGlError(true) != GL_NO_ERROR) qWarning() << "initIndexBuffer ->  oBuffer.create()";
}

/**
 * \brief Allocates the input QGL buffer.
 * \param [in,out] oBuffer  : buffer to be allocated.
 * \param [in] pData        : data
 * \param [in] i32SizeData  : size of the data
 */
static void allocateBuffer(QGLBuffer &oBuffer, const void * pData, cint i32SizeData)
{
    oBuffer.bind();
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "allocateBuffer -> oBuffer.bind()";
    oBuffer.allocate(pData, i32SizeData);
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "allocateBuffer -> oBuffer.allocate(pData, i32SizeData) " << oBuffer.size() << " " << oBuffer.bufferId() ;
    oBuffer.release();
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "allocateBuffer -> oBuffer.release()";
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

    if(checkGlError(true) != GL_NO_ERROR) qWarning() << "checkBuffer ";
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

    if(checkGlError(true) != GL_NO_ERROR) qWarning() << "checkBuffer ";
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
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindVertexBuffer  -> vertexBuffer.bind()";
    oShader.enableAttributeArray("vertex");
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindVertexBuffer  -> oShader.enableAttributeArray()";
    oShader.setAttributeBuffer("vertex", GL_FLOAT, 0, 3 );
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindVertexBuffer  -> oShader.setAttributeBuffer()";
    vertexBuffer.release();
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindVertexBuffer  -> vertexBuffer.release()";
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
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindColorBuffer  -> colorBuffer.bind()";
    oShader.enableAttributeArray("color");
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindColorBuffer  -> oShader.enableAttributeArray()";
    oShader.setAttributeBuffer("color", GL_FLOAT, 0, 3 );
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindColorBuffer  -> oShader.setAttributeBuffer()";
    colorBuffer.release();
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindColorBuffer  -> colorBuffer.release()";
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
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindTextureBuffer  -> textureBuffer.bind()";
    oShader.enableAttributeArray("texture_coord");
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindTextureBuffer  -> oShader.enableAttributeArray()";
    oShader.setAttributeBuffer("texture_coord", GL_FLOAT, 0, 2);
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindTextureBuffer  -> oShader.setAttributeBuffer()";
    textureBuffer.release();
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindTextureBuffer  ->textureBuffer.release()";
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
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindNormalBuffer  -> normalBuffer.bind()";
    oShader.enableAttributeArray("normal");
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindNormalBuffer -> oShader.enableAttributeArray()";
    oShader.setAttributeBuffer("normal", GL_FLOAT, 0, 3);
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindNormalBuffer -> oShader.setAttributeBuffer()";
    normalBuffer.release();
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "bindNormalBuffer -> normalBuffer.release()";
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
//    std::cout <<"checkShader ";
    checkShader(oShader);
//    std::cout <<"bindVertexBuffer ";
    bindVertexBuffer(vertexBuffer, oShader);
//    std::cout <<"indexBuffer ";
    indexBuffer.bind();
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "drawBuffer 4 -> indexBuffer.bind()";
//    std::cout <<"glDrawElements ";
    glDrawElements(oPrimitiveToRender, indexBuffer.size(), GL_UNSIGNED_INT, 0);
//    std::cout <<"release ";
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "drawBuffer 4 -> glDrawElements";
    indexBuffer.release();
//    std::cout <<"end drawBuffer ";

    return checkGlError(true);
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
//    std::cout <<"checkShader ";
    checkShader(oShader);
//    std::cout <<"bindVertexBuffer ";
    bindVertexBuffer(vertexBuffer, oShader);
//    std::cout <<"bindNormalBuffer ";
    bindNormalBuffer(normalBuffer, oShader);

//    std::cout <<"indexBuffer ";
    indexBuffer.bind();
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "drawBuffer 5 -> indexBuffer.bind()";
//    std::cout <<"glDrawElements ";
    glDrawElements(oPrimitiveToRender, indexBuffer.size(), GL_UNSIGNED_INT, 0);
//        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "drawBuffer 5 -> glDrawElements";
//    std::cout <<"release ";
    indexBuffer.release();

//    std::cout <<"end drawBuffer ";

    return checkGlError(true);
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
         if(checkGlError(true) != GL_NO_ERROR) qWarning() << "drawBufferWithColor 5 -> indexBuffer.bind()";
    glDrawElements(oPrimitiveToRender, indexBuffer.size(), GL_UNSIGNED_INT, 0);
         if(checkGlError(true) != GL_NO_ERROR) qWarning() << "drawBufferWithColor 5 -> glDrawElements";
    indexBuffer.release();
    GLenum l_oLastError = checkGlError(true);
         if(l_oLastError != GL_NO_ERROR) qWarning() << "drawBufferWithColor 5 -> indexBuffer.release()";

    return l_oLastError;
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
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "drawBufferWithColor 6 -> indexBuffer.bind()";
    glDrawElements(oPrimitiveToRender, indexBuffer.size(), GL_UNSIGNED_INT, 0);
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "drawBufferWithColor 6 -> glDrawElements";
    indexBuffer.release();
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "drawBufferWithColor 6 -> indexBuffer.release()";

    return checkGlError(true);;
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
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "drawBufferWithTexture 5 -> indexBuffer.bind()";
    glDrawElements(oPrimitiveToRender, indexBuffer.size(), GL_UNSIGNED_INT, 0);
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "drawBufferWithTexture 5 -> glDrawElements";
    indexBuffer.release();

    return checkGlError(true);;
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
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "drawBufferWithTexture 6 -> indexBuffer.bind()";
    glDrawElements(oPrimitiveToRender, indexBuffer.size(), GL_UNSIGNED_INT, 0);
        if(checkGlError(true) != GL_NO_ERROR) qWarning() << "drawBufferWithTexture 6 -> glDrawElements " << indexBuffer.size();
    indexBuffer.release();

    return checkGlError(true);
}


#endif
