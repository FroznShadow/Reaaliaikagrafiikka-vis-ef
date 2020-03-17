//#ifndef AUDIO_HPP
//#define AUDIO_HPP
//
//#include <Core/Managers/EntityManager.hpp>
//#include <Core/AudioSystem.h>
//
//namespace Engine
//{
//	class Audio : public Component
//	{
//	public:
//		Audio(){}
//		virtual ~Audio() {};
//		virtual void Init() override;
//		virtual void Cleanup() override;
//
//		virtual void Update(DeltaTime deltaTime) override;
//		irrklang::ik_u32 getPlayPosition();
//		//play
//		void Play();
//		void Pause();
//		void Stop();
//		irrklang::ISound* getAudio();
//		irrklang::vec3df getPosition();
//		void setVolume(irrklang::ik_f32 volume);
//		void setAudio(Resource* audiores);
//		void setPosition(irrklang::vec3df position);
//	protected:
//	private:
//		irrklang::ISoundEngine* soundEngine;
//		irrklang::ISoundSource* soundSource;
//		irrklang::ISound* sound;
//		irrklang::vec3df position;
//		int volume;
//		bool isPlaying;
//		AudioSystem* as;
//
//	};
//	
//	inline void Audio::Init() {};
//	inline void Audio::Cleanup(){};
//	inline void Audio::setAudio(Resource* audiores){
//		as = audiores->getAudioSystem();
//		soundSource = audiores->getAudioData();
//	};
//
//	inline void Audio::Update(DeltaTime deltaTime) {};
//	inline irrklang::ik_u32 Audio::getPlayPosition(){ return sound->getPlayPosition(); };
//	inline void Audio::Play(){ as->audioPlay(sound); };
//	inline void Audio::Pause(){ as->audioPause(sound); };
//	inline void Audio::Stop(){ as->audioStop(sound); };
//	inline irrklang::ISound* Audio::getAudio(){ return sound; };
//	inline irrklang::vec3df Audio::getPosition(){ return position; };
//	inline void Audio::setVolume(irrklang::ik_f32 volume){ as->setVolume(sound, volume); };
//	inline void Audio::setPosition(irrklang::vec3df position){ as->setPosition(sound, position); };
//
//}
//#endif
