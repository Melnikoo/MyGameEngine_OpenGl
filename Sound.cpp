#include "Sound.h"

Sound::Sound()
{
	SoundEngine = createIrrKlangDevice();
	
}

void Sound::playMusic()
{
	music = SoundEngine->play3D("./audio/closeToJazz.mp3", vec3df(0.0, 0.0, 0.0), true, false, true);
}
