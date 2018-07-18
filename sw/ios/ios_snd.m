/*
** PROLOGUE:
**
** Implements the sound interface for the library.
** The implementation makes use of three different iOS systems to
** perform the functionality as follows..
**
** The AVAudioSession is used as required for iOS apps in order
** to coordinate sound playback on the device.
**
** The AVAudioPlayer interface is used to playback the background
** music. It can make use of hardware, and supports a wide 
** variety of sound formats, so it is ideal.
**
** Finally, all other sounds and effects are implemented using
** the OpenAL interface.
*/

/*
** PREREQUISITES
*/
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVAudioPlayer.h>

#include <assert.h>

#include "sw_def.h"
#include "sw_dbg.h"
#include "sw_snd.h"

#include "oal_snd.h"


/* 
** DEFINITIONS
*/

static AVAudioPlayer *  g_mus_player;
static int              g_mus_loop   = -1;
static float            g_mus_volume = 1.0;
static int              g_mus_is_playing = 0;

static SWsample         g_vo_sample;
static SWsound          g_vo_sound;
static int              g_vo_volume;
static int              g_vo_is_playing = 0;


/*
** LOCAL FUNCTIONS
*/

static void swSndInterruptionListener( void * inClientData, UInt32 inInterruptionState )
{
    /*
    ** Callback function invoked when the app is interrupted, and when it
    ** is restarted.
    */
    
	if( inInterruptionState == kAudioSessionBeginInterruption ) {
        swoalSndEnable( 0 );
	}
	else if( inInterruptionState == kAudioSessionEndInterruption )
	{
		OSStatus result = AudioSessionSetActive( true );
        if( result ) {
            SW_ERRF( "swSndInterruptionListener() - Error %d\n", (int)result );
        }
        swoalSndEnable( 1 );
	}
}

static void swSndInitSession( void )
{
    /*
    ** Apple recommends using the Audio session control for every app.
    ** The session is automatically created when the app is launched, so there is no
    ** need to explicitly free it when exiting. However, it is necessary to
    ** properly initialize it.
    **
    ** In our case, we set up an interruption listener to set the OpenAL context
    ** appropriately for an interruption, as well as configuring the background
    ** music so it behaves appropriately if the iPod is playing music.
    */
    
    OSStatus result;
    UInt32   size;
    UInt32   category;
    UInt32	 iPodIsPlaying;
    
    result = AudioSessionInitialize( NULL, NULL, swSndInterruptionListener, NULL );
    if( result ) {
        SW_ERRF( "AudioSessionInitialize() - Error %d\n", (int)result );
    }
    
    /*
    ** if the iPod is playing, use the ambient category to mix with it
    ** otherwise, use solo ambient to get the hardware for playing the app background track    
    */
    size = sizeof( iPodIsPlaying );
    
    result = AudioSessionGetProperty( kAudioSessionProperty_OtherAudioIsPlaying, &size, &iPodIsPlaying );
    if( result ) {
        SW_ERRF( "AudioSessionGetProperty() - Error %d\n", (int)result );
    }
            
    category = (iPodIsPlaying) ? kAudioSessionCategory_AmbientSound : kAudioSessionCategory_SoloAmbientSound;
            
    result = AudioSessionSetProperty( kAudioSessionProperty_AudioCategory, sizeof(category), &category );
    if( result ) {
        SW_ERRF( "AudioSessionSetProperty() - Error %d\n", (int)result );
    }
            
    result = AudioSessionSetActive( true );
    if( result ) {
        SW_ERRF( "AudioSessionSetActive() - Error %d\n", (int)result );
    }
}

static void swSndFreeSession( void )
{
    /*
    ** Audio session was created automatically by the system, so
    ** there is no need to explicitly free it.
    */
}


/*
** EXPORTED FUNCTIONS
*/

int swSndInit( void )
{
    swSndInitSession();
    swoalSndInit();
    
    g_mus_is_playing = 0; 
    g_mus_volume     = 1.0;
    g_mus_loop       = -1;
    
    g_vo_sound       = 0;
    g_vo_sample      = 0;
    g_vo_volume      = 1.0;
    g_vo_is_playing  = 0;
    
    return 1;
}

void swSndFree( void )
{
    swoalSndFree();
    swSndFreeSession();
}


SWsample swSamGen( const char * filename )
{
    /*
    ** Creates a new sample from the given sound file.
    ** Samples should contain only the name and extension of the file, and
    ** all samples should be uncompressed linear PCM 16 bit format.
    */
    
    assert( NULL != filename );
    
    NSString * ns_file = [NSString stringWithCString:filename encoding:NSASCIIStringEncoding];
    NSString * ns_name = [ns_file stringByDeletingPathExtension];
    NSString * ns_extn = [ns_file pathExtension];
    NSString * ns_path = [[NSBundle mainBundle] pathForResource:ns_name ofType:ns_extn];
    
    if( NULL == ns_path ) {
        SW_ERRF( "swSamGen( %s ) - File not found!\n", filename );
        return 0;
    }
    
	AudioFileID afid;
	NSURL * ns_url = [NSURL fileURLWithPath:ns_path];
	
	OSStatus result = AudioFileOpenURL((CFURLRef)ns_url, kAudioFileReadPermission, 0, &afid);
    
    if( result != 0 ) {
        SW_ERRF( "swSamGen( %s ) - File could not be opened!\n", filename );
        return 0;
    }

    /*
    ** Obtain the description of this sound sample.
    ** Native format for iOS is 16bit little endian. We will assume that the sound
    ** was created as little endian, but will add a check to ensure it is 16bit.
    */
    AudioStreamBasicDescription asbd;    
    UInt32 prop_siz = sizeof(AudioStreamBasicDescription);
    
    result = AudioFileGetProperty(afid, kAudioFilePropertyDataFormat, &prop_siz, &asbd);
    
    if( result != 0 ) {
        SW_ERRF("swSamGen( %s ) - Cannot obtain data format!.\n", filename);
    	AudioFileClose(afid);        
        return 0;
    }    
    if( asbd.mBitsPerChannel != 16 ) {
        SW_ERRF("swSamGen( %s ) - Sound is not 16bit\n", filename);
    	AudioFileClose(afid);
        return 0;        
    }
    if( asbd.mChannelsPerFrame > 2 ) {
        SW_ERRF("swSamGen( %s ) - Sound has more than 2 channels per frame.\n", filename);
    	AudioFileClose(afid);        
        return 0;        
    }
    
    /*
    ** Read the data from the file.
    ** 
    */
	prop_siz = sizeof(UInt64);
	UInt64 data_siz = 0;    
    
	result = AudioFileGetProperty(afid, kAudioFilePropertyAudioDataByteCount, &prop_siz, &data_siz);
    
    if( result != 0 ) {
        SW_ERRF("wSamGen( %s ) - Cannot obtain sound size in bytes.\n", filename);
    	AudioFileClose(afid);        
        return 0;
    }    
    
    UInt32 data_siz32 = (UInt32)data_siz;    
    ALvoid * pdata = malloc(data_siz32);
    
    if( NULL == pdata ) {
        SW_ERRF("swSamGen( %s ) - Memory allocation failure of %d bytes.\n", filename, data_siz32);
    	AudioFileClose(afid);
        return 0;       
    }
    
    result = AudioFileReadBytes(afid, FALSE, 0, &data_siz32, pdata);
    
    if( result != 0 ) {
        SW_ERRF("swSamGen( %s ) - Cannot read sound data.\n", filename);
    	AudioFileClose(afid);        
        free(pdata);
        return 0;
    }        
    AudioFileClose(afid);
    
    /*
    ** Generate an OpenAL sample handle from the retrieved data.
    */
    SWsample sample = 0;
    
    sample = swoalSamGen(
            asbd.mChannelsPerFrame,
            asbd.mSampleRate,
            data_siz32,
            pdata
            );
    
    free( pdata );
    
    return sample;
}


SWsample swSamGenCompressed( const char * filename )
{
    /*
    ** Create a new sample from a sound file containing a compressed data.
    ** Uses the Extended Audio File services to read in the compressed data, 
    ** and convert it to uncompressed linear PCM 16bit format, which is 
    ** suitable for playback.
    **
    ** The iOS system supports reading sound files in a wide
    ** variety of formats. However, the preferred format is to use
    ** IMA4 compression in CAF format. This creates a small file size and
    ** only requires minimal CPU processing for decompression.
    */

	OSStatus status = noErr;
    
    assert( NULL != filename );
    
    NSString * ns_file = [NSString stringWithCString:filename encoding:NSASCIIStringEncoding];
    NSString * ns_name = [ns_file stringByDeletingPathExtension];
    NSString * ns_extn = @".caf";
    NSString * ns_path = [[NSBundle mainBundle] pathForResource:ns_name ofType:ns_extn];
    
    if( NULL == ns_path )
    {
        SW_ERRF("swSamGenCompressed( %s ) - File not found.\n", filename );
        return 0;
    }
        
	ExtAudioFileRef	eafid  = NULL;        
	NSURL * ns_url = [NSURL fileURLWithPath:ns_path];
        
	status = ExtAudioFileOpenURL( (CFURLRef)ns_url, &eafid );
	if (status != noErr)
    {
        SW_ERRF("swSamGenCompressed( %s ) - File cannot be opened.\n", filename );
        return 0;
	}
    
    /*
    ** Determine the format for the compressed audio data.
    ** Limit sounds to having at most 2 channels per frame.
    */
    AudioStreamBasicDescription asbd_in;    
    UInt32 prop_size = sizeof(AudioStreamBasicDescription);    
    
	status = ExtAudioFileGetProperty( eafid, kExtAudioFileProperty_FileDataFormat, &prop_size, &asbd_in );
	if (status != noErr)
	{
        SW_ERRF("swSamGenCompressed( %s ) - Unable to read source file foramt.\n", filename );
        ExtAudioFileDispose( eafid );
        return 0;
	}
    
	if (asbd_in.mChannelsPerFrame > 2)
	{
        SW_ERRF("swSamGenCompressed( %s ) - Source format has more than 2 channels per frame.\n", filename );
        ExtAudioFileDispose( eafid );
        return 0;
	}
    
	AudioStreamBasicDescription asbd_out;
    
    /*
    ** Set the client format to 16 bit signed integer (native-endian) data
    ** Maintain the channel count and sample rate of the original source format
    */
    asbd_out.mSampleRate         = asbd_in.mSampleRate;
    asbd_out.mChannelsPerFrame   = asbd_in.mChannelsPerFrame;
    
    asbd_out.mFormatID           = kAudioFormatLinearPCM;
    asbd_out.mBytesPerPacket     = 2 * asbd_out.mChannelsPerFrame;
    asbd_out.mFramesPerPacket    = 1;
    asbd_out.mBytesPerFrame      = 2 * asbd_out.mChannelsPerFrame;
    asbd_out.mBitsPerChannel     = 16;
    asbd_out.mFormatFlags        = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
    
    status = ExtAudioFileSetProperty( eafid, kExtAudioFileProperty_ClientDataFormat, sizeof(asbd_out), &asbd_out );
    if (status != noErr)
    {
        SW_ERRF("swSamGenCompressed( %s ) - Unable to set client data format.\n", filename );
        ExtAudioFileDispose( eafid );
        return 0;
    }
    
	SInt64 frame_count = 0;    
    prop_size = sizeof(frame_count);
    
    status = ExtAudioFileGetProperty( eafid, kExtAudioFileProperty_FileLengthFrames, &prop_size, &frame_count);
    if (status != noErr)
    {
        SW_ERRF("swSamGenCompressed( %s ) - Unable to obtain file size.\n", filename );
        ExtAudioFileDispose( eafid );
        return 0;
    }
    
    UInt32 data_size = (UInt32) frame_count * asbd_out.mBytesPerFrame;
    void * theData = malloc(data_size);
    
    if( theData == NULL )
    {
        SW_ERRF("swSamGenCompressed( %s ) - Unable to allocate memory.\n", filename );
        ExtAudioFileDispose( eafid );
        return 0;
    }
    
    memset( theData, 0, data_size );
    
    AudioBufferList abl;
    
    abl.mNumberBuffers = 1;
    abl.mBuffers[0].mDataByteSize     = data_size;
    abl.mBuffers[0].mNumberChannels   = asbd_out.mChannelsPerFrame;
    abl.mBuffers[0].mData             = theData;
    
    status = ExtAudioFileRead( eafid, (UInt32 *)&frame_count, &abl );
    if(status != noErr)
    {
        SW_ERRF("swSamGenCompressed( %s ) - Unable to read and convert sound file.\n", filename );
        ExtAudioFileDispose( eafid );
        free( theData );
        return 0;
    }
    
    ExtAudioFileDispose( eafid );
    
    /*
    ** Generate the OpenAL sample from the collected data.
    */
    SWsample sample = 0;
    
    sample = swoalSamGen(
        asbd_out.mChannelsPerFrame,
        asbd_out.mSampleRate,
        data_size,
        theData
        );
    
    free( theData );
    
    return sample;
}
    
void swSamDel( SWsample sample )
{    
    swoalSamDel( sample );
}

void swSndGen( unsigned int count, SWsound * sounds )
{
    swoalSndGen( count, sounds );
}

void swSndDel( unsigned int count, SWsound * sounds )
{
    swoalSndDel( count, sounds );
}

void swSndSample( SWsound sound, SWsample sample )
{
    swoalSndSample( sound, sample );
}
void swSndPitch( SWsound sound, float pitch )
{
    swoalSndPitch( sound, pitch );
}
void swSndGain( SWsound sound, float gain )
{
    swoalSndGain( sound, gain );
}
void swSndLoop( SWsound sound, int loop )
{
    swoalSndLoop( sound, loop );
}
void swSndPlay( SWsound sound )
{
    swoalSndPlay( sound );
}
void swSndPause( SWsound sound )
{
    swoalSndPause( sound );
}
void swSndStop( SWsound sound )
{
    swoalSndStop( sound );
}
int swSndIsPlaying( SWsound sound )
{
    return swoalSndIsPlaying( sound );
}
int swSndIsPaused( SWsound sound )
{
    return swoalSndIsPaused( sound );
}


void swMusGen( const char * filename )
{
    /*
    ** Set up the music player using the given sound file.
    ** The music is played using the AVAudioPlayer object provided by iOS.
    ** It is capable of playing the sound using the hardware, and can handle a variety
    ** of sound formats.
    **
    ** We don't bother to check the format of the sound file given, but rely on
    ** the initialization to reveal if the sound file is compatible or not, and
    ** report an error if necessary.
    */

    swMusDel();  /* Stop any previous music playing */
    
    NSString * ns_file = [NSString stringWithCString:filename encoding:NSASCIIStringEncoding];
    NSString * ns_name = [ns_file stringByDeletingPathExtension];
    NSString * ns_extn = [ns_file pathExtension];
    NSString * ns_path = [[NSBundle mainBundle] pathForResource:ns_name ofType:ns_extn];
    
    if( NULL == ns_path )
    {
        SW_ERRF("swMusGen() - Music file %s not found\n", filename );
        return;
    }    
    
	NSURL * ns_url = [NSURL fileURLWithPath:ns_path];
    
    NSError * error;    
    g_mus_player = [[AVAudioPlayer alloc] initWithContentsOfURL:ns_url error:&error];
    
    if( NULL == g_mus_player )
    {
        SW_ERRF("swMusGen() - Unable to create music player with %s\n", filename);
    }
}

void swMusDel( void )
{
    if( g_mus_player != NULL )
    {
        [g_mus_player stop];
        [g_mus_player release];
        
        g_mus_is_playing = 0;
    }
    g_mus_player = NULL;
}


/*
** Sets various properties of the music player. 
** If the music player is currently playing, the changes are applied
** immediately.
*/
void swMusVolume( float volume )
{
    g_mus_volume = SW_CLAMP( volume, 0.0, 1.0 );
    
    if( g_mus_player && g_mus_is_playing )
    {
        [g_mus_player setVolume:g_mus_volume];
    }
}

void swMusLoop( int loop )
{
    g_mus_loop = ( loop == 0 ? 0 : -1 );
    
    if( g_mus_player && g_mus_is_playing )
    {
        [g_mus_player setNumberOfLoops:g_mus_loop];
    }    
}

void swMusPlay( void )
{
    if( g_mus_player )
    {
        if( g_mus_is_playing == 0 )
        {
            [g_mus_player setVolume:g_mus_volume];
            [g_mus_player setNumberOfLoops:g_mus_loop];
            [g_mus_player play];
            
            g_mus_is_playing = 1;
        }
        if( g_mus_is_playing == 2 )
        {
            [g_mus_player play];
            g_mus_is_playing = 1;
        }
    }
}

void swMusStop( void )
{
    if( g_mus_player && g_mus_is_playing )
    {
        [g_mus_player stop];
        g_mus_is_playing = 0;
    }        
}

void swMusPause( void )
{
    if( g_mus_player && g_mus_is_playing == 1 )
    {
        [g_mus_player pause];
        g_mus_is_playing = 2;
    }
}


void swVoGen( const char * filename )
{
    if( g_vo_sound != 0 )
        swSndStop( g_vo_sound );
    
    if( g_vo_sound == 0 )
        swSndGen( 1, &g_vo_sound );
    
    if( g_vo_sample != 0 )
        swSamDel( g_vo_sample );
    
    g_vo_sample = swSamGenCompressed( filename );
    
    swSndSample( g_vo_sound, g_vo_sample );
}

void swVoDel( void )
{
    if( g_vo_sound  != 0 )
        swSndStop( g_vo_sound );
    
    if( g_vo_sample != 0 )
    {
        swSndSample( g_vo_sound, 0 );
        swSamDel   ( g_vo_sample );
    }
    g_vo_sample = 0;
}

void swVoVolume( float volume )
{
    if( g_vo_sound != 0 )
        swSndGain( g_vo_sound, volume );
}

void swVoPlay ( void )
{
    swVoStop();
    
    if( g_vo_sound != 0 )
    {
        swSndPlay( g_vo_sound );
        g_vo_is_playing = 1;
    }
}

void swVoPause( void )
{
    swVoIsPlaying();
    
    if( g_vo_sound != 0 && g_vo_is_playing == 1 )
    {
        swSndPause( g_vo_sound );
        g_vo_is_playing = 2;
    }
}

void swVoStop ( void )
{
    if( g_vo_is_playing != 0 )
    {
        swSndStop( g_vo_sound );
        g_vo_is_playing = 0;
    }
}

int swVoIsPlaying( void )
{
    if( g_vo_is_playing == 1 )
    {
        if( swSndIsPlaying(g_vo_sound) == 0 )
            g_vo_is_playing = 0;
    }
    return (g_vo_is_playing != 0);
}







