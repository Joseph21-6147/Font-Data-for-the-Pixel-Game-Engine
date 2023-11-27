// Font sprite to font data tool
//
// Joseph21
// October 29, 2023


#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


#define SPRITE_FILENAME "javidx9_nesfont8x8.png"


class Font_Sprite2Data : public olc::PixelGameEngine {

public:
    Font_Sprite2Data() {
        sAppName = "Font_Sprite2Data";
    }

private:
    // put your class variables here
    olc::Sprite *pFontSprite = nullptr;
    std::string sFontFileName = SPRITE_FILENAME;

public:
    bool OnUserCreate() override {

        // initialize your assets here
        pFontSprite = new olc::Sprite( sFontFileName );

        return true;
    }

/* Create font data from a black and white sprite
 * ==============================================
 *
 * The following functions can be used to convert a black and white font sprite file into a
 * data string that can be fed into olc::PixelGameEngine::olc_ConstructFontSheet()
 * This way an alternative native font can be used in combination with the PGE.
 *
 * If you don't update the spacings as well, the calls to DrawStringProp() won't work properly.
 */

    // this function assumes the sprite to have a height that is a multiple of 24 pixels
    void ProcessSpriteColumn( olc::Sprite *pSprite, int nColumn ) {
        int y = 0;
        while (y < pSprite->height) {

            // group 4 bytes into one uint32_t
            uint8_t uBytes[4];
            for (int j = 0; j < 4; j++) {
                uBytes[j] = 0;

                // group 6 pixels into one byte
                for (int i = 0; i < 6; i++) {
                    olc::Pixel curPix = pSprite->GetPixel( nColumn, y + (j * 6) + i );
                    uint8_t uPixVal;
                    if (curPix == olc::BLACK || curPix == olc::BLANK) {
                        uPixVal = 0;
                    } else {
                        uPixVal = 1;
                    }
                    uBytes[j] |= (uPixVal << i);
                }

                // apply offset to put value into printable character range
                uBytes[j] += 48;
            }

            // display for testing
            for (int j = 3; j >= 0; j--) {
                char cDisplayByte = (char)(uBytes[j]);
                std::cout << cDisplayByte;
                // if the char to display is back slash, add another to prevent escape sequence problems
                if (cDisplayByte == '\\') {
                    std::cout << '\\';
                }
            }

            y += 24;
        }
    }

    void ProcessSprite( olc::Sprite *pSprite ) {
        std::cout << "Font sprite size - width = " << pSprite->width << ", height = " << pSprite->height << std::endl;

        // process sprite column by column
        for (int x = 0; x < pSprite->width; x++) {
            if (x % 8 == 0) {
                std::cout << std::endl;
            }
            ProcessSpriteColumn( pSprite, x );
        }
        std::cout << std::endl << std::endl;
    }

/* Create an olc::Sprite from a text file
 * ======================================
 *
 * The following function can be used to convert a text file containing 0 and 1 characters
 * into an olc::Sprite object. The text file is assumed to be 128 characters wide and
 * 48 characters tall.
 *
 * This function can be used to edit font sprite sheets in combination with the excel
 * tool I created for this purpose.
 */

    olc::Sprite *TextfileToSprite( std::string &sFileName ) {
        // create new sprite, hardcoded size here!!
        olc::Sprite *pSprite = new olc::Sprite( 128, 48 );
        // for debugging, colour every pixel magenta
        for (int y = 0; y < pSprite->height; y++) {
            for (int x = 0; x < pSprite->width; x++) {
                pSprite->SetPixel( x, y, olc::MAGENTA );
            }
        }

        std::ifstream dataFileStream( sFileName );
        std::string token;

        int nRow = 0;
        while (getline( dataFileStream, token )) {
            for (int nCol = 0; nCol < (int)token.length(); nCol++) {

                olc::Pixel p;
                switch (token[nCol]) {
                    case '0': p = olc::BLACK; break;
                    case '1': p = olc::WHITE; break;
                    default:  p = olc::MAGENTA;
                }
                pSprite->SetPixel( nCol, nRow, p );
            }

            nRow += 1;
        }
        dataFileStream.close();

        return pSprite;
    }

/* Create font spacings data from an olc::Sprite
 * =============================================
 *
 * The following functions can be used to create the spacings data for a 16 x 8 font sprite,
 * where each character is considered to be 8x8 pixels in size.
 *
 * See the notes how these spacings are calculated. This spacings data should be used if you
 * use an alternative font, otherwise the DrawStringProp() series of methods won't work properly.
 */

    void AnalyseOneSpacing( olc::Sprite *pSprite, olc::vi2d charPos, uint8_t &nLeftBlankCols, uint8_t &nCharWidth ) {

        int nFrstNonBlank = -1;
        int nLastNonBlank = -1;

        // start scanning for first non blank column from left to right
        bool bCurColIsBlank = true; // until proven otherwise
        for (int x = 0; x < 8 && bCurColIsBlank; x++) {
            for (int y = 0; y < 8 && bCurColIsBlank; y++) {
                olc::Pixel p = pSprite->GetPixel( charPos.x + x, charPos.y + y );
                bCurColIsBlank = (p == olc::BLACK || p == olc::BLANK);
            }
            if (!bCurColIsBlank) {
                nFrstNonBlank = x;
            }
        }

        // now scan right to left for first non blank column
        bCurColIsBlank = true;
        for (int x = 7; x >= 0 && bCurColIsBlank; x--) {
            for (int y = 0; y < 8 && bCurColIsBlank; y++) {
                olc::Pixel p = pSprite->GetPixel( charPos.x + x, charPos.y + y );
                bCurColIsBlank = (p == olc::BLACK || p == olc::BLANK);
            }
            if (!bCurColIsBlank) {
                nLastNonBlank = x;
            }
        }
        if (nFrstNonBlank == -1) {
            // space encountered
            nLeftBlankCols = 0;
            nCharWidth     = 3;
        } else {
            nLeftBlankCols = nFrstNonBlank;
            nCharWidth     = std::min( 8, nLastNonBlank - nFrstNonBlank + 2 );
        }
    }

    void AnalyseSpacings( olc::Sprite *pSprite ) {

        for (int y = 0; y < 6; y++) {
            for (int x = 0; x < 16; x++) {
                uint8_t x_spacing;
                uint8_t y_spacing;
                AnalyseOneSpacing( pSprite, olc::vi2d( x * 8, y * 8 ), x_spacing, y_spacing );

                std::cout << "0x" << std::to_string( x_spacing ) << std::to_string( y_spacing ) << ",";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // your update code per frame here

        // T is for Testing - outputs the data string for the pFontSprite to console
        if (GetKey( olc::Key::T ).bPressed) {
            ProcessSprite( pFontSprite );
        }

        // L is for Loading - reads in a text file and creates a sprite from it
        if (GetKey( olc::Key::L ).bPressed) {
            std::string sFileToLoad;
            std::cout << "Enter filename to load: ";
            std::cin >> sFileToLoad;
            delete pFontSprite;
            pFontSprite = TextfileToSprite( sFileToLoad );
        }

        // S is for Spacings data - generates and output the spacing data for the current
        // sprite to console
        if (GetKey( olc::Key::S ).bPressed) {
            AnalyseSpacings( pFontSprite );
        }

        // Rendering here
        Clear( olc::VERY_DARK_RED );
        DrawString( 10, 10, "the quick brown fox jumps over the lazy dog !@#$%^&*()`-=[];'\\,./ " );
        DrawString( 10, 20, "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG 1234567890~_+{}:\"|<>? ");

        DrawStringProp( 10, 30, "the quick brown fox jumps over the lazy dog !@#$%^&*()`-=[];'\\,./ " );
        DrawStringProp( 10, 40, "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG 1234567890~_+{}:\"|<>? ");

        DrawSprite( 10, 60, pFontSprite, 3 );

        return true;
    }

    bool OnUserDestroy() override {

        // your clean up code here
        delete pFontSprite;

        return true;
    }
};

// keep the screen dimensions constant and vary the resolution by adapting the pixel size
#define SCREEN_X   1200
#define SCREEN_Y    600
#define PIXEL_X       2
#define PIXEL_Y       2

int main()
{
	Font_Sprite2Data demo;
	if (demo.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y ))
		demo.Start();

	return 0;
}

