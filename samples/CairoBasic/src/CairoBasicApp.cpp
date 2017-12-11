#include <vector>
using std::vector;

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Path2d.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/ip/Fill.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;

class Flower {
  public:
	Flower( vec2 loc, float radius, float petalOutsideRadius, float petalInsideRadius, int numPetals, ColorA color )
		: mLoc( loc ), mRadius( radius ), mPetalOutsideRadius( petalOutsideRadius ), mPetalInsideRadius( petalInsideRadius ), mNumPetals( numPetals ), mColor( color )
	{}

	void makePath( cairo::Context &ctx ) const
	{
		for( int petal = 0; petal < mNumPetals; ++petal ) {
			ctx.newSubPath();
			float petalAngle = ( petal / (float)mNumPetals ) * 2 * M_PI;
			vec2 outsideCircleCenter = mLoc + vec2( 1, 0 ) * (float)cos( petalAngle ) * mRadius + vec2( 0, 1 ) * (float)sin( petalAngle ) * mRadius;
			vec2 insideCircleCenter = mLoc + vec2( 1, 0 ) * (float)cos( petalAngle ) * mPetalInsideRadius + vec2( 0, 1 ) * (float)sin( petalAngle ) * mPetalInsideRadius;
			ctx.arc( outsideCircleCenter, mPetalOutsideRadius, petalAngle + M_PI / 2 + M_PI, petalAngle + M_PI / 2 );
			ctx.arc( insideCircleCenter, mPetalInsideRadius, petalAngle + M_PI / 2, petalAngle + M_PI / 2 + M_PI );
			ctx.closePath();
		}		
	}
	
	void draw( cairo::Context &ctx ) const
	{
		// draw the solid petals
		ctx.setSource( mColor );
		makePath( ctx );
		ctx.fill();
		
		// draw the petal outlines
		ctx.setSource( mColor * 0.8f );
		makePath( ctx );
		ctx.stroke();
	};
	
  private:
	vec2		mLoc;
	float		mRadius, mPetalOutsideRadius, mPetalInsideRadius;
	int			mNumPetals;
	ColorA		mColor;
};

class CairoBasicApp : public App {
public:
  void setup();
	void mouseDown( MouseEvent event );
	void keyDown( KeyEvent event );
	void renderScene( cairo::Context &ctx );
	void draw();
	
public:
	vector<Flower>		mFlowers;
  cairo::SurfaceImage srf;
  gl::TextureRef srfRef;
};

void CairoBasicApp::setup()
{
  srf = cairo::SurfaceImage( 640, 480, true );
  cairo::Context ctx(srf);
	renderScene( ctx );
  srfRef=gl::Texture::create(srf.getSurface());
}

void CairoBasicApp::mouseDown( MouseEvent event )
{	
	// create a new flower
	float radius = randFloat( 60, 90 );
	int numPetals = randInt( 6, 50 );
	float outerRadius = ( 2 * M_PI * radius ) / numPetals / 2 * randFloat( 0.9f, 1.0f );
	float innerRadius = outerRadius * randFloat( 0.2f, 0.4f );
	mFlowers.push_back( Flower( event.getPos(), radius, outerRadius, innerRadius, numPetals, ColorA( CM_HSV, randFloat(), 1, 1, 0.65f ) ) );
  cairo::Context ctx(srf);
  mFlowers.back().draw(ctx);
	// renderScene( ctx );
  srfRef=gl::Texture::create(srf.getSurface());
}

void CairoBasicApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_f ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getChar() == 'x' ) {
		mFlowers.clear();
    cairo::Context ctx(srf);
    renderScene( ctx );
    srfRef=gl::Texture::create(srf.getSurface());
	}
	else if( event.getChar() == 's' ) {
		cairo::Context ctx( cairo::SurfaceSvg( getHomeDirectory() / "CairoBasicShot.svg", getWindowWidth(), getWindowHeight() ) );
		renderScene( ctx );
	}
	else if( event.getChar() == 'e' ) {
		cairo::Context ctx( cairo::SurfaceEps( getHomeDirectory() / "CairoBasicShot.eps", getWindowWidth(), getWindowHeight() ) );
		renderScene( ctx );
	}
	else if( event.getChar() == 'p' ) {
		cairo::Context ctx( cairo::SurfacePs( getHomeDirectory() / "CairoBasicShot.ps", getWindowWidth(), getWindowHeight() ) );
		renderScene( ctx );
	}	
	else if( event.getChar() == 'd' ) {
		cairo::Context ctx( cairo::SurfacePdf( getHomeDirectory() / "CairoBasicShot.pdf", getWindowWidth(), getWindowHeight() ) );
		renderScene( ctx );
	}	
}

void CairoBasicApp::renderScene( cairo::Context &ctx )
{
	// clear the context with our radial gradient
	cairo::GradientRadial radialGrad( getWindowCenter(), 0, getWindowCenter(), getWindowWidth() );
	radialGrad.addColorStop( 0, Color( 1, 1, 1 ) );
	radialGrad.addColorStop( 1, Color( 0.6, 0.6, 0.6 ) );	
	ctx.setSource( radialGrad );	
	ctx.paint();
	
	for( vector<Flower>::const_iterator flIt = mFlowers.begin(); flIt != mFlowers.end(); ++flIt )
		flIt->draw( ctx );
}

void CairoBasicApp::draw()
{
	// render the scene straight to the window
	// cairo::Context ctx( cairo::createWindowSurface() );	
  // cairo::SurfaceImage srf = cairo::SurfaceImage( 640, 480, true );
  // cairo::Context ctx(srf);
	// renderScene( ctx );
  gl::draw(srfRef);
  // gl::draw(gl::Texture(srf.getSurface()));// create surface
}

CINDER_APP( CairoBasicApp, RendererGl )
// CINDER_APP( CairoBasicApp, Renderer2d )
