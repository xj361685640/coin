/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2005 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See <URL:http://www.coin3d.org/> for more information.
 *
 *  Systems in Motion, Postboks 1283, Pirsenteret, 7462 Trondheim, NORWAY.
 *  <URL:http://www.sim.no/>.
 *
\**************************************************************************/

/*!
  \class SoGLCubeMapImage include/Inventor/misc/SoGLCubeMapImage.h
  \brief The SoGLCubeMapImage class is used to handle cube map textures.

  \COIN_CLASS_EXTENSION

  \since Coin 3.0
*/

// *************************************************************************

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <Inventor/C/threads/storage.h>
#include <Inventor/C/tidbits.h>
#include <Inventor/SbImage.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoGLDisplayList.h>
#include <Inventor/elements/SoGLTextureImageElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoGL.h>
#include <Inventor/misc/SoGLCubeMapImage.h>
#include <Inventor/system/gl.h>
#include <Inventor/lists/SbList.h>
#include <Inventor/C/glue/gl.h>
#include <Inventor/C/glue/glp.h>
#include <Inventor/misc/SoContextHandler.h>


#ifdef COIN_THREADSAFE
#include <Inventor/threads/SbMutex.h>
#endif // COIN_THREADSAFE

// *************************************************************************

static GLenum get_gltarget(SoGLCubeMapImage::Target target)
{
  GLenum ret; 
  switch (target) {
  default:
  case SoGLCubeMapImage::POSITIVE_X:
    ret = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    break;
  case SoGLCubeMapImage::NEGATIVE_X:
    ret = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
    break;
  case SoGLCubeMapImage::POSITIVE_Y:
    ret = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
    break;    
  case SoGLCubeMapImage::NEGATIVE_Y:
    ret = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
    break;
  case SoGLCubeMapImage::POSITIVE_Z:
    ret = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
    break;
  case SoGLCubeMapImage::NEGATIVE_Z:
    ret = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    break;
  }
  return ret;
}

class SoGLCubeMapImageP {
public:
  SoGLCubeMapImageP(void) { 
    for (int i = 0; i < 6; i++) {
      this->image[i] = NULL;
    }
  }
  ~SoGLCubeMapImageP() { }

  class dldata {
  public:
    dldata(void)
      : dlist(NULL), age(0) { }
    dldata(SoGLDisplayList *dl)
      : dlist(dl),
        age(0) { }
    dldata(const dldata & org)
      : dlist(org.dlist),
        age(org.age) { }
    SoGLDisplayList * dlist;
    uint32_t age;
  };
  
  SoGLDisplayList * findDL(SoState *state) {
    int currcontext = SoGLCacheContextElement::get(state);
    int i, n = this->dlists.getLength();
    SoGLDisplayList *dl;
    for (i = 0; i < n; i++) {
      dl = this->dlists[i].dlist;
      if (dl->getContext() == currcontext) return dl;
    }
    return NULL;
  }

  SbList <dldata> dlists;

  static SoType classTypeId;
  const SbImage * image[6];

#ifdef COIN_THREADSAFE
  static SbMutex * mutex;
#endif // !COIN_THREADSAFE
  
  inline void lock(void) {
#ifdef COIN_THREADSAFE
    SoGLCubeMapImageP::mutex->lock();
#endif // COIN_THREADSAFE
  }
  inline void unlock(void) {
#ifdef COIN_THREADSAFE
    SoGLCubeMapImageP::mutex->unlock();
#endif // COIN_THREADSAFE
  }

  static void contextCleanup(uint32_t context, void * closure)
  {
    SoGLCubeMapImageP * thisp = (SoGLCubeMapImageP *) closure;
    thisp->lock();
    int n = thisp->dlists.getLength();
    int i = 0;
    
    while (i < n) {
      if (thisp->dlists[i].dlist->getContext() == (int) context) {
        thisp->dlists[i].dlist->unref(NULL);
        thisp->dlists.remove(i);
        n--;
      }
      else i++;
    }
    thisp->unlock();
  }
};

SoType SoGLCubeMapImageP::classTypeId STATIC_SOTYPE_INIT;
#ifdef COIN_THREADSAFE
SbMutex * SoGLCubeMapImageP::mutex = NULL;
#endif // !COIN_THREADSAFE

#define PRIVATE(obj) (obj->pimpl)

// *************************************************************************

/*!
  Constructor.
*/
SoGLCubeMapImage::SoGLCubeMapImage(void)
{
  PRIVATE(this) = new SoGLCubeMapImageP;
  SoContextHandler::addContextDestructionCallback(SoGLCubeMapImageP::contextCleanup, PRIVATE(this));
}

/*!
  Destructor.
*/
SoGLCubeMapImage::~SoGLCubeMapImage()
{
  SoContextHandler::removeContextDestructionCallback(SoGLCubeMapImageP::contextCleanup, PRIVATE(this));
  delete PRIVATE(this);
}

void
SoGLCubeMapImage::unref(SoState * state)
{
  inherited::unref(state);
}

/*!
  \COININTERNAL
*/
void
SoGLCubeMapImage::initClass(void)
{
  assert(SoGLCubeMapImageP::classTypeId.isBad());
  SoGLCubeMapImageP::classTypeId =
    SoType::createType(SoGLImage::getClassTypeId(), SbName("GLCubeMapImage"));
  cc_coin_atexit((coin_atexit_f*)SoGLCubeMapImage::cleanupClass);
}

//
// called by atexit()
//
void
SoGLCubeMapImage::cleanupClass(void)
{
  SoGLCubeMapImageP::classTypeId STATIC_SOTYPE_INIT;
}

// Doc in superclass.
SoType
SoGLCubeMapImage::getClassTypeId(void)
{
  assert(!SoGLCubeMapImageP::classTypeId.isBad());
  return SoGLCubeMapImageP::classTypeId;
}

// Doc in superclass.
SoType
SoGLCubeMapImage::getTypeId(void) const
{
  return SoGLCubeMapImage::getClassTypeId();
}


void 
SoGLCubeMapImage::setCubeMapImage(const Target target,
                                  const SbImage * image)
{
  int idx = (int) target;
  PRIVATE(this)->image[idx] = image;
  
  PRIVATE(this)->lock();
  for (int i = 0; i < PRIVATE(this)->dlists.getLength(); i++) {
    PRIVATE(this)->dlists[i].dlist->unref(NULL);
  }
  PRIVATE(this)->dlists.truncate(0);
  PRIVATE(this)->unlock();

  // FIXME: this is a hack. Just set one of the images in
  // SoGLImage. Needed for rendering to work correctly.
  if (image) {
    this->SoGLImage::setData(image, CLAMP_TO_EDGE, CLAMP_TO_EDGE,
                             0.5f, 0, NULL);
  }
}


void
SoGLCubeMapImage::setData(const SbImage * image,
                          const Wrap wraps,
                          const Wrap wrapt,
                          const float quality,
                          const int border,
                          SoState * createinstate)
{
  assert(0 && "Use setCubeMapImage to set data in SoGLCubeMapImage");
}

void
SoGLCubeMapImage::setData(const SbImage * image,
                          const Wrap wraps,
                          const Wrap wrapt,
                          const Wrap wrapr,
                          const float quality,
                          const int border,
                          SoState * createinstate)
{
  assert(0 && "Use setCubeMapImage to set data in SoGLCubeMapImage");
}

SoGLDisplayList *
SoGLCubeMapImage::getGLDisplayList(SoState * state)
{
  PRIVATE(this)->lock();
  SoGLDisplayList * dl = PRIVATE(this)->findDL(state);
  if (!dl) {
    dl = new SoGLDisplayList(state,
                             SoGLDisplayList::TEXTURE_OBJECT);
    if (dl) {
      dl->setTextureTarget((int) GL_TEXTURE_CUBE_MAP);

      const cc_glglue * glue = sogl_glue_instance(state);
      dl->open(state);

      for (int i = 0; i < 6; i++) {
        const SbImage * img = PRIVATE(this)->image[i];
        if (img && img->hasData()) {
          SbVec2s size;
          int numcomponents;
          unsigned char * bytes = img->getValue(size, numcomponents);
          GLenum format;
          switch (numcomponents) {
          default: // avoid compiler warnings
          case 1: format = GL_LUMINANCE; break;
          case 2: format = GL_LUMINANCE_ALPHA; break;
          case 3: format = GL_RGB; break;
          case 4: format = GL_RGBA; break;
          }

          // FIXME: resize image if not power of two
          glTexImage2D(get_gltarget((Target) i),
                       0, numcomponents, size[0], size[1], 0, 
                       format, GL_UNSIGNED_BYTE, bytes);

        }
      }

      // FIXME: make it possible to configure filter and mipmap on/off
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      
      // FIXME: make it possible to configure wrap modes
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      dl->close(state);
      PRIVATE(this)->dlists.append(SoGLCubeMapImageP::dldata(dl));
    }
  }
  PRIVATE(this)->unlock();
  return dl;
}

