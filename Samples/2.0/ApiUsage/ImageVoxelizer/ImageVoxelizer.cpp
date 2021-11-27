
#include "GraphicsSystem.h"
#include "ImageVoxelizerGameState.h"

#include "Compositor/OgreCompositorManager2.h"
#include "OgreConfigFile.h"
#include "OgreRoot.h"
#include "OgreWindow.h"

// Declares WinMain / main
#include "MainEntryPointHelper.h"
#include "System/MainEntryPoints.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMainApp( HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR strCmdLine, INT nCmdShow )
#else
int mainApp( int argc, const char *argv[] )
#endif
{
    return Demo::MainEntryPoints::mainAppSingleThreaded( DEMO_MAIN_ENTRY_PARAMS );
}

namespace Demo
{
    class ImageVoxelizerGraphicsSystem : public GraphicsSystem
    {
        virtual Ogre::CompositorWorkspace *setupCompositor()
        {
            Ogre::CompositorManager2 *compositorManager = mRoot->getCompositorManager2();
            return compositorManager->addWorkspace( mSceneManager, mRenderWindow->getTexture(), mCamera,
                                                    "PbsMaterialsWorkspace", true );
        }

        virtual void setupResources( void )
        {
            GraphicsSystem::setupResources();

            Ogre::ConfigFile cf;
            cf.load( mResourcePath + "resources2.cfg" );

            Ogre::String originalDataFolder = cf.getSetting( "DoNotUseAsResource", "Hlms", "" );

            if( originalDataFolder.empty() )
                originalDataFolder = "./";
            else if( *( originalDataFolder.end() - 1 ) != '/' )
                originalDataFolder += "/";

            const char *c_locations[] = { "Compute/Tools",
                                          "Compute/Tools/Any",
                                          "Compute/Tools/GLSL",
                                          "Compute/Tools/HLSL",
                                          "Compute/Tools/Metal",
                                          "Compute/Algorithms/IrradianceFields",
                                          "Compute/Algorithms/IrradianceFields/Visualizer",
                                          "VCT",
                                          "VCT/ImageVoxelizer",
                                          "VCT/Visualizer",
                                          "Hlms/Pbs/Any",
                                          "2.0/scripts/materials/Postprocessing/SceneAssets" };

            for( size_t i = 0; i < sizeof( c_locations ) / sizeof( c_locations[0] ); ++i )
            {
                Ogre::String dataFolder = originalDataFolder + c_locations[i];
                addResourceLocation( dataFolder, getMediaReadArchiveType(), "General" );
            }
        }

    public:
        ImageVoxelizerGraphicsSystem( GameState *gameState ) : GraphicsSystem( gameState )
        {
            // REMOVE ME: Temporary while I'm debugging to speed up dev iteration
            mAlwaysAskForConfig = false;
        }
    };

    void MainEntryPoints::createSystems( GameState **outGraphicsGameState,
                                         GraphicsSystem **outGraphicsSystem,
                                         GameState **outLogicGameState, LogicSystem **outLogicSystem )
    {
        ImageVoxelizerGameState *gfxGameState = new ImageVoxelizerGameState( "" );

        ImageVoxelizerGraphicsSystem *graphicsSystem = new ImageVoxelizerGraphicsSystem( gfxGameState );

        gfxGameState->_notifyGraphicsSystem( graphicsSystem );

        *outGraphicsGameState = gfxGameState;
        *outGraphicsSystem = graphicsSystem;
    }

    void MainEntryPoints::destroySystems( GameState *graphicsGameState, GraphicsSystem *graphicsSystem,
                                          GameState *logicGameState, LogicSystem *logicSystem )
    {
        delete graphicsSystem;
        delete graphicsGameState;
    }

    const char *MainEntryPoints::getWindowTitle( void )
    {
        return "CIVCT: Cascaded Image Voxel Cone Tracing (Real Time Global Illumination Technique)";
    }
}  // namespace Demo
