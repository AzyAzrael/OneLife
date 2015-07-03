#include "Picker.h"

#include "minorGems/game/Font.h"
#include "minorGems/game/drawUtils.h"

#include "minorGems/util/stringUtils.h"
#include "minorGems/util/SettingsManager.h"

#include "minorGems/graphics/converters/PNGImageConverter.h"
#include "minorGems/graphics/converters/TGAImageConverter.h"

#include "minorGems/io/file/FileInputStream.h"



#define PER_PAGE 5

extern Font *mainFont;
extern Font *smallFont;


Picker::Picker( Pickable *inPickable, double inX, double inY )
        : PageComponent( inX, inY ),
          mPickable( inPickable ),
          mSkip( 0 ),
          mResults( NULL ),
          mNumResults( 0 ),
          mNextButton( smallFont, +60, -290, "Next" ), 
          mPrevButton( smallFont, -60, -290, "Prev" ), 
          mSearchField( mainFont, 
                        0,  100, 4,
                        false,
                        "", NULL, " " ),
          mSelectionIndex( -1 ) {

    addComponent( &mNextButton );
    addComponent( &mPrevButton );
    addComponent( &mSearchField );
    
    mNextButton.addActionListener( this );
    mPrevButton.addActionListener( this );
    mSearchField.addActionListener( this );
    
    mSearchField.setFireOnAnyTextChange( true );

    redoSearch();
    }



Picker::~Picker() {
    if( mResults != NULL ) {
        delete [] mResults;
        }
    }



void Picker::redoSearch() {
    char *search = mSearchField.getText();
    
    if( mResults != NULL ) {
        delete [] mResults;
        mResults = NULL;
        }

    int numRemain;
    
    mResults = mPickable->search( search, 
                                  mSkip, 
                                  PER_PAGE, 
                                  &mNumResults, &numRemain );

    
    mPrevButton.setVisible( mSkip > 0 );
    mNextButton.setVisible( numRemain > 0 );
    
    delete [] search;

    int oldSelection = mSelectionIndex;
    mSelectionIndex = -1;

    if( oldSelection != mSelectionIndex ) {
        fireActionPerformed( this );
        }
    }



        
void Picker::actionPerformed( GUIComponent *inTarget ) {
    if( inTarget == &mNextButton ) {
        mSkip += PER_PAGE;
        redoSearch();
        }
    else if( inTarget == &mPrevButton ) {
        mSkip -= PER_PAGE;
        if( mSkip < 0 ) {
            mSkip = 0;
            }
        redoSearch();
        }
    else if( inTarget == &mSearchField ) {
        mSkip = 0;
        redoSearch();
        }
    }




void Picker::draw() {
    setDrawColor( 0.75, 0.75, 0.75, 1 );
    
    doublePair bgPos = { 0, -85 };
    
    drawRect( bgPos, 80, 160 );
    


    if( mResults != NULL ) {
        doublePair pos = { -50, 40 };
        
        
        for( int i=0; i<mNumResults; i++ ) {
            if( i == mSelectionIndex ) {
                setDrawColor( 1, 1, 1, 1 );
                doublePair selPos = pos;
                selPos.x = 0;
                drawRect( selPos, 80, 32 );
                }

            setDrawColor( 1, 1, 1, 1 );
            mPickable->draw( mResults[i], pos );
            
            doublePair textPos = pos;
            textPos.x += 52;
            
            setDrawColor( 0, 0, 0, 1 );
            smallFont->drawString( mPickable->getText( mResults[i] ), 
                                   textPos, alignLeft );
            

            pos.y -= 64;
            }
        }
    
    }

        

void Picker::pointerUp( float inX, float inY ) {
    if( inX > -80 && inX < 80 &&
        inY < 75 && inY > -245 ) {
        
        inY -= 40;
        
        inY *= -1;
        
        inY += 32;
        
        int oldSelection = mSelectionIndex;
        
 
        mSelectionIndex = (int)( inY / 64 );
        
        if( mSelectionIndex >= mNumResults ) {
            mSelectionIndex = -1;
            }
        if( mSelectionIndex < 0 ) {
            mSelectionIndex = -1;
            }

        
        if( oldSelection != mSelectionIndex ) {
            fireActionPerformed( this );
            }
        }
    }



int Picker::getSelectedObject() {
    if( mSelectionIndex == -1 ) {
        return -1;
        }
    
    return mPickable->getID( mResults[mSelectionIndex] );
    }



void Picker::unselectObject() {
    mSelectionIndex = -1;
    }
