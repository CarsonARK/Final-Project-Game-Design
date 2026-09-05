#pragma once
#include <irrKlang.h>

class SoundManager {
public:
	static SoundManager& getSoundManager() {
		static SoundManager instance;
		return instance;
	}

	irrklang::ISoundEngine* getSoundEngine() {
		return engine;
	} 
	void playTankFireSound(irrklang::vec3df atLocation) {
		if (engine) {
			tankFiring->setDefaultVolume(1.0f);
			engine->setSoundVolume(1.0f);
			engine->play2D(tankFiring, false);
		}
	}
	void setListenerPosition(irrklang::vec3df position, irrklang::vec3df lookDirection, irrklang::vec3df UpVector) {
		if (engine) {
			engine->setListenerPosition(position, lookDirection, irrklang::vec3df(0, 0, 0), UpVector);
		}
	}
	/*
	* emitterSound = SoundManager::getSoundManager().getSoundEngine()->addSoundSourceFromFile((Aftr::ManagerEnvironmentConfiguration::getLMM() + "sounds/amulance1.wav").c_str());
	emitterSoundInstance = SoundManager::getSoundManager().getSoundEngine()->play3D(emitterSound, irrklang::vec3df(myCube->getPosition().x, myCube->getPosition().y, myCube->getPosition().z), true, false, true);
	emitterSoundInstance->setPosition(irrklang::vec3df(myCube->getPosition().x, myCube->getPosition().y, myCube->getPosition().z));
	*/
	void initializeSounds() {
		static bool initialized = false;
		if (engine && !initialized) {
			initialized = true;
			tankMoving = engine->addSoundSourceFromFile((Aftr::ManagerEnvironmentConfiguration::getLMM() + "sounds/tankmoving.wav").c_str());
			tankFiring = engine->addSoundSourceFromFile((Aftr::ManagerEnvironmentConfiguration::getLMM() + "sounds/ShellShockLiveFire.wav").c_str());

			tankMoving->setDefaultVolume(0.35f);

			playerOneTankMoving = engine->play3D(tankMoving, irrklang::vec3df(0, 0, 0), true, false, true);
			playerTwoTankMoving = engine->play3D(tankMoving, irrklang::vec3df(0, 0, 0), true, false, true);

			setTanksNotMoving();
		}
	}

	void setTanksNotMoving() {
		initializeSounds();

		if (playerOneTankMoving) {
			playerOneTankMoving->setIsPaused(true);
		}
		if (playerTwoTankMoving) {
			playerTwoTankMoving->setIsPaused(true);
		}
	}
	void setTankMoving(bool isPlayerOne, bool isMoving, irrklang::vec3df atPosition) {
		if (isPlayerOne) {
			if (playerOneTankMoving) {
				playerOneTankMoving->setIsPaused(!isMoving);
				playerOneTankMoving->setPosition(atPosition);
			}
		}
		else {
			if (playerTwoTankMoving) {
				playerTwoTankMoving->setIsPaused(!isMoving);
				playerTwoTankMoving->setPosition(atPosition);
			}
		}
	}

	irrklang::ISoundSource* tankMoving;
	irrklang::ISoundSource* tankFiring;
	irrklang::ISound* playerOneTankMoving;
	irrklang::ISound* playerTwoTankMoving;
private:
	SoundManager() {
		engine = irrklang::createIrrKlangDevice();
	}
	~SoundManager() {
		if (engine) {
			engine->drop();
			engine = nullptr;
		}
	}

	irrklang::ISoundEngine* engine = nullptr;
};