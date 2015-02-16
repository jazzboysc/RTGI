


#define __INSIDE_SCENEFILEDEFINEDINTERACTION_CPP_
#include "SceneFileDefinedInteraction.h"
#undef  __INSIDE_SCENEFILEDEFINEDINTERACTION_CPP_

// Include for FatalError()...
#include "Utils/ImageIO/imageIO.h"
#include "DataTypes/MathDefs.h"


void BindIncrementDecrementKey( char *varName, int key, int by, int limit );
void BindIncrementDecrementKey( char *varName, int key, float by, float limit );

SceneDefinedUI::SceneDefinedUI() 
{
	memset( keyMap, UI_UNKNOWN, __UI_FUNCTIONS * sizeof( int ) );
	translateScale[0] = translateScale[1] = translateScale[2] = 1;
	translateScale[3] = translateScale[4] = translateScale[5] = 1;
	rotateScale[0] = rotateScale[1] = rotateScale[2] = rotateScale[3] = 1;

	// Setup default key mappings here.
	keyMap[UI_QUIT		] = KEY_ESCAPE; 
}


SceneDefinedUI::SceneDefinedUI( FILE *f ) 
{
	memset( keyMap, UI_UNKNOWN, __UI_FUNCTIONS * sizeof( int ) );
	translateScale[0] = translateScale[1] = translateScale[2] = 1;
	translateScale[3] = translateScale[4] = translateScale[5] = 1;
	rotateScale[0] = rotateScale[1] = rotateScale[2] = rotateScale[3] = 1;

	// Search the scene file.
	char buf[ MAXLINELENGTH ], token[256], *ptr;
	while( fgets(buf, MAXLINELENGTH, f) != NULL )  
	{
		// Is this line a comment?
		ptr = StripLeadingWhiteSpace( buf );
		if (ptr[0] == '#' || ptr[0] == '\n' || ptr[0] == '\r' || ptr[0] == 0 ) continue;

		// Nope.  So find out what the command is...
		ptr = StripLeadingTokenToBuffer( ptr, token );
		MakeLower( token );
	
		// Take different measures, depending on the command.
		if (!strcmp(token,"end")) break;
		else if (!strcmp(token,"scale"))
		{
			// First read the function to map to
			ptr = StripLeadingTokenToBuffer( ptr, token );
			int func = MapStringToFunction( token );

			// Now find the scaling
			if ( func >= UI_TRANSLATE_RIGHT && func <= UI_TRANSLATE_BACK )
			{
				ptr = StripLeadingTokenToBuffer( ptr, token );
				translateScale[func-UI_TRANSLATE_RIGHT] = atof( token );
			}
			else if ( func >= UI_ROTATE_UP && func <= UI_ROTATE_RIGHT )
			{
				ptr = StripLeadingTokenToBuffer( ptr, token );
				rotateScale[func-UI_ROTATE_UP] = atof( token );
			}
		}
		else if (!strcmp(token,"map"))
		{
			// First read the function to map to
			ptr = StripLeadingTokenToBuffer( ptr, token );
			int func = MapStringToFunction( token );

			// Read the key (and modifiers) to map to that function
			//    If the first (or second or third) word is a modifer (e.g., alt
			//    control, shift), add it to the modifier list and keep looking for
			//    the actual key to bind.
			int key=-1, modifiers = KEY_NO_MODIFIER;
			bool isModifier;
			do
			{
				ptr = StripLeadingTokenToBuffer( ptr, token );
				key = MapStringToKey( token, true );
				isModifier = IsModifier( key );
				if (isModifier) modifiers |= key;
			} while (isModifier && ptr[0]);
			key = ApplyKeyModifiers( key, modifiers );

			// Assign the relevent key to the appropriate function.
			//    Note keyMap[0] is never used, so this still works 
			//    should MapStringToFunction() return UI_UNKNOWN.
			keyMap[func] = key > 0 ? key : KEY_UNKNOWN;
		}


		// These are not general.  They need to be customized in every
		//    application, by providing a
		else if (!strcmp(token,"incr") || !strcmp(token,"increment"))
		{
			int idelta=1, imaxVal=MAXINTEGER, key=-1, type;
			float fdelta=1, fmaxVal=MAXFLOAT;
			char varName[ 128 ];
			bool goodData=false;
			// Grab the datatype of the variable to increment.  This doesn't necessarily
			//    have to match the datatype declared...  Though behavior may be slightly
			//    undefined/unexpected if it doesn't match.
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			if (!strcmp(token,"int") || !strcmp(token,"integer")) type = 0;
			else if (!strcmp(token, "float") || !strcmp(token,"real") || !strcmp(token,"double")) type=1;
			else if (!strcmp(token, "bool") || !strcmp(token,"boolean")) type=2;
			else FatalError("Keymap control keyword 'incr' missing type identifier!");

			// Grab the variable name
			ptr = StripLeadingTokenToBuffer( ptr, varName );
			MakeLower( varName );

			// Search to see how to increment and what the max value is.
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			do 
			{
				goodData = false;
				if (!strcmp(token, "by"))
				{
					ptr = StripLeadingTokenToBuffer( ptr, token );
					if (type==0) // int
						idelta = atoi( token );
					else if (type==1) // float
						fdelta = (float) atof( token );
					else
						FatalError("Keymap control keyword 'incr' cannot specify 'by' for bool values!");
					goodData=true;
					ptr = StripLeadingTokenToBuffer( ptr, token );
					MakeLower( token );
				}
				else if (!strcmp(token, "max"))
				{
					ptr = StripLeadingTokenToBuffer( ptr, token );
					if (type==0) // int
						imaxVal = atoi( token );
					else if (type==1) // float
						fmaxVal = (float) atof( token );
					else
						FatalError("Keymap control keyword 'incr' cannot specify 'max' for bool values!");
					goodData=true;
					ptr = StripLeadingTokenToBuffer( ptr, token );
					MakeLower( token );
				}
			} while (goodData);

			// Read in the key....
			int modifiers = KEY_NO_MODIFIER;
			bool isModifier;
			do
			{
				goodData = ptr[0] > 0;
				key = MapStringToKey( token, true );
				isModifier = IsModifier( key );
				if (isModifier) modifiers |= key;
				ptr = StripLeadingTokenToBuffer( ptr, token );
			} while (isModifier && goodData);
			key = ApplyKeyModifiers( key, modifiers );
			key = key > 0 ? key : KEY_UNKNOWN;

			if (type != 1) 
				BindIncrementDecrementKey( varName, key, idelta, imaxVal );
			else 
				BindIncrementDecrementKey( varName, key, fdelta, fmaxVal );
		}
		else if (!strcmp(token,"decr") || !strcmp(token, "decrement"))
		{
			int idelta=1, iminVal=MININTEGER, key=-1, type;
			float fdelta=1, fminVal=-MAXFLOAT;
			char varName[ 128 ];
			bool goodData=false;
			// Grab the datatype of the variable to increment.  This doesn't necessarily
			//    have to match the datatype declared...  Though behavior may be slightly
			//    undefined/unexpected if it doesn't match.
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			if (!strcmp(token,"int") || !strcmp(token,"integer")) type = 0;
			else if (!strcmp(token, "float") || !strcmp(token,"real") || !strcmp(token,"double")) type=1;
			else if (!strcmp(token, "bool") || !strcmp(token,"boolean")) type=2;
			else FatalError("Keymap control keyword 'decr' missing type identifier!");

			// Grab the variable name
			ptr = StripLeadingTokenToBuffer( ptr, varName );
			MakeLower( varName );

			// Search to see how to increment and what the max value is.
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			do 
			{
				goodData = false;
				if (!strcmp(token, "by"))
				{
					ptr = StripLeadingTokenToBuffer( ptr, token );
					if (type==0) // int
						idelta = atoi( token );
					else if (type==1) // float
						fdelta = (float) atof( token );
					else
						FatalError("Keymap control keyword 'decr' cannot specify 'by' for bool values!");
					goodData=true;
					ptr = StripLeadingTokenToBuffer( ptr, token );
					MakeLower( token );
				}
				else if (!strcmp(token, "min"))
				{
					ptr = StripLeadingTokenToBuffer( ptr, token );
					if (type==0) // int
						iminVal = atoi( token );
					else if (type==1) // float
						fminVal = (float) atof( token );
					else
						FatalError("Keymap control keyword 'incr' cannot specify 'max' for bool values!");
					goodData=true;
					ptr = StripLeadingTokenToBuffer( ptr, token );
					MakeLower( token );
				}
			} while (goodData);

			// Read in the key....
			int modifiers = KEY_NO_MODIFIER;
			bool isModifier;
			do
			{
				goodData = ptr[0] > 0;
				key = MapStringToKey( token, true );
				isModifier = IsModifier( key );
				if (isModifier) modifiers |= key;
				ptr = StripLeadingTokenToBuffer( ptr, token );
			} while (isModifier && goodData);
			key = ApplyKeyModifiers( key, modifiers );
			key = key > 0 ? key : KEY_UNKNOWN;

			if (type != 1) 
				BindIncrementDecrementKey( varName, key, -idelta, iminVal );
			else 
				BindIncrementDecrementKey( varName, key, -fdelta, fminVal );
		}
		else
			printf("Error: Unknown command '%s' when loading SceneDefinedUI!\n", token);
	}


}




int SceneDefinedUI::MapStringToFunction( char *str ) const
{
	MakeLower( str );

	for (int i=1; i<__UI_FUNCTIONS;i++)
		if (!strcmp( str, uiFuncs[i].string )) return uiFuncs[i].uiID;

	return UI_UNKNOWN;
}


int SceneDefinedUI::MapStringToKey( char *str, bool verbose ) const
{
	if (!str)
	{
		if (verbose) printf("**** Error!  MapStringToKey() passed NULL string!\n");
		return 0;
	}

	MakeLower( str );

	int len = (int)strlen( str );
	if (len == 1) return str[0];

	for (int i=0;i<NUM_KEYSTRINGS;i++)
		if (!strcmp( str, keyStrings[i].string )) return keyStrings[i].uiID;

	if (verbose) printf("**** MapStringToKey() Error!  Unknown key '%s'.  Using '%c'.\n", str, str[0]);
	return str[0];
}



int SceneDefinedUI::ApplyKeyModifiers( int unmodifiedKey, int modifiers ) const
{
	return (unmodifiedKey & KEY_UNMODIFIED_MASK) | modifiers ;
}

bool SceneDefinedUI::IsModifier( int key ) const
{	
	return (key & KEY_MODIFIER_MASK) > 0;
}

int SceneDefinedUI::ConvertGLUTKey( unsigned char key, int modifiers ) const
{
	int theKey = (int)key;
	int ourModifiers = KEY_NO_MODIFIER;
	if (modifiers & GLUT_ACTIVE_SHIFT) ourModifiers |= KEY_MODIFIER_SHIFT;
	if (modifiers & GLUT_ACTIVE_CTRL)  ourModifiers |= KEY_MODIFIER_CONTROL;
	if (modifiers & GLUT_ACTIVE_ALT)   ourModifiers |= KEY_MODIFIER_ALT;

	if (key >= 'A' && key <= 'Z') theKey = (int)(key-'A'+'a');

	return (theKey & KEY_UNMODIFIED_MASK) | ourModifiers;
}

int SceneDefinedUI::ConvertGLUTModifiers( int modifiers ) const
{
	int ourModifiers = KEY_NO_MODIFIER;
	if (modifiers & GLUT_ACTIVE_SHIFT) ourModifiers |= KEY_MODIFIER_SHIFT;
	if (modifiers & GLUT_ACTIVE_CTRL)  ourModifiers |= KEY_MODIFIER_CONTROL;
	if (modifiers & GLUT_ACTIVE_ALT)   ourModifiers |= KEY_MODIFIER_ALT;
	return ourModifiers;
}

int SceneDefinedUI::ConvertGLUTTrackball( int button, int modifiers ) const
{
	int ourModifiers = KEY_NO_MODIFIER;
	if (modifiers & GLUT_ACTIVE_SHIFT) ourModifiers |= KEY_MODIFIER_SHIFT;
	if (modifiers & GLUT_ACTIVE_CTRL)  ourModifiers |= KEY_MODIFIER_CONTROL;
	if (modifiers & GLUT_ACTIVE_ALT)   ourModifiers |= KEY_MODIFIER_ALT;

	int base = MOUSE_LBUTTON;  
	if (button == GLUT_LEFT_BUTTON) base += 0;
	else if (button == GLUT_MIDDLE_BUTTON) base += 1;
	else if (button == GLUT_RIGHT_BUTTON) base += 2;

	int returnVal = (base & KEY_UNMODIFIED_MASK) | ourModifiers;

	for (int i = UI_EYE_TRACKBALL; i < UI_OBJECT_TRACKBALL_3; i++)
		if (keyMap[ i ] == returnVal) return returnVal;

	return KEY_UNKNOWN;
}

int SceneDefinedUI::ConvertGLUTMouseX( int button, int deltaX, int modifiers ) const
{
	int ourModifiers = KEY_NO_MODIFIER;
	if (modifiers & GLUT_ACTIVE_SHIFT) ourModifiers |= KEY_MODIFIER_SHIFT;
	if (modifiers & GLUT_ACTIVE_CTRL)  ourModifiers |= KEY_MODIFIER_CONTROL;
	if (modifiers & GLUT_ACTIVE_ALT)   ourModifiers |= KEY_MODIFIER_ALT;

	int base = MOUSE_BASE;  // +0 for X (+2 for Y)
	if (button == GLUT_LEFT_BUTTON) base += 4;
	else if (button == GLUT_MIDDLE_BUTTON) base += 8;
	else if (button == GLUT_RIGHT_BUTTON) base += 12;
	if (deltaX < 0) base++;

	return (base & KEY_UNMODIFIED_MASK) | ourModifiers;
}

int SceneDefinedUI::ConvertGLUTMouseY( int button, int deltaY, int modifiers ) const
{
	int ourModifiers = KEY_NO_MODIFIER;
	if (modifiers & GLUT_ACTIVE_SHIFT) ourModifiers |= KEY_MODIFIER_SHIFT;
	if (modifiers & GLUT_ACTIVE_CTRL)  ourModifiers |= KEY_MODIFIER_CONTROL;
	if (modifiers & GLUT_ACTIVE_ALT)   ourModifiers |= KEY_MODIFIER_ALT;

	int base = MOUSE_BASE + 2;  // +2 for Y (+0 for X)
	if (button == GLUT_LEFT_BUTTON) base += 4;
	else if (button == GLUT_MIDDLE_BUTTON) base += 8;
	else if (button == GLUT_RIGHT_BUTTON) base += 12;
	if (deltaY < 0) base++;

	return (base & KEY_UNMODIFIED_MASK) | ourModifiers;
}


int SceneDefinedUI::ConvertGLUTSpecialKey( int key, int modifiers ) const
{
	int theKey = KEY_UNKNOWN;
	int ourModifiers = KEY_NO_MODIFIER;
	if (modifiers & GLUT_ACTIVE_SHIFT) ourModifiers |= KEY_MODIFIER_SHIFT;
	if (modifiers & GLUT_ACTIVE_CTRL)  ourModifiers |= KEY_MODIFIER_CONTROL;
	if (modifiers & GLUT_ACTIVE_ALT)   ourModifiers |= KEY_MODIFIER_ALT;

	switch( key )
	{
	default:
		break;
	case GLUT_KEY_F1:
	case GLUT_KEY_F2:
	case GLUT_KEY_F3:
	case GLUT_KEY_F4:
	case GLUT_KEY_F5:
	case GLUT_KEY_F6:
	case GLUT_KEY_F7:
	case GLUT_KEY_F8:
	case GLUT_KEY_F9:
	case GLUT_KEY_F10:
	case GLUT_KEY_F11:
	case GLUT_KEY_F12:
		theKey = KEY_F1+key-1;
		break;
	case GLUT_KEY_LEFT:			theKey = KEY_LEFT_ARROW; break;
	case GLUT_KEY_UP:			theKey = KEY_UP_ARROW; break;
	case GLUT_KEY_RIGHT:		theKey = KEY_RIGHT_ARROW; break;
	case GLUT_KEY_DOWN:			theKey = KEY_DOWN_ARROW; break;
	case GLUT_KEY_PAGE_UP:		theKey = KEY_PAGE_UP; break;
	case GLUT_KEY_PAGE_DOWN:	theKey = KEY_PAGE_DOWN; break;
	case GLUT_KEY_INSERT:		theKey = KEY_INSERT; break;
	case GLUT_KEY_HOME:			theKey = KEY_HOME; break;
	case GLUT_KEY_END:			theKey = KEY_END; break;
	}

	return theKey != KEY_UNKNOWN ? ((theKey & KEY_UNMODIFIED_MASK) | ourModifiers) : KEY_UNKNOWN;
}


