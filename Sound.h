#ifndef SOUND_H
#define SOUND_H

#include <irrklang/irrKlang.h>
using namespace irrklang;
class Sound
{
public:
	float soundRadius;
	ISoundEngine* SoundEngine;
	ISound* music;
	Sound();

	void playMusic();

};

#endif